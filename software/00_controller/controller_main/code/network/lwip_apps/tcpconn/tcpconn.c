#include "tcpconn.h"
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/api.h"

static void tcpconn_thread(void *arg)
{
  struct netconn *  conn;
  struct netconn *  newconn;
  struct netbuf  *  buf;
  err_t             err;
  void           *  data;
  u16_t             len;

  LWIP_UNUSED_ARG(arg);

  conn = netconn_new(NETCONN_TCP);
  netconn_bind(conn, IP_ADDR_ANY, 7);
  netconn_listen(conn);



  /// void netconn_set_recvtimeout ( struct netconn * aNetConn, int aTimeout );
  #define LWIP_SO_RCVTIME0                  1              // default is 0

  while (1)
  {
    err = netconn_accept(conn, &newconn);

    if (err == ERR_OK)
    {
      while ((err = netconn_recv(newconn, &buf)) == ERR_OK)
      {
        do
        {
             netbuf_data(buf, &data, &len);
             err = netconn_write(newconn, data, len, NETCONN_COPY);
        } while (netbuf_next(buf) >= 0);
        netbuf_delete(buf);
      }

      netconn_close(newconn);
      netconn_delete(newconn);
    }
  }
}

void tcpconn_init(void)
{
  sys_thread_new("tcpecho_thread", tcpconn_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}

