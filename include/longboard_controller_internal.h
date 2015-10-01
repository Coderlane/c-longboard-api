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
#include <longboard_controller.h>


struct lc_motor_t {
  struct usp_pwm_t *lcm_pwm;

};

/**
 * @brief The master throttle
 */
struct lc_throttle_t {


};

#endif /* LONGBOARD_CONTROLLER_INTERNAL_H */
