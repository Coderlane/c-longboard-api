/**
 * @file throttle.c
 * @brief
 * @author Travis Lane
 * @version 0.0.1
 * @date 2015-10-01
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <libusp/pwm.h>

#include "longboard_controller.h"
#include "longboard_controller_internal.h"

/**
 * @brief Create a new throttle.
 *
 * @return A new throttle.
 */
struct lc_throttle_t *
lc_throttle_new()
{
  struct lc_throttle_t *throttle;
  struct usp_pwm_list_t *pwm_list;
  struct usp_pwm_list_entry_t *pwm_entry;
  struct usp_pwm_t *pwm;

  throttle = calloc(sizeof(struct lc_throttle_t), 1);

  throttle->lct_pwm_controller = usp_controller_new();

  pwm_list = usp_controller_get_pwms(throttle->lct_pwm_controller);
  usp_pwm_list_foreach(pwm_list, pwm_entry)
  {
    pwm = usp_pwm_list_entry_get_pwm(pwm_entry);
    if (strcmp(usp_pwm_get_name(pwm), "odc1_pwm0") == 0) {
      usp_pwm_ref(pwm);
      throttle->lct_pwm_left = pwm;
    } else if (strcmp(usp_pwm_get_name(pwm), "odc1_pwm1") == 0) {
      usp_pwm_ref(pwm);
      throttle->lct_pwm_right = pwm;
    }
  }
  usp_pwm_list_unref(pwm_list);

  assert(throttle->lct_pwm_left != NULL);
  assert(throttle->lct_pwm_right != NULL);

  return throttle;
}

/**
 * @brief Delete a throttle.
 *
 * @param throttle The throttle to delete.
 */
void
lc_throttle_delete(struct lc_throttle_t *throttle)
{
  usp_controller_delete(throttle->lct_pwm_controller);
  usp_pwm_unref(throttle->lct_pwm_left);
  usp_pwm_unref(throttle->lct_pwm_right);
  free(throttle);
}
