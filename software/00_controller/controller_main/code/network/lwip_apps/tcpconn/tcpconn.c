#include "tcpconn.h"
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/api.h"






void tcpconn_process_outgoing(struct netconn *)
{

}



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


  while (1)
  {
    err = netconn_accept(conn, &newconn);

    if (err == ERR_OK)
    {
      netconn_set_recvtimeout (newconn,1);

      do
      {
    	err = netconn_recv(newconn, &buf);
    	if(err == ERR_OK)
    	{
			do
			{
				 netbuf_data(buf, &data, &len);
				 err = netconn_write(newconn, data, len, NETCONN_COPY);
			} while (netbuf_next(buf) >= 0);
			netbuf_delete(buf);

			tcpconn_process_outgoing();
    	}
    	else if(err == ERR_TIMEOUT)
    	{
    		tcpconn_process_outgoing();
    	}
    	else
    	{
    		// Some other error, we treat it as broken connection
    		break;
    	}

      }while(1);

      netconn_close(newconn);
      netconn_delete(newconn);
    }
  }
}

void tcpconn_init(void)
{
  sys_thread_new("tcpcon n", tcpconn_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}

