#include "serial_srv.h"
#include "stm32h7xx_ll_usart.h"





typedef struct
{   
    xSemaphoreHandle    sema;   
    
    uint8_t               TxBuffer[256];
    int                   TxHead;
    int                   TxTail;

    USART_TypeDef	    * usart;
    serial_rec_char       rec_char;

}serial_usart_ctx_t;


typedef struct
{   
    serial_usart_ctx_t u1;
    serial_usart_ctx_t u6;

}srv_serial_data_t;





srv_serial_data_t  serial_pt;



void    srv_serial_low_init(serial_usart_ctx_t * ctx,USART_TypeDef * usart)
{
    memset(ctx,0,sizeof(*ctx));


    ctx->usart = usart;

    vSemaphoreCreateBinary(ctx->sema);
}

serial_usart_ctx_t   * srv_serial_id2ctx(uint32_t id)
{
    switch(id)
    {
        case SRV_SERIAL_UxART1_ID: return &serial_pt.u1;
        case SRV_SERIAL_UxART6_ID: return &serial_pt.u6;
    }

    return NULL;
}
void    srv_serial_baudrate(uint32_t port_id,uint32_t baudrate)
{
	LL_USART_InitTypeDef USART_InitStruct = {0};

    serial_usart_ctx_t  * ctx;

    ctx = srv_serial_id2ctx(port_id);
    if(ctx== NULL)
    {
      return;
    }

    USART_InitStruct.PrescalerValue 	 = LL_USART_PRESCALER_DIV1;
    USART_InitStruct.BaudRate 			 = baudrate;
    USART_InitStruct.DataWidth 			 = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits 			 = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity 			 = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection 	 = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling 		 = LL_USART_OVERSAMPLING_16;

    LL_USART_Init(ctx->usart, &USART_InitStruct);

}




/*!
    \brief  Low level serial_pt port initialization

*/

void    srv_serial_init(void)
{

    memset(&serial_pt,0,sizeof(serial_pt));
    
 

}

void    srv_serial_once(void)
{
    srv_serial_low_init(&serial_pt.u1,USART1);
    srv_serial_low_init(&serial_pt.u6,USART6);


   /* 
        Initialize port 
        Most already initialized by Cube
    */


    HAL_NVIC_SetPriority(USART1_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);

    LL_USART_EnableIT_RXNE_RXFNE(serial_pt.u1.usart);


    HAL_NVIC_SetPriority(USART6_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(USART6_IRQn);

    LL_USART_EnableIT_RXNE_RXFNE(serial_pt.u6.usart);
}






void    srv_serial_rcv_callback(uint32_t port_id,serial_rec_char rec_char)
{
    serial_usart_ctx_t  * ctx;

    ctx = srv_serial_id2ctx(port_id);
    if(ctx== NULL)
    {
      return;
    }

    ctx->rec_char = rec_char;

}

/*!
    \brief  Low level serial_pt port interrupt
            Our own, hal version not suitable 

*/
void ATTRIBUTE_IN_RAM UARTx_IRQHandler(uint32_t id,serial_usart_ctx_t * pctx)
{
  uint8_t          cc;
  portBASE_TYPE    hpt_woken1 = pdFALSE;  
  

  while( LL_USART_IsActiveFlag_RXNE_RXFNE(pctx->usart) != 0)
  {
    /* Read one byte from the receive data register */
    cc          = LL_USART_ReceiveData8(pctx->usart);
    pctx->rec_char(id,cc,&hpt_woken1);
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
      /* Disable the Transmit interrupt */
      LL_USART_DisableIT_TXE_TXFNF(pctx->usart);
      xSemaphoreGiveFromISR(pctx->sema,&hpt_woken1); 
    }
  }

  if( LL_USART_IsActiveFlag_ORE(pctx->usart) != 0)
  {
	  LL_USART_ClearFlag_ORE(pctx->usart);
  }

  if( LL_USART_IsActiveFlag_NE(pctx->usart) != 0)
  {
 	  LL_USART_ClearFlag_NE(pctx->usart);
  }

  if( LL_USART_IsActiveFlag_FE(pctx->usart) != 0)
  {
 	  LL_USART_ClearFlag_FE(pctx->usart);
  }

  portEND_SWITCHING_ISR(hpt_woken1);
  
}

void ATTRIBUTE_IN_RAM USART1_IRQHandler(void)
{
    UARTx_IRQHandler(SRV_SERIAL_UxART1_ID,&serial_pt.u1);
}


void ATTRIBUTE_IN_RAM USART6_IRQHandler(void)
{
    UARTx_IRQHandler(SRV_SERIAL_UxART6_ID,&serial_pt.u6);
}




/*!
    \brief  Low level serial_pt port send function (non blocking)

    \param buffer   Data buffer
    \param length   Data buffer size

*/

void    srv_serial_send(uint32_t port_id,const char * buffer,int length)
{
    int                 ii = 0;
    uint32_t            new_head;
    serial_usart_ctx_t  * ctx;

    ctx = srv_serial_id2ctx(port_id);
    if(ctx== NULL)
    {
      return;
    }

    LL_USART_DisableIT_TXE_TXFNF(ctx->usart);

    while(ii < length )
    {
        new_head = (ctx->TxHead + 1) % DIM(ctx->TxBuffer);

        if(new_head == ctx->TxTail)
        {
            // no more room
            break;
        }
        
        ctx->TxBuffer[ctx->TxHead] = buffer[ii++];
        ctx->TxHead = new_head;       
    }    

    LL_USART_EnableIT_TXE_TXFNF(ctx->usart);
    
}





/*!
    \brief  Low level serial_pt port send function (blocking)

    \param buffer   Data buffer
    \param length   Data buffer size

*/



void    srv_serial_puts(uint32_t port_id,const char * buffer,int length)
{
    serial_usart_ctx_t  * ctx;

    ctx = srv_serial_id2ctx(port_id);
    if(ctx== NULL)
    {
      return;
    }

    if(length <0)
    {
      length = strlen(buffer);
    }

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

