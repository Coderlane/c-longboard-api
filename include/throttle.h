/**
 * @file longboard_throttle.h
 * @brief
 * @author Travis Lane
 * @version 0.0.1
 * @date 2015-10-01
 */

#ifndef LONGBOARD_THROTTLE_H
#define LONGBOARD_THROTTLE_H


/**
 * @brief The maximum amount of power to change by in 1 second.
 * This is power/s, which translates to real acceleration.
 */
#define LB_THROTTLE_ACCEL 10



/**
 * @brief The time in seconds to sleep before changing the power level.
 */
#define LB_THROTTLE_SEC_SLEEP 0

/**
 * @brief The time in nanoseconds to sleep before changing the power level.
 */
#define LB_THROTTLE_NSEC_SLEEP 100000000

struct lb_throttle_t *lb_throttle_new();
void lb_throttle_delete(struct lb_throttle_t *throttle);

int lb_throttle_power_set(struct lb_throttle_t *throttle, float power);
int lb_throttle_power_get(struct lb_throttle_t *throttle, float *out_power);

#endif /* LONGBOARD_THROTTLE_H */
