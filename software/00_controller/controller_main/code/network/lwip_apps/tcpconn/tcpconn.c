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
	const char 	*	 conn_message;


	uint32_t		 port;

}tcpconn_data_t;



tcpconn_data_t	 tctx,tctd;


static void callback_null(const char * msg,uint32_t msg_len)
{

}

void tcpconn_callback(fn_cb_t fb_cb)
{
	tctx.callback_fn = fb_cb;
}

static void tcpconn_send_int(const char * msg,uint32_t msg_len,tcpconn_data_t * tct)
{

	uint32_t		  free_count;
	uint32_t		  part_count;

	if(tct->buf_head >=  tct->buf_tail)
	{
		free_count = DIM(tct->buf) - (tct->buf_head -  tct->buf_tail) -1;
	}
	else
	{
		free_count = tct->buf_tail - tct->buf_head-1;
	}

	if(msg_len > free_count)
	{
		msg_len = free_count;
	}

	if( tct->buf_head + msg_len >=  DIM(tct->buf))
	{
		part_count = DIM(tct->buf) - tct->buf_head;

		memcpy(&tct->buf[tct->buf_head],msg,part_count);
		memcpy(&tct->buf[0],&msg[part_count],msg_len - part_count);
		tct->buf_head =  msg_len - part_count;
	}
	else
	{
		memcpy(&tct->buf[tct->buf_head],msg,msg_len);
		tct->buf_head = tct->buf_head + msg_len;
	}
}


void tcpconn_send(const char * msg,uint32_t msg_len)
{
	tcpconn_send_int(msg,msg_len,&tctx);
}

void tcpconn_send_debug(const char * msg,uint32_t msg_len)
{
	tcpconn_send_int(msg,msg_len,&tctd);
}


void tcpconn_process_outgoing(struct netconn * nc,tcpconn_data_t * tct)
{
	err_t             err;
	uint32_t 		  head_flash;

	//TODO - process or at least count error

	head_flash = tct->buf_head;
	if(head_flash != tct->buf_tail)
	{
		if(head_flash  > tct->buf_tail)
		{
			err = netconn_write(nc, &tct->buf[tct->buf_tail], head_flash - tct->buf_tail, NETCONN_COPY);
			tct->buf_tail = head_flash;

			if(err !=0)
			{
				tct->err_cnt++;
			}
		}
		else
		{
			err = netconn_write(nc, &tct->buf[tct->buf_tail], DIM(tct->buf) - tct->buf_tail, NETCONN_COPY);
			tct->buf_tail = 0;
			if(err !=0)
			{
				tct->err_cnt++;
			}
			err = netconn_write(nc, &tct->buf[0], head_flash, NETCONN_COPY);
			tct->buf_tail = head_flash;
			{
				tct->err_cnt++;
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

  tcpconn_data_t *  ctx = (tcpconn_data_t *)arg;


  conn = netconn_new(NETCONN_TCP);
  netconn_bind(conn, IP_ADDR_ANY, ctx->port);
  netconn_listen(conn);


  while (1)
  {
    err = netconn_accept(conn, &newconn);

    if (err == ERR_OK)
    {
      // Inefficient, but simple
      // Used becasue we cannot use the socket from different threads :-(
      netconn_set_recvtimeout (newconn,1);


      tcpconn_send_int(ctx->conn_message,strlen(ctx->conn_message),ctx);

      do
      {
    	err = netconn_recv(newconn, &buf);
    	if(err == ERR_OK)
    	{
			do
			{
				 netbuf_data(buf, &data, &len);
				 ctx->callback_fn(data,len);
			} while (netbuf_next(buf) >= 0);
			netbuf_delete(buf);

			tcpconn_process_outgoing(newconn,ctx);
    	}
    	else if(err == ERR_TIMEOUT)
    	{
    		tcpconn_process_outgoing(newconn,ctx);
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
  memset(&tctd,0,sizeof(tctd));

  tctx.callback_fn = callback_null;
  tctx.port        = 23;

  tctd.callback_fn = callback_null;
  tctd.port        = 24;

  tctd.conn_message = "UniController starting...\r\n";
  tctx.conn_message = "Welcome to UniController main interface\r\n";

  tcpconn_send_int(tctd.conn_message,strlen(tctd.conn_message),&tctd);




}

void tcpconn_once(void)
{
  tctd.conn_message = "Welcome to UniController debug interface\r\n";


  sys_thread_new("tcpconn", tcpconn_thread, &tctx, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
  sys_thread_new("tcpconn_deb", tcpconn_thread, &tctd, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}


