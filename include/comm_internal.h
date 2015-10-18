/**
 * @file comm_internal.h
 * @brief
 * @author Travis Lane
 * @version 0.0.1
 * @date 2015-10-02
 */

#ifndef LONGBOARD_COMM_INTERNAL
#define LONGBOARD_COMM_INTERNAL

#include "comm.h"

typedef void (*lc_comm_generic_func)(struct lc_comm_t *);
typedef void (*lc_comm_get_float_func)(struct lc_comm_t*, float *out);

struct lc_comm_t {
  enum lc_comm_type_t lcc_type;
  void *lcc_ctx;

  lc_comm_generic_func lcc_delete_func;
};

struct lc_comm_bt_t {
  const char *lcc_bt_addr;
  int lcc_bt_socket;
};

struct lc_comm_t *lc_comm_new(enum lc_comm_type_t type, void *ctx,
                              lc_comm_generic_func delete_func);
void lc_comm_bt_delete(struct lc_comm_t *comm);

void lc_comm_open(struct lc_comm_t *comm);
void lc_comm_close(struct lc_comm_t *comm);

int lc_comm_get_power(struct lc_comm_t *comm, float *out_power);

#endif /* LONGBOARD_COMM_INTERNAL */
