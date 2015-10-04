/**
 *
 * @file throttle.c
 * @brief
 * @author Travis Lane
 * @version 0.0.1
 * @date 2015-10-01
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
    pwm = usp_pwm_list_entry_get_pwm(pwm_entry);
    if (strcmp(usp_pwm_get_name(pwm), "odc1_pwm0") == 0) {
      usp_pwm_ref(pwm);
      throttle->lbt_pwm_left = pwm;
    } else if (strcmp(usp_pwm_get_name(pwm), "odc1_pwm1") == 0) {
      usp_pwm_ref(pwm);
      throttle->lbt_pwm_right = pwm;
    }
  }
  usp_pwm_list_unref(pwm_list);

  assert(throttle->lbt_pwm_left != NULL);
  assert(throttle->lbt_pwm_right != NULL);


  throttle->lbt_running = true;

  pthread_create (&(throttle->lbt_thread), NULL,
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
  throttle->lbt_running = false;
  usp_controller_delete(throttle->lbt_pwm_controller);
  usp_pwm_unref(throttle->lbt_pwm_left);
  usp_pwm_unref(throttle->lbt_pwm_right);
  free(throttle);
}

void *
lb_throttle_runner(void *ctx)
{
  struct lb_throttle_t *throttle = ctx;
  assert(throttle != NULL);


   while(throttle->lbt_running) {


   }

  return NULL;
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
lb_throttle_power_set(struct lb_throttle_t *throttle, float power)
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
    lb_throttle_fail(throttle);
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
lb_throttle_power_get(struct lb_throttle_t *throttle, float *out_power)
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
    lb_throttle_fail(throttle);
  } else {
    *out_power = power_left;
  }

  return rc;
}

/**
 * @brief Set the speeds on the pwms to 0, then disable them.
 *
 * @param throttle The throttle to fail out.
 */
void
lb_throttle_fail(struct lb_throttle_t *throttle)
{
  usp_pwm_set_duty_cycle(throttle->lbt_pwm_left, 0.0f);
  usp_pwm_set_duty_cycle(throttle->lbt_pwm_right, 0.0f);

  usp_pwm_disable(throttle->lbt_pwm_left);
  usp_pwm_disable(throttle->lbt_pwm_right);
}
