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
 * @brief Create a new comm object based on a bluetooth comm.
 *
 * @param addr The bluetooth address to connect to.
 *
 * @return A new comm object.
 */
struct lb_comm_t *
lb_comm_bt_new(const char *addr)
{
  struct lb_comm_bt_t *bt_comm;

  bt_comm = malloc(sizeof(struct lb_comm_bt_t));
  bt_comm->lbc_bt_addr = addr;
  bt_comm->lbc_bt_socket = -1;

  return lb_comm_new(LB_COMM_BT, bt_comm, lb_comm_bt_delete);
}

/**
 * @brief Delete a bluetooth comm object and the parent comm.
 *
 * @param comm The comm to delete.
 */
void
lb_comm_bt_delete(struct lb_comm_t *comm)
{
  struct lb_comm_bt_t *bt_comm = comm->lbc_ctx;
  assert(comm->lbc_type == LB_COMM_BT);

  if (bt_comm->lbc_bt_socket > 0) {
    lb_comm_bt_close(comm);
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
lb_comm_bt_open(struct lb_comm_t *comm)
{
  int sock = -1, rc = LB_OK;
  struct lb_comm_bt_t *bt_comm;
  struct sockaddr_rc bt_addr;
  struct timeval timeout;

  assert(comm->lbc_type == LB_COMM_BT);
  bt_comm = comm->lbc_ctx;

  timeout.tv_usec = 0;
  timeout.tv_sec = 2;

  // Clear the bt_addr
  memset(&bt_addr, 0, sizeof(bt_addr));

  // Create a socket to connect.
  sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
  if (sock < 0) {
    rc = LB_COMM_ERROR;
    goto out;
  }

  // Blocking comms is okay.
  // Set a timeout as necessary.
  setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval));

  // Set up the bt_addr
  bt_addr.rc_family = AF_BLUETOOTH;
  bt_addr.rc_channel = (uint8_t)1;
  str2ba(bt_comm->lbc_bt_addr, &(bt_addr.rc_bdaddr));

  // Connect the socket to the remote host.
  rc = connect(sock, (struct sockaddr *)&bt_addr, sizeof(bt_addr));
  if (rc != 0) {
    rc = LB_COMM_ERROR;
    goto out;
  }

out:
  if (rc != LB_OK) {
    close(sock);
    sock = -1;
  } else {
    bt_comm->lbc_bt_socket = sock;
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
lb_comm_bt_close(struct lb_comm_t *comm)
{
  struct lb_comm_bt_t *bt_comm;

  assert(comm->lbc_type == LB_COMM_BT);
  bt_comm = comm->lbc_ctx;

  close(bt_comm->lbc_bt_socket);
  bt_comm->lbc_bt_socket = -1;

  return LB_OK;
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
lb_comm_bt_get_power(struct lb_comm_t *comm, float *out_power)
{
  ssize_t size_read, left;
  struct lb_comm_bt_t *bt_comm;
  char *start;
  char buf[32];

  assert(comm->lbc_type == LB_COMM_BT);
  bt_comm = comm->lbc_ctx;

  if(bt_comm->lbc_bt_socket < 0) {
    return LB_COMM_ERROR;
  }

  left = sizeof(buf);
  start = buf;

  while ((size_read = read(bt_comm->lbc_bt_socket, start, left)) > 0) {
    if (strchr(start, '\n') != NULL) {
      sscanf(buf, "%f", out_power);
      return LB_OK;
    } else {
      start += size_read;
      left -= size_read;
      if (left <= 0) {
        /*
         * XXX: Hack, better validation would be nice.
         * Overflowed the line, retry.
         */
        return LB_RETRY;
      }
    }
  }

  /*
   * Failed reading somewhere. This is probably a socket error.
   */
  return LB_COMM_ERROR;
}
