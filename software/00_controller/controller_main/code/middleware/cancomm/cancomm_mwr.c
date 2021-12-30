 /*!
    \file cancommt_mwr.c
*/


#include "services.h"
#include "middleware.h"


#define  MWR_CANCOMM_CH_CNT				1
#define  MWR_CANCOMM_RING_SIZE         20


typedef struct
{  

   FDCAN_HandleTypeDef  * CanHandle;
   FDCAN_FilterTypeDef    sFilterConfig;

   FDCAN_TxHeaderTypeDef  TxHeader;
   uint8_t                TxData[8];

   FDCAN_RxHeaderTypeDef  RxHeader;
   uint8_t                RxData[8];



   uint32_t               rcv_cntr;
   uint32_t               err_cntr;
   uint32_t               txc_cntr;
   uint32_t               ovrerr_cntr;

   uint32_t               test_mask;



   can_frame_t            cring[MWR_CANCOMM_RING_SIZE];
   uint32_t               cring_tail;
   uint32_t               cring_head;

  
}mwr_cancomm_channel_t;

typedef struct
{
	 mwr_cancomm_channel_t  canCh[MWR_CANCOMM_CH_CNT];
    uint32_t                can_test;
}mwr_cancomm_type;


mwr_cancomm_type      ccm;


const var_ptable_t   mwr_cancomm_var_ptable[] SERMON_ATTR =  
{     
  { "can_test",             &ccm.can_test             ,E_VA_INT_FREE}
};


FDCAN_HandleTypeDef hfdcan1;

static void mwr_cancomm_task_can1(void * params);


static void mwr_cancomm_config(void)
{


  /* Bit time configuration:
	fdcan_ker_ck               = 40 MHz
	Time_quantum (tq)          = 25 ns
	Synchronization_segment    = 1 tq
	Propagation_segment        = 23 tq
	Phase_segment_1            = 8 tq
	Phase_segment_2            = 8 tq
	Synchronization_Jump_width = 8 tq
	Bit_length                 = 40 tq = 1 �s
	Bit_rate                   = 1 MBit/s
  */
  hfdcan1.Instance = FDCAN1;
  hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan1.Init.AutoRetransmission = ENABLE;
  hfdcan1.Init.TransmitPause = DISABLE;
  hfdcan1.Init.ProtocolException = ENABLE;
  hfdcan1.Init.NominalPrescaler = 0x1; /* tq = NominalPrescaler x (1/fdcan_ker_ck) */
  hfdcan1.Init.NominalSyncJumpWidth = 0x8;
  hfdcan1.Init.NominalTimeSeg1 = 0x1F; /* NominalTimeSeg1 = Propagation_segment + Phase_segment_1 */
  hfdcan1.Init.NominalTimeSeg2 = 0x8;
  hfdcan1.Init.MessageRAMOffset = 0;
  hfdcan1.Init.StdFiltersNbr = 1;
  hfdcan1.Init.ExtFiltersNbr = 0;
  hfdcan1.Init.RxFifo0ElmtsNbr = 8;
  hfdcan1.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_64;
  hfdcan1.Init.RxFifo1ElmtsNbr = 0;
  hfdcan1.Init.RxBuffersNbr = 0;
  hfdcan1.Init.TxEventsNbr = 0;
  hfdcan1.Init.TxBuffersNbr = 0;
  hfdcan1.Init.TxFifoQueueElmtsNbr = 8;
  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  hfdcan1.Init.TxElmtSize = FDCAN_DATA_BYTES_64;
  if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
  {
	/* Initialization Error */
	Error_Handler();
  }

  ccm.canCh[0].CanHandle = &hfdcan1;

  /* Configure Rx filter */
  ccm.canCh[0].sFilterConfig.IdType 		 = FDCAN_STANDARD_ID;
  ccm.canCh[0].sFilterConfig.FilterIndex  	 = 0;
  ccm.canCh[0].sFilterConfig.FilterType 	 = FDCAN_FILTER_MASK;
  ccm.canCh[0].sFilterConfig.FilterConfig 	 = FDCAN_FILTER_TO_RXFIFO0;
  ccm.canCh[0].sFilterConfig.FilterID1 	 	 = 0x7FF;
  ccm.canCh[0].sFilterConfig.FilterID2 	 	 = 0x7FF;
  if (HAL_FDCAN_ConfigFilter(&hfdcan1, &ccm.canCh[0].sFilterConfig) != HAL_OK)
  {
	/* Filter configuration Error */
	Error_Handler();
  }

  /* Start the FDCAN module */
  if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
  {
	/* Start Error */
	Error_Handler();
  }

  if (HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK)
  {
	/* Notification Error */
	Error_Handler();
  }
  
}

/*!
    \brief Can communication init function
    
*/

