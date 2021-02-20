#include "serial_485_srv.h"
#include "services.h"


typedef struct
{    
    xSemaphoreHandle    sema;   
 
    
    serial_rec_485_char rcv_char;

    uint8_t   TxBuffer[128];
    int       TxHead;
    int       TxTail;

    USART_TypeDef     * usart;
    GPIO_TypeDef      * gpio;
    uint16_t            pin;

}srv_serial_485_ctx_t;

typedef struct
{   
    srv_serial_485_ctx_t u2;
}srv_serial_485_data_t;



static srv_serial_485_data_t  serial_485;


srv_serial_485_ctx_t   * srv_serial_485_id2ctx(uint32_t id)
{
    switch(id)
    {
        case SRV_SERIAL_485_UxART1_ID: return &serial_485.u2;
    }

    return NULL;
}


void    srv_serial_485_low_init(srv_serial_485_ctx_t * ctx)
{
	// Enable and configure RS485 DE signal hardware control
	// Time is in sample units ( 1/16 character time)
	LL_USART_SetDEDeassertionTime(ctx->usart,8);
	LL_USART_SetDEAssertionTime(ctx->usart,8);
	LL_USART_EnableDEMode(ctx->usart);

    vSemaphoreCreateBinary(ctx->sema);
}



void    srv_serial_485_enable(uint32_t port_id,int enable)
{
    srv_serial_485_ctx_t  * ctx;

    ctx = srv_serial_485_id2ctx(port_id);
    if(ctx== NULL)
    {
      fw_assert(0);
      return;
    }
    
    if(enable != 0)
    {
    	LL_USART_EnableIT_RXNE_RXFNE(ctx->usart);
    }
    else
    {
    	LL_USART_DisableIT_RXNE_RXFNE(ctx->usart);
    }

}


/*!
    \brief  Low level serial_485 port initialization

*/

void    srv_serial_485_init(void)
{

    memset(&serial_485,0,sizeof(serial_485));
    
   


}

void    srv_serial_485_once(void)
{

    // serial_485 ports already low level initialized by Cube


    /* Initialize port engines */

    srv_serial_485_low_init(&serial_485.u2);


    /* Enable port interrupt */
    HAL_NVIC_SetPriority(USART2_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);


    // Note - interrupts must be enabled with call to srv_serial_485_enable()
}


void    srv_serial_485_rcv_callback(uint32_t port_id,serial_rec_485_char  rcv_char)
{
    srv_serial_485_ctx_t  * ctx;

    ctx = srv_serial_485_id2ctx(port_id);
    if(ctx== NULL)
    {
      fw_assert(0);
      return;
    }

    ctx->rcv_char = rcv_char;
}


/*!
    \brief  Low level serial_485 port interrupt
            Our own, hal version not suitable 

*/




void UARTx_485_IRQHandler(uint32_t id,srv_serial_485_ctx_t  * pctx)
{
  uint8_t          cc;
  portBASE_TYPE    hpt_woken1 = pdFALSE;  
  
  while( LL_USART_IsActiveFlag_RXNE_RXFNE(pctx->usart) != 0 )
  {
    /* Read one byte from the receive data register */
    cc = LL_USART_ReceiveData8(pctx->usart);
    
    if(pctx->rcv_char != NULL)
    {
        pctx->rcv_char(id,cc,&hpt_woken1);
    }
  }

  if( LL_USART_IsActiveFlag_TXE_TXFNF(pctx->usart) != 0)
  {
    /* Write one byte to the transmit data register */
    if(pctx->TxHead != pctx->TxTail)
    {
      LL_USART_TransmitData8(pctx->usart,pctx->TxBuffer[pctx->TxTail] & 0x00FF);
      pctx->TxTail = ( pctx->TxTail + 1) % DIM(pctx->TxBuffer);
    }
    else
    {
    	LL_USART_DisableIT_TXE_TXFNF(pctx->usart);
    	LL_USART_EnableIT_TC(pctx->usart);
    }
  }

  if( LL_USART_IsActiveFlag_TC(pctx->usart) != 0)
  {
      /* Disable the Transmit interrupt */
      LL_USART_DisableIT_TXE_TXFNF(pctx->usart);

      xSemaphoreGiveFromISR(pctx->sema,&hpt_woken1); 
  }


  portEND_SWITCHING_ISR(hpt_woken1);
}


void USART2_IRQHandler(void)
{
   UARTx_485_IRQHandler(SRV_SERIAL_485_UxART1_ID,&serial_485.u2);
}


static int srv_serial_485_send_cc(srv_serial_485_ctx_t  * ctx,char cc)
{
    uint32_t                new_head;

    new_head = (ctx->TxHead + 1) % DIM(ctx->TxBuffer);

    if(new_head == ctx->TxTail)
    {
        // no more room
       return -1;
    }
    ctx->TxBuffer[ctx->TxHead] = cc;
    ctx->TxHead = new_head;

    return 0;
}


/*!
    \brief  Low level serial_485 port send function (non blocking)

    \param buffer   Data buffer
    \param length   Data buffer size

*/

void    srv_serial_485_send(uint32_t port_id,const char * buffer,int length)
{
    int                     ii = 0;

    srv_serial_485_ctx_t  * ctx;

    ctx = srv_serial_485_id2ctx(port_id);
    if(ctx== NULL)
    {
      fw_assert(0);
      return;
    }

    LL_USART_DisableIT_TXE_TXFNF(ctx->usart);


    if(length == 0)
    {
    	length = strlen(buffer);
    }



    // send remaining characters
    while(ii < length )
    {
        if(srv_serial_485_send_cc(ctx,buffer[ii++]) != 0)
        {
            // no more room
            break;
        }
    }    

    // Send extra dummy character (post)
    srv_serial_485_send_cc(ctx,' ');

    LL_USART_EnableIT_TXE_TXFNF(ctx->usart);
    
}


/*!
    \brief  Low level serial_485 port send function (blocking)

    \param buffer   Data buffer
    \param length   Data buffer size

*/



void    srv_serial_485_puts(uint32_t port_id,const char * buffer,int length)
{
    srv_serial_485_ctx_t  * ctx;

    ctx = srv_serial_485_id2ctx(port_id);
    if(ctx== NULL)
    {
      fw_assert(0);
      return;
    }

    if(length == -1)
    {
      length = strlen(buffer);
    }


    // Clear current transaction - if any 
    ctx->TxHead = 0;
    ctx->TxTail = 0;

    while(length > 0)
    {
		if( LL_USART_IsActiveFlag_TXE_TXFNF(ctx->usart) != 0)
		{
			LL_USART_TransmitData8(ctx->usart,(*buffer) & 0x00FF);
			buffer++;
			length--;
		}
    }



}

/*!
    \brief  Low level serial_485 port send function. Blocks if there is transmission in progress

    \param buffer   Data buffer
    \param length   Data buffer size

*/
void    srv_serial_485_send_blocked(uint32_t port_id,const char * buffer,int length)
{
    srv_serial_485_ctx_t  * ctx;

    ctx = srv_serial_485_id2ctx(port_id);
    if(ctx== NULL)
    {
      fw_assert(0);
      return;
    }

    xSemaphoreTake(ctx->sema,portMAX_DELAY);
    srv_serial_485_send(port_id,buffer,length);
}
