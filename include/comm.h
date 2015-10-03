/**
 * @file comm.h
 * @brief
 * @author Travis Lane
 * @version 0.0.1
 * @date 2015-10-02
 */

#ifndef LONGBOARD_COMM_H
#define LONGBOARD_COMM_H

enum lc_comm_type_t { LC_COMM_BT };

struct lc_comm_t;

struct lc_comm_t *lc_comm_bt_new(const char *addr);
void lc_comm_delete(struct lc_comm_t *comm);

int lc_comm_bt_open(struct lc_comm_t *comm);
int lc_comm_bt_close(struct lc_comm_t *comm);
int lc_comm_bt_get_power(struct lc_comm_t *comm, float *out_power);

#endif /*LONGBOARD_COMM_H */
