/**
 * @file longboard_controller_internal.h
 * @brief
 * @author Travis Lane
 * @version 0.0.1
 * @date 2015-09-29
 */

#ifndef LONGBOARD_CONTROLLER_INTERNAL_H
#define LONGBOARD_CONTROLLER_INTERNAL_H

#include <stdint.h>
#include "controller.h"

struct usp_pwm_t;
struct usp_controller_t;

/**
 * @brief The master throttle
 */
struct lc_throttle_t {
  struct usp_controller_t *lct_pwm_controller;

  struct usp_pwm_t *lct_pwm_left;
  struct usp_pwm_t *lct_pwm_right;
};

void lc_throttle_fail(struct lc_throttle_t *throttle);

#endif /* LONGBOARD_CONTROLLER_INTERNAL_H */
