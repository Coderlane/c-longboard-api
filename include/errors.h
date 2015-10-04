/**
 * @file errors.h
 * @brief
 * @author Travis Lane
 * @version 0.0.1
 * @date 2015-10-02
 */

enum lb_error_t {
  LB_COMM_ERROR = -2,
  LB_PWM_ERROR = -1,
  LB_OK = 0,
  LB_RETRY = 2
};
