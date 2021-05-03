#include "tcpconn.h"
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/api.h"

#include "string.h"
#include "system.h"

typedef struct
{
	fn_cb_t 		 callback_fn;
	char 			 buf[4096];
	uint32_t		 buf_tail;
	uint32_t		 buf_head;

	uint32_t 		 err_cnt;

}tcpconn_data_t;



tcpconn_data_t	 tctx;


static void callback_null(const char * msg,uint32_t msg_len)
{

}


void tcpconn_callback(fn_cb_t fb_cb)
{
	tctx.callback_fn = fb_cb;
}

void tcpconn_send(const char * msg,uint32_t msg_len)
{

	uint32_t		  free_count;
	uint32_t		  part_count;

	if(tctx.buf_head >=  tctx.buf_tail)
	{
		free_count = DIM(tctx.buf) - (tctx.buf_head -  tctx.buf_tail) -1;
	}
	else
	{
		free_count = tctx.buf_tail - tctx.buf_head-1;
	}

	if(msg_len > free_count)
	{
		msg_len = free_count;
	}

	if( tctx.buf_head + msg_len >=  DIM(tctx.buf))
	{
		part_count = DIM(tctx.buf) - tctx.buf_head;

		memcpy(&tctx.buf[tctx.buf_head],msg,part_count);
		memcpy(&tctx.buf[0],&msg[part_count],msg_len - part_count);
		tctx.buf_head =  msg_len - part_count;
	}
	else
	{
		memcpy(&tctx.buf[tctx.buf_head],msg,msg_len);
		tctx.buf_head = tctx.buf_head + msg_len;
	}
}


void tcpconn_process_incoming(void *  data,u16_t len)
{
	tctx.callback_fn(data,len);
}


void tcpconn_process_outgoing(struct netconn * nc)
{
	err_t             err;
	uint32_t 		  head_flash;

	//todo - process or at least count error

	head_flash = tctx.buf_head;
	if(head_flash != tctx.buf_tail)
	{
		if(head_flash  > tctx.buf_tail)
		{
			err = netconn_write(nc, &tctx.buf[tctx.buf_tail], head_flash - tctx.buf_tail, NETCONN_COPY);
			tctx.buf_tail = head_flash;

			if(err !=0)
			{
				tctx.err_cnt++;
			}
		}
		else
		{
			err = netconn_write(nc, &tctx.buf[tctx.buf_tail], DIM(tctx.buf) - tctx.buf_tail, NETCONN_COPY);
			tctx.buf_tail = 0;
			if(err !=0)
			{
				tctx.err_cnt++;
			}
			err = netconn_write(nc, &tctx.buf[0], head_flash, NETCONN_COPY);
			tctx.buf_tail = head_flash;
			{
				tctx.err_cnt++;
			}

		}
	}
}



static void tcpconn_thread(void *arg)
{
  struct netconn *  conn;
  struct netconn *  newconn;
  struct netbuf  *  buf;

  void           *  data;
  u16_t             len;
  err_t             err;

  LWIP_UNUSED_ARG(arg);

  conn = netconn_new(NETCONN_TCP);
  netconn_bind(conn, IP_ADDR_ANY, 23);
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
				 tcpconn_process_incoming(data,len);
			} while (netbuf_next(buf) >= 0);
			netbuf_delete(buf);

			tcpconn_process_outgoing(newconn);
    	}
    	else if(err == ERR_TIMEOUT)
    	{
    		tcpconn_process_outgoing(newconn);
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
    else
    {
    	vTaskDelay(100);
    }
  }
}

void tcpconn_init(void)
{
  memset(&tctx,0,sizeof(tctx));

  tctx.callback_fn = callback_null;

}

void tcpconn_once(void)
{
   sys_thread_new("tcpconn", tcpconn_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}


