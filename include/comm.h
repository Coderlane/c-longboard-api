/**
 * @file comm.h
 * @brief
 * @author Travis Lane
 * @version 0.0.1
 * @date 2015-10-02
 */

#ifndef LONGBOARD_COMM_H
#define LONGBOARD_COMM_H

enum lb_comm_type_t { LB_COMM_BT };

struct lb_comm_t;

struct lb_comm_t *lb_comm_bt_new(const char *addr);

int lb_comm_delete(struct lb_comm_t *comm);
int lb_comm_open(struct lb_comm_t *comm);
int lb_comm_close(struct lb_comm_t *comm);
int lb_comm_get_power(struct lb_comm_t *comm, float *out_power);

#endif /*LONGBOARD_COMM_H */
