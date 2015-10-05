/**
 * @file longboard_controller_internal.h
 * @brief
 * @author Travis Lane
 * @version 0.0.1
 * @date 2015-09-29
 */

#ifndef LONGBOARD_THROTTLE_INTERNAL_H
#define LONGBOARD_THROTTLE_INTERNAL_H

#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>

#include "throttle.h"

struct usp_pwm_t;
struct usp_controller_t;

/**
 * @brief The master throttle
 */
struct lb_throttle_t {
  struct usp_controller_t *lbt_pwm_controller;

  struct usp_pwm_t *lbt_pwm_left;
  struct usp_pwm_t *lbt_pwm_right;

  float lbt_current_power;
  float lbt_target_power;
  float lbt_max_accel;

  bool lbt_running;
  pthread_t lbt_thread;
  pthread_mutex_t lbt_mutex;
};

int lb_throttle_stop_pwms(struct lb_throttle_t *throttle);
int lb_throttle_start_pwms(struct lb_throttle_t *throttle);

void* lb_throttle_runner(void *ctx);

bool lb_throttle_get_running(struct lb_throttle_t *throttle);
void lb_throttle_set_running(struct lb_throttle_t *throttle, bool running);

#endif /* LONGBOARD_THROTTLE_INTERNAL_H */
