/**
 * @file longboard_controller.h
 * @brief
 * @author Travis Lane
 * @version 0.0.1
 * @date 2015-10-01
 */

#ifndef LONGBOARD_CONTROLLER_H
#define LONGBOARD_CONTROLLER_H

struct lc_throttle_t *lc_throttle_new();
void lc_throttle_delete(struct lc_throttle_t *throttle);

#endif /* LONGBOARD_CONTROLLER_H */
