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

typedef void (*lb_comm_delete_func)(struct lb_comm_t *);

struct lb_comm_t {
  enum lb_comm_type_t lbc_type;
  void *lbc_ctx;

  lb_comm_delete_func lbc_delete_func;
};

struct lb_comm_bt_t {
  const char *lbc_bt_addr;
  int lbc_bt_socket;
};

struct lb_comm_t *lb_comm_new(enum lb_comm_type_t type, void *ctx,
                              lb_comm_delete_func delete_func);
void lb_comm_bt_delete(struct lb_comm_t *comm);

#endif /* LONGBOARD_COMM_INTERNAL */