void mwr_cancomm_init(void)
{
    memset(&ccm,0,sizeof(ccm));


    mwr_cancomm_config();
    srv_sermon_register(mwr_cancomm_var_ptable,DIM(mwr_cancomm_var_ptable));

}




void mwr_cancomm_once(void)
{
    xTaskCreate( mwr_cancomm_task_can1,   "Can1",  3*configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY   + 1, NULL );
}


void mwr_cancomm_test()
{

    mwr_cancomm_channel_t * ch;

    
    ch    = &ccm.canCh[0];

    /* Prepare Tx Header */
    ch->TxHeader.Identifier 		 = 0x321;
    ch->TxHeader.IdType 			 = FDCAN_STANDARD_ID;
    ch->TxHeader.TxFrameType 		 = FDCAN_DATA_FRAME;

    ch->TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    ch->TxHeader.BitRateSwitch 		 = FDCAN_BRS_OFF;
    ch->TxHeader.FDFormat 			 = FDCAN_CLASSIC_CAN;
    ch->TxHeader.TxEventFifoControl  = FDCAN_NO_TX_EVENTS;
    ch->TxHeader.MessageMarker 		 = 0;



    ch->TxData[0]     = 0xDE;
    ch->TxData[1]     = 0xAD;
    ch->TxData[2]     = 0;
    ch->TxData[3]     = ch->ovrerr_cntr;
    ch->TxData[4]     = ch->rcv_cntr;
    ch->TxData[5]     = ch->err_cntr;
    ch->TxData[6]     = 0;

    ch->TxHeader.DataLength 		 = FDCAN_DLC_BYTES_8;
          
    HAL_FDCAN_AddMessageToTxFifoQ(ch->CanHandle, &ch->TxHeader, ch->TxData);


   
}

void mwr_cancomm_send(can_frame_t * raw_frame,uint32_t wait4ready)
{
    mwr_cancomm_channel_t * ch;

    ch    = &ccm.canCh[0];

    ch->TxHeader.Identifier 		 = raw_frame->can_id;
    ch->TxHeader.TxFrameType 		 = FDCAN_DATA_FRAME;
   	ch->TxHeader.IdType  			 = FDCAN_EXTENDED_ID;
    ch->TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    ch->TxHeader.BitRateSwitch 		 = FDCAN_BRS_OFF;
    ch->TxHeader.FDFormat 			 = FDCAN_FD_CAN;
    ch->TxHeader.TxEventFifoControl  = FDCAN_NO_TX_EVENTS;
    ch->TxHeader.MessageMarker 		 = 0;
    ch->TxHeader.DataLength 		 = raw_frame->can_dlc;


    HAL_FDCAN_AddMessageToTxFifoQ(ch->CanHandle, &ch->TxHeader, ch->TxData);

    
}

/*!
    \brief 
    
*/
static void mwr_cancomm_task_can1(void * params)
{
	if (HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK)
	{
	    /* Notification Error */
	    Error_Handler();
	}

}



int mwr_cancomm_receive(can_frame_t * raw_frame)
{
   mwr_cancomm_channel_t * ch;


   ch    = &ccm.canCh[0];


   if(ch->cring_head == ch->cring_tail)
   {
      // No frames available
      return -1;
   }
   else
   {
      memcpy(raw_frame,&ch->cring[ch->cring_tail],sizeof(*raw_frame));

      ch->cring_tail = (ch->cring_tail + 1)%MWR_CANCOMM_RING_SIZE;

      return 0;
   }
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{

  mwr_cancomm_channel_t * ch;
  uint32_t				  ch_idx;
  uint32_t 				  new_head;


  for(ch_idx =0 ; ch_idx < MWR_CANCOMM_CH_CNT;ch_idx++)
  {
	  ch    = &ccm.canCh[ch_idx];

	  if(ch->CanHandle != hfdcan)
	  {
		  continue;
	  }


	  if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET)
	  {
		/* Retreive Rx messages from RX FIFO0 */
		if (HAL_FDCAN_GetRxMessage(ch->CanHandle, FDCAN_RX_FIFO0, &ch->RxHeader, ch->RxData) != HAL_OK)
		{
			/* Reception Error */
			Error_Handler();
		}
		else
		{
			 new_head = (ch->cring_head + 1)%MWR_CANCOMM_RING_SIZE;

			 if(new_head != ch->cring_tail)
			 {
				  can_frame_t * fr;

				  fr = &ch->cring[ch->cring_head];

				  fr->can_dlc = ch->RxHeader.DataLength;
				  fr->can_id  = ch->RxHeader.Identifier;


				  fw_assert(ch->RxHeader.DataLength <= sizeof(fr->data));
				  memcpy(fr->data,ch->RxData,fr->can_dlc);

				  ch->cring_head = new_head;
			 }
			 else
			 {
				ch->ovrerr_cntr++;
			 }
		}


		if (HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK)
		{
		  /* Notification Error */
		  Error_Handler();
		}
	  }
  }
}




