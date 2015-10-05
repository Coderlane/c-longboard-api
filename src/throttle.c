/**
 *
 * @file throttle.c
 * @brief
 * @author Travis Lane
 * @version 0.0.1
 * @date 2015-10-01
 */

#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <libusp/pwm.h>

#include "errors.h"
#include "throttle.h"
#include "throttle_internal.h"

/**
 * @brief Create a new throttle.
 *
 * @return A new throttle.
 */
struct lb_throttle_t *
lb_throttle_new()
{
  struct lb_throttle_t *throttle;
  struct usp_pwm_list_t *pwm_list;
  struct usp_pwm_list_entry_t *pwm_entry;
  struct usp_pwm_t *pwm;

  throttle = calloc(sizeof(struct lb_throttle_t), 1);

  throttle->lbt_pwm_controller = usp_controller_new();

  pwm_list = usp_controller_get_pwms(throttle->lbt_pwm_controller);
  usp_pwm_list_foreach(pwm_list, pwm_entry)
  {
    const char *name;
    pwm = usp_pwm_list_entry_get_pwm(pwm_entry);
    name = usp_pwm_get_name(pwm);

    if (strcmp(name, "odc1_pwm0") == 0) {
      usp_pwm_ref(pwm);
      throttle->lbt_pwm_left = pwm;
    } else if (strcmp(name, "odc1_pwm1") == 0) {
      usp_pwm_ref(pwm);
      throttle->lbt_pwm_right = pwm;
    }
  }
  usp_pwm_list_unref(pwm_list);

  assert(throttle->lbt_pwm_left != NULL);
  assert(throttle->lbt_pwm_right != NULL);

  throttle->lbt_running = true;
  throttle->lbt_max_accel = LB_THROTTLE_MAX_ACCEL;
  throttle->lbt_current_power = 0;
  throttle->lbt_target_power = 0;
  pthread_mutex_init(&(throttle->lbt_mutex), NULL);
  pthread_create(&(throttle->lbt_thread), NULL,
      lb_throttle_runner, throttle);

  return throttle;
}

/**
 * @brief Delete a throttle.
 *
 * @param throttle The throttle to delete.
 */
void
lb_throttle_delete(struct lb_throttle_t *throttle)
{
  lb_throttle_set_running(throttle, false);

  usp_controller_delete(throttle->lbt_pwm_controller);
  usp_pwm_unref(throttle->lbt_pwm_left);
  usp_pwm_unref(throttle->lbt_pwm_right);
  free(throttle);
}

/**
 * @brief Get the running state of a throttle.
 *
 * @param throttle The throttle to get the running state of.
 *
 * @return The running state of the throttle.
 */
bool
lb_throttle_get_running(struct lb_throttle_t *throttle)
{
  bool running;

  pthread_mutex_lock(&(throttle->lbt_mutex));
  running = throttle->lbt_running;
  pthread_mutex_unlock(&(throttle->lbt_mutex));

  return running;
}

/**
 * @brief Set the running state of a throttle. Note: This doesn't start
 * the runner thread, but it will stop it.
 *
 * @param throttle The throttle to change the running state of.
 * @param running The new running state.
 */
void
lb_throttle_set_running(struct lb_throttle_t *throttle, bool running)
{
  pthread_mutex_lock(&(throttle->lbt_mutex));
  throttle->lbt_running = running;
  pthread_mutex_unlock(&(throttle->lbt_mutex));
}

/**
 * @brief The throttle thread runner.
 *
 * @param ctx The throttle context.
 *
 * @return NULL
 */
void *
lb_throttle_runner(void *ctx)
{
  int rc;
  struct timespec sleep_in, sleep_out;
  struct timespec *sleep_in_ptr, *sleep_out_ptr, *sleep_tmp_ptr;
  struct lb_throttle_t *throttle = ctx;
  assert(throttle != NULL);

  while ((rc = lb_throttle_start_pwms(throttle)) != 0) {
    sleep(1);
  }

  while (lb_throttle_get_running(throttle) == true) {
    pthread_mutex_lock(&(throttle->lbt_mutex));
    if (throttle->lbt_current_power != throttle->lbt_target_power) {
      float diff = throttle->lbt_current_power - throttle->lbt_target_power;
      float current_power = throttle->lbt_target_power;

      if (fabsf(diff) > LB_THROTTLE_MAX_ACCEL) {
        current_power = diff > 0 ? current_power + LB_THROTTLE_MAX_ACCEL
                                 : current_power - LB_THROTTLE_MAX_ACCEL;
      }

      /* XXX: Handle failing to set the power better. */
      rc = lb_throttle_current_set(throttle, current_power);
      if(rc == LB_OK) {
        throttle->lbt_current_power = current_power;
      } else {
        throttle->lbt_current_power = 0.0f;
      }
    }
    pthread_mutex_unlock(&(throttle->lbt_mutex));

    sleep_in.tv_sec = LB_THROTTLE_SEC_SLEEP;
    sleep_in.tv_nsec = LB_THROTTLE_NSEC_SLEEP;

    sleep_in_ptr = &sleep_in;
    sleep_out_ptr = &sleep_out;

    while ((rc = nanosleep(sleep_in_ptr, sleep_out_ptr)) == EINTR) {
      sleep_tmp_ptr = sleep_in_ptr;
      sleep_in_ptr = sleep_out_ptr;
      sleep_out_ptr = sleep_tmp_ptr;
    }
  }

  return NULL;
}

