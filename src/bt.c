/**
 * @file bt.c
 * @brief
 * @author Travis Lane
 * @version 0.0.1
 * @date 2015-10-02
 */

#include <sys/socket.h>
#include <sys/time.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "comm.h"
#include "comm_internal.h"
#include "errors.h"

/**
 * @brief Create a generic comm object.
 *
 * @param type The type of comm object.
 * @param ctx The context for the specific comm object.
 * @param delete_func The delete func for the comm object.
 *
 * @return A new comm object.
 */
struct lc_comm_t *
lc_comm_new(enum lc_comm_type_t type, void *ctx,
            lc_comm_delete_func delete_func)
{
  struct lc_comm_t *comm;
  comm = malloc(sizeof(struct lc_comm_t));
  assert(comm != NULL);

  comm->lcc_type = type;
  comm->lcc_ctx = ctx;

  comm->lcc_delete_func = delete_func;

  return comm;
}

/**
 * @brief Delete a generic comm object.
 *
 * @param comm The comm object to delete.
 */
void
lc_comm_delete(struct lc_comm_t *comm)
{
  comm->lcc_delete_func(comm);
}

/**
 * @brief Create a new comm object based on a bluetooth comm.
 *
 * @param addr The bluetooth address to connect to.
 *
 * @return A new comm object.
 */
struct lc_comm_t *
lc_comm_bt_new(const char *addr)
{
  struct lc_comm_bt_t *bt_comm;

  bt_comm = malloc(sizeof(struct lc_comm_bt_t));
  bt_comm->lcc_bt_addr = addr;
  bt_comm->lcc_bt_socket = -1;

  return lc_comm_new(LC_COMM_BT, bt_comm, lc_comm_bt_delete);
}

/**
 * @brief Delete a bluetooth comm object and the parent comm.
 *
 * @param comm The comm to delete.
 */
void
lc_comm_bt_delete(struct lc_comm_t *comm)
{
  struct lc_comm_bt_t *bt_comm = comm->lcc_ctx;
  assert(comm->lcc_type == LC_COMM_BT);

  if (bt_comm->lcc_bt_socket > 0) {
    lc_comm_bt_close(comm);
  }

  free(bt_comm);
  free(comm);
}

/**
 * @brief Open a bluetooth socket.
 *
 * @param comm The comm object to open the socket on.
 *
 * @return A status code.
 */
int
lc_comm_bt_open(struct lc_comm_t *comm)
{
  int sock = -1, rc = LC_OK;
  struct lc_comm_bt_t *bt_comm;
  struct sockaddr_rc bt_addr;
  struct timeval timeout;

  assert(comm->lcc_type == LC_COMM_BT);
  bt_comm = comm->lcc_ctx;

  timeout.tv_usec = 0;
  timeout.tv_sec = 2;

  // Clear the bt_addr
  memset(&bt_addr, 0, sizeof(bt_addr));

  // Create a socket to connect.
  sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
  if (sock < 0) {
    rc = LC_COMM_ERROR;
    goto out;
  }

  // Blocking comms is okay.
  // Set a timeout as necessary.
  setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval));

  // Set up the bt_addr
  bt_addr.rc_family = AF_BLUETOOTH;
  bt_addr.rc_channel = (uint8_t)1;
  str2ba(bt_comm->lcc_bt_addr, &(bt_addr.rc_bdaddr));

  // Connect the socket to the remote host.
  rc = connect(sock, (struct sockaddr *)&bt_addr, sizeof(bt_addr));
  if (rc != 0) {
    rc = LC_COMM_ERROR;
    goto out;
  }

out:
  if (rc != LC_OK) {
    close(sock);
    sock = -1;
  } else {
    bt_comm->lcc_bt_socket = sock;
  }

  return rc;
}

/**
 * @brief Close a bluetooth socket.
 *
 * @param comm The comm object to close the socket on.
 *
 * @return A status code.
 */
int
lc_comm_bt_close(struct lc_comm_t *comm)
{
  struct lc_comm_bt_t *bt_comm;

  assert(comm->lcc_type == LC_COMM_BT);
  bt_comm = comm->lcc_ctx;

  close(bt_comm->lcc_bt_socket);
  bt_comm->lcc_bt_socket = -1;

  return LC_OK;
}

/**
 * @brief Read a power level from the bluetooth socket.
 *
 * @param comm The comm object to read.
 * @param out_power The power level read.
 *
 * @return A status code.
 */
int
lc_comm_bt_get_power(struct lc_comm_t *comm, float *out_power)
{
  ssize_t size_read, left;
  struct lc_comm_bt_t *bt_comm;
  char *start;
  char buf[32];

  assert(comm->lcc_type == LC_COMM_BT);
  bt_comm = comm->lcc_ctx;

  if(bt_comm->lcc_bt_socket < 0) {
    return LC_COMM_ERROR;
  }

  left = sizeof(buf);
  start = buf;

  while ((size_read = read(bt_comm->lcc_bt_socket, start, left)) > 0) {
    if (strchr(start, '\n') != NULL) {
      sscanf(buf, "%f", out_power);
      return LC_OK;
    } else {
      start += size_read;
      left -= size_read;
      if (left <= 0) {
        /*
         * XXX: Hack, better validation would be nice.
         * Overflowed the line, retry.
         */
        return LC_RETRY;
      }
    }
  }

  /*
   * Failed reading somewhere. This is probably a socket error.
   */
  return LC_COMM_ERROR;
}
