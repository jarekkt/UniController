/*!
    \file burst_mux.c

    \brief
*/


#include "system.h"
#include "services.h"
#include "middleware.h"
#include "engine.h"
#include "burst_mux.h"
#include "string.h"



typedef struct
{
	uint32_t			address;
}burst_mux_t;

const char        hexd[] = "0123456789ABCDEF";



burst_mux_t  bmux;


void burst_mux_init()
{
	memset(&bmux,0,sizeof(bmux));

	// Selects address pair - for RS485 channels
	bmux.address  = 0; // todo
}

void burst_mux_once()
{

}


int32_t burst_mux_process_enc_crc(char * fstart,char ** fend)
{

	char            * var_crc;
    uint8_t           crc;
    uint8_t           crc_calc;
	char            * pBreake = 0;
	int				  ii;


	pBreake = strchr(fstart, ':');
	if(pBreake == NULL)
	{
	   // Wrong frame without crc
	   return -1;
	}
	else
	{
		*fend = pBreake;

	    *pBreake     = '\0';
	     var_crc      = pBreake + 1;

	    if(strcmp(var_crc,"$") != 0)
	    {
	       // No magic crc bypass
	       crc = (char)strtol(var_crc,NULL,16);
	       ii = 0;
	       crc_calc = 0;
	       while(fstart[ii] != 0)
	       {
	          crc_calc = crc_calc ^ fstart[ii++];
	       }

	       if(crc != crc_calc)
	       {
	          // Ignore frames with wrong CRC
	          return -1;
	       }
	    }
	}

	return 0;
}



int32_t burst_mux_process_enc_req(char * fstart,char * fend,burst_rcv_ctx_t *rcv_ctx)
{
	char            * pBreake;
	char            * var_name;
	char            * var_value;
    char              resp_value[128];
    char              resp_enc_value[128+32];
    int32_t		      resp_len;
    uint8_t           crc = 0;

	int32_t			  execute_store = -1;

	int				  ii;


	//  Check against	R/W request
	/*
			Find separator '='
	*/

	pBreake = strchr(fstart, '=');
	if(pBreake == NULL)
	{
			/*
					Read request
			 */
			var_name        = fstart;
			var_value       = NULL;
	}
	else
	{
			/*
					Write request. Extract parameter name and value.
			*/
			var_name        = fstart;
			*pBreake        = '\0';
			var_value       = pBreake + 1;
	}

	resp_len = 	burst_process_variable(var_name,var_value,resp_value,sizeof(resp_value),&execute_store);
	if( resp_len> 0)
	{
		if(rcv_ctx->frame_format == RCV_FRAME_DIRECT)
		{
			burst_rcv_send_response(rcv_ctx,resp_value,resp_len);
		}
		else
		{
			resp_len = snprintf(resp_enc_value,sizeof(resp_enc_value),"<%s=%s:$$>\r\n",var_name,resp_value);

		    if(resp_len >=6 )
		    {
		       // Prepare message with CRC
		       for(ii = 1;ii < resp_len-6;ii++)
		       {
		           crc ^= resp_enc_value[ii];
		       }

		       resp_enc_value[resp_len-5] = hexd[crc>>4];
		       resp_enc_value[resp_len-4] = hexd[crc&0x0F];
		    }

			burst_rcv_send_response(rcv_ctx,resp_enc_value,resp_len);
		}
	}

	return execute_store;

}



int32_t burst_mux_process_enc(char * fstart,char * fend,burst_rcv_ctx_t *rcv_ctx)
{
	int32_t execute_store = -1;

	if(burst_mux_process_enc_crc(fstart,&fend)== 0)
	{
		execute_store = burst_mux_process_enc_req(fstart,fend,rcv_ctx);
	}

	return execute_store;

}



int32_t  burst_mux_serial_process(uint32_t idx,char * buffer,uint32_t len)
{
	int32_t             execute_store = -1;
	burst_rcv_ctx_t		rcv_ctx;
	int32_t				curr = 0;
	int32_t				next = 0;
	char 			  * fstart;
	char 		      * fend;

	buffer[len] = 0;





	while(curr < len)
	{
		while(next<len)
		{
			if( (buffer[next] == '\n') || (buffer[next] == '&') )
			{
				buffer[next++] = 0;
				break;
			}
			next++;
		}

		rcv_ctx.channel	= idx;

		fstart = strchr(&buffer[curr],'<');
		fend  = strchr(&buffer[curr],'>');

		if( (fstart != NULL) && (fend != NULL))
		{
			// Got standard encapsulated frame
			rcv_ctx.frame_format = RCV_FRAME_ENCAPSULATED;
			execute_store = burst_mux_process_enc(fstart+1,fend,&rcv_ctx);
		}
		else
		{
			// Try for pure gcode - direct message
			rcv_ctx.frame_format = RCV_FRAME_DIRECT;
			gcode_engine_command(&buffer[curr],len-curr,&rcv_ctx);
		}

		curr = next;
	}

	return execute_store;
}