/**
 * @brief Set the requested power level.
 *
 * @param throttle The throttle to set.
 * @param power The power level requested.
 *
 * @return A status code.
 */
int
lb_throttle_request_set(struct lb_throttle_t *throttle, float power)
{
  pthread_mutex_lock(&(throttle->lbt_mutex));
  throttle->lbt_target_power = power;
  pthread_mutex_unlock(&(throttle->lbt_mutex));
  return LB_OK;
}

/**
 * @brief Get the value of the requested power level.
 *
 * @param throttle The throttle to get the value of.
 * @param out_power The power level that is currently requested.
 *
 * @return A status code.
 */
int
lb_throttle_request_get(struct lb_throttle_t *throttle, float *out_power)
{
  pthread_mutex_lock(&(throttle->lbt_mutex));
  *out_power = throttle->lbt_target_power;
  pthread_mutex_unlock(&(throttle->lbt_mutex));
  return LB_OK;
}

/**
 * @brief Set the current power level of a throttle.
 *
 * @param throttle The throttle to set the power level of.
 * @param power The power level to set as a percentage.
 *
 * @return A status code.
 */
int
lb_throttle_current_set(struct lb_throttle_t *throttle, float power)
{
  int rc;

  rc = usp_pwm_set_duty_cycle(throttle->lbt_pwm_left, power);
  if (rc != USP_OK) {
    goto out;
  }

  rc = usp_pwm_set_duty_cycle(throttle->lbt_pwm_right, power);
  if (rc != USP_OK) {
    goto out;
  }

out:
  if(rc != 0) {
    rc = LB_PWM_ERROR;
    lb_throttle_stop_pwms(throttle);
  }

  return rc;
}

/**
 * @brief Get the current power level of a throttle.
 *
 * @param throttle The throttle to get the power level of.
 * @param out_power The power level of the throttle as a percentage.
 *
 * @return A status code.
 */
int
lb_throttle_current_get(struct lb_throttle_t *throttle, float *out_power)
{
  int rc;
  float power_left, power_right;

  rc = usp_pwm_get_duty_cycle(throttle->lbt_pwm_left, &power_left);
  if(rc != USP_OK) {
    goto out;
  }

  rc = usp_pwm_get_duty_cycle(throttle->lbt_pwm_right, &power_right);
  if(rc != USP_OK) {
    goto out;
  }

  if(power_left != power_right) {
    rc = LB_PWM_ERROR;
    goto out;
  }

out:
  if(rc != 0) {
    rc = LB_PWM_ERROR;
    lb_throttle_stop_pwms(throttle);
  } else {
    *out_power = power_left;
  }

  return rc;
}

/**
 * @brief Set the speeds on the pwms to 0, then disable them.
 *
 * @param throttle The throttle to stop_pwms out.
 */
int
lb_throttle_start_pwms(struct lb_throttle_t *throttle)
{
  int rc;

  rc = usp_pwm_set_duty_cycle(throttle->lbt_pwm_left, 0.0f);
  if (rc != 0) {
    goto out;
  }
  rc = usp_pwm_set_duty_cycle(throttle->lbt_pwm_right, 0.0f);
  if (rc != 0) {
    goto out;
  }

  rc = usp_pwm_enable(throttle->lbt_pwm_left);
  if (rc != 0) {
    goto out;
  }
  rc = usp_pwm_enable(throttle->lbt_pwm_right);
  if (rc != 0) {
    goto out;
  }

out:
  if (rc != 0) {
    lb_throttle_stop_pwms(throttle);
    rc = LB_PWM_ERROR;
  }

  return rc;
}

/**
 * @brief Set the speeds on the pwms to 0, then disable them.
 *
 * @param throttle The throttle to stop_pwms out.
 */
int
lb_throttle_stop_pwms(struct lb_throttle_t *throttle)
{
  int rc, rc_out = LB_OK;

  rc = usp_pwm_set_duty_cycle(throttle->lbt_pwm_left, 0.0f);
  if (rc != 0) {
    rc_out = LB_PWM_ERROR;
  }
  rc = usp_pwm_set_duty_cycle(throttle->lbt_pwm_right, 0.0f);
  if (rc != 0) {
    rc_out = LB_PWM_ERROR;
  }

  rc = usp_pwm_disable(throttle->lbt_pwm_left);
  if (rc != 0) {
    rc_out = LB_PWM_ERROR;
  }

  rc = usp_pwm_disable(throttle->lbt_pwm_right);
  if (rc != 0) {
    rc_out = LB_PWM_ERROR;
  }

  return rc_out;
}
