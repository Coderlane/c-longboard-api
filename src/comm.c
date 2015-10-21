/**
 * @file comm.c
 * @brief
 * @author Travis Lane
 * @version 0.0.1
 * @date 2015-10-02
 */

#include <assert.h>
#include <stdlib.h>

#include "comm.h"
#include "comm_internal.h"

/**
 * @brief Create a generic comm object.
 *
 * @param type The type of comm object.
 * @param ctx The context for the specific comm object.
 * @param delete_func The delete func for the comm object.
 *
 * @return A new comm object.
 */
struct lb_comm_t *
lb_comm_new(enum lb_comm_type_t type, void *ctx)
{
  struct lb_comm_t *comm;

  comm = calloc(sizeof(struct lb_comm_t), 1);
  assert(comm != NULL);

  comm->lbc_type = type;
  comm->lbc_ctx = ctx;

  return comm;
}

/**
 * @brief Delete a generic comm object.
 *
 * @param comm The comm object to delete.
 */
int
lb_comm_delete(struct lb_comm_t *comm)
{
  return comm->lbc_delete_func(comm);
}

int
lb_comm_open(struct lb_comm_t *comm)
{
  return comm->lbc_open_func(comm);
}

int
lb_comm_close(struct lb_comm_t *comm)
{
  return comm->lbc_close_func(comm);
}

int
lb_comm_get_power(struct lb_comm_t *comm, float *out_power)
{
  return comm->lbc_get_power_func(comm, out_power);
}
