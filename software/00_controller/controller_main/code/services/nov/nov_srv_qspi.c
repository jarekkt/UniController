/*! \file   srv_nov_qspi.c

    \brief  Low level QSPI support
    
    
*/ 

#include "system.h"
#include "nov_srv_qspi.h"
#include "quadspi.h"

extern QSPI_HandleTypeDef hqspi;

volatile uint8_t CmdCplt, RxCplt, TxCplt, StatusMatch, TimeOut;


static int QSPI_WriteEnable(QSPI_HandleTypeDef *hqspi);
static int QSPI_AutoPollingMemReady(QSPI_HandleTypeDef *hqspi);
static int QSPI_MemMap(QSPI_HandleTypeDef *hqspi,int32_t enable);




static int32_t srv_nov_qspi_setup_quad_gd25q32csig(void)
{

	  QSPI_CommandTypeDef s_command;
	  uint8_t  rdrs_15_8;
	  uint16_t did;


	  /* Read device ID  */
	  memset(&did,0,sizeof(did));
	  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	  s_command.Instruction       = READ_DEVICE_ID_QUAD;

	  s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
	  s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
	  s_command.Address		      = 0;

	  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	  s_command.DataMode          = QSPI_DATA_4_LINES;
	  s_command.DummyCycles       = 4;
	  s_command.NbData            = 2;


	  RxCplt= 0;

	  /* Configure the command */
	  if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	  {
	     return -1;
	  }

	  /* Reception of the data */
	  if (HAL_QSPI_Receive_IT(&hqspi, (uint8_t *)(&did)) != HAL_OK)
	  {
		  return -1;
	  }


	  while(RxCplt== 0)
	  {
	  }


	  if(did != 0x15C8)
	  {
		  /*
		   *  Make sure we have valid GD25Q32C chip
		   */
		  return -1;
	  }


	  /* Initialize the read status reg 2 */
	  s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	  s_command.Instruction       = READ_STATUS_REG2_CMD;

	  s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
	  s_command.AddressMode       = QSPI_ADDRESS_NONE;

	  s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	  s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	  s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	  s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	  s_command.DataMode          = QSPI_DATA_1_LINE;
	  s_command.DummyCycles       = 0;
	  s_command.NbData            = 1;


	  /* Configure the command */
	  if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	  {
	     return -1;
	  }

	  RxCplt= 0;

	  /* Reception of the data */
	  if (HAL_QSPI_Receive_IT(&hqspi, (uint8_t *)(&rdrs_15_8)) != HAL_OK)
	  {
		  return -1;
	  }

	  while(RxCplt== 0)
      {

	  }
	  RxCplt= 0;

	  /* Enable quad mode if needed - requires write to non-volatile register */
	  if( (rdrs_15_8 & 0x02 /* QE - quad enable bit */) == 0)
	  {
		  rdrs_15_8 |= 0x02;

		  s_command.NbData            = 1;

		  /* Enable write operations */
		  QSPI_WriteEnable(&hqspi);

		  /* Update volatile configuration register (with new dummy cycles) */
		  s_command.Instruction = WRITE_STATUS_REG2_CMD;

		  /* Configure the write volatile configuration register command */
		  if (HAL_QSPI_Command(&hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		  {
			  return -1;
		  }

		  TxCplt= 0;

		  /* Transmission of the data */
		  if (HAL_QSPI_Transmit_IT(&hqspi, (uint8_t *)(&rdrs_15_8)) != HAL_OK)
		  {
			return -1;
		  }

		  while(TxCplt== 0)
		  {
		  }
	  }





	  return 0;

}


int32_t  srv_nov_qspi_init(void)
{
	int32_t result;

	result = srv_nov_qspi_setup_quad_gd25q32csig();
	if(result != 0)
	{
		return result;
	}

	QSPI_MemMap(&hqspi,1);

	return 0;
}


int32_t  srv_nov_qspi_read(uint8_t * data, size_t size, uint32_t flash_offset)
{
	QSPI_CommandTypeDef 	 sCommand;

	sCommand.Instruction 		  = QUAD_OUT_FAST_READ_CMD;
	sCommand.InstructionMode   	  = QSPI_INSTRUCTION_1_LINE;

	sCommand.AddressSize       	  = QSPI_ADDRESS_24_BITS;
	sCommand.AddressMode 		  = QSPI_ADDRESS_1_LINE;
	sCommand.Address			  = flash_offset;

	sCommand.AlternateByteMode 	  = QSPI_ALTERNATE_BYTES_NONE;
	sCommand.DdrMode           	  = QSPI_DDR_MODE_DISABLE;
	sCommand.DdrHoldHalfCycle  	  = QSPI_DDR_HHC_ANALOG_DELAY;
	sCommand.SIOOMode          	  = QSPI_SIOO_INST_EVERY_CMD;

	sCommand.DataMode    		  = QSPI_DATA_4_LINES;
	sCommand.NbData 		   	  = size ;
	sCommand.DummyCycles 		  = 8;



	StatusMatch = 0;
	RxCplt		= 0;

	/* Configure the command */
	if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
	     return -1;
	}

	/* Reception of the data */
	if (HAL_QSPI_Receive_IT(&hqspi, (uint8_t *)(data)) != HAL_OK)
	{
		  return -1;
	}

	while(RxCplt == 0)
	{

	}

	return 0;

}

int32_t  srv_nov_qspi_write(const uint8_t * data, size_t size, uint32_t flash_offset)
{
	QSPI_CommandTypeDef 	 sCommand;
	uint32_t				 ii,jj;
	uint32_t				 total = 0;
	int32_t					 result = 0;
	uint8_t					 v_flash,v_mem;


	if( (flash_offset % QSPI_SECTOR_SIZE) !=0)
	{
		// We serve only aligned requests
		return -1;
	}


	QSPI_MemMap(&hqspi,0);


	// Sector erase first

	for(ii = 0; ii < (size + QSPI_SECTOR_SIZE -1) / QSPI_SECTOR_SIZE;ii++)
	{

		QSPI_WriteEnable(&hqspi);

		/* Block erase */
		sCommand.Instruction 		= SECTOR_4K_ERASE_CMD;
		sCommand.InstructionMode    = QSPI_INSTRUCTION_1_LINE;

		sCommand.AddressSize        = QSPI_ADDRESS_24_BITS;
		sCommand.AddressMode 		= QSPI_ADDRESS_1_LINE;
		sCommand.Address     		= flash_offset + QSPI_SECTOR_SIZE * ii;

		sCommand.AlternateByteMode  = QSPI_ALTERNATE_BYTES_NONE;
		sCommand.DdrMode            = QSPI_DDR_MODE_DISABLE;
		sCommand.DdrHoldHalfCycle   = QSPI_DDR_HHC_ANALOG_DELAY;
		sCommand.SIOOMode           = QSPI_SIOO_INST_EVERY_CMD;

		sCommand.DataMode    		= QSPI_DATA_NONE;
		sCommand.DummyCycles 		= 0;

		CmdCplt = 0;

		if (HAL_QSPI_Command_IT(&hqspi, &sCommand) != HAL_OK)
		{
			return -1;
		}

		while(CmdCplt == 0)
		{
		}

		CmdCplt = 0;
		StatusMatch = 0;

		/* Configure automatic polling mode to wait for end of erase ------- */
		QSPI_AutoPollingMemReady(&hqspi);

		while(StatusMatch  == 0)
		{

		};
		HAL_Delay(25);
	}

	// Page program

	for(ii = 0; ii < (size + QSPI_PAGE_SIZE -1) / QSPI_PAGE_SIZE;ii++)
	{

		/* Unlock */
		QSPI_WriteEnable(&hqspi);

		/* Page program */
		sCommand.Instruction = QUAD_IN_FAST_PROG_CMD;

		sCommand.Address     = flash_offset + QSPI_PAGE_SIZE * ii;

		sCommand.DataMode    = QSPI_DATA_4_LINES;
		sCommand.DummyCycles = 0;

		total += QSPI_PAGE_SIZE;

		if(total <= size)
		{
			sCommand.NbData = QSPI_PAGE_SIZE ;
		}
		else
		{
			sCommand.NbData = size % QSPI_PAGE_SIZE; // last chunk - not full page
		}


		StatusMatch = 0;
		TxCplt = 0;

		if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		{
			  return -1;
		}

		if (HAL_QSPI_Transmit_IT(&hqspi, &(((uint8_t *)data)[QSPI_PAGE_SIZE*ii])) != HAL_OK)
		{
			  return -1;
		}

		while(TxCplt == 0)
		{

		}

		TxCplt = 0;
		StatusMatch = 0;

		QSPI_AutoPollingMemReady(&hqspi);

		while(StatusMatch == 0)
		{
		}

	}


	QSPI_MemMap(&hqspi,1);

	// Write  validate

	for (jj = 0; jj < size; jj++)
	{
		v_flash = (((uint8_t *)data)[jj]) ;
		v_mem   = QSPI_START[flash_offset+jj];
		if( v_flash != v_mem)
		{
			result = -1;
			break;
		}
	}

	return result;

}




/**
  * @brief  Command completed callbacks.
  * @param  hqspi: QSPI handle
  * @retval None
  */
void HAL_QSPI_CmdCpltCallback(QSPI_HandleTypeDef *hqspi)
{
  CmdCplt++;
}

/**
  * @brief  Rx Transfer completed callbacks.
  * @param  hqspi: QSPI handle
  * @retval None
  */
void HAL_QSPI_RxCpltCallback(QSPI_HandleTypeDef *hqspi)
{
  RxCplt++;
}

/**
  * @brief  Tx Transfer completed callbacks.
  * @param  hqspi: QSPI handle
  * @retval None
  */
void HAL_QSPI_TxCpltCallback(QSPI_HandleTypeDef *hqspi)
{
  TxCplt++;
}

/**
  * @brief  Status Match callbacks
  * @param  hqspi: QSPI handle
  * @retval None
  */
void HAL_QSPI_StatusMatchCallback(QSPI_HandleTypeDef *hqspi)
{
  StatusMatch++;
}





/*
 *  Does not work reliably - single bit errors even if direct flash read works
 */


/**
  * @brief  This function send a Write Enable and wait it is effective.
  * @param  hqspi: QSPI handle
  * @retval None
  */

static int QSPI_MemMap(QSPI_HandleTypeDef *hqspi,int32_t enable)
{
  QSPI_CommandTypeDef       sCommand;
  QSPI_MemoryMappedTypeDef  sMemMappedCfg;
  volatile uint32_t 		dummy;


  if(enable != 0)
  {
	  sCommand.Instruction 			  = QUAD_OUT_FAST_READ_CMD;
	  sCommand.InstructionMode   	  = QSPI_INSTRUCTION_1_LINE;

	  sCommand.AddressSize       	  = QSPI_ADDRESS_24_BITS;
	  sCommand.AddressMode 		 	  = QSPI_ADDRESS_1_LINE;
	  sCommand.Address  			  = 0;

	  sCommand.AlternateByteMode 	  = QSPI_ALTERNATE_BYTES_NONE;
	  sCommand.DdrMode           	  = QSPI_DDR_MODE_DISABLE;
	  sCommand.DdrHoldHalfCycle  	  = QSPI_DDR_HHC_ANALOG_DELAY;
	  sCommand.SIOOMode          	  = QSPI_SIOO_INST_EVERY_CMD;

	  sCommand.DataMode    		 	  = QSPI_DATA_4_LINES;
	  sCommand.DummyCycles 		 	  = 8;


	  sMemMappedCfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;

	  if (HAL_QSPI_MemoryMapped(hqspi, &sCommand, &sMemMappedCfg) != HAL_OK)
	  {
		return -1;
	  }

	  // Force dummy read to actually enter the mode
	  dummy = *((uint32_t*)QSPI_START);
	  dummy++;
  }
  else
  {
	  HAL_QSPI_Abort(hqspi);
  }

  return 0;
}



/**
  * @brief  This function send a Write Enable and wait it is effective.
  * @param  hqspi: QSPI handle
  * @retval None
  */

static int QSPI_WriteEnable(QSPI_HandleTypeDef *hqspi)
{
  QSPI_CommandTypeDef     sCommand;
  QSPI_AutoPollingTypeDef sConfig;

  /* Enable write operations ------------------------------------------ */
  sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction       = WRITE_ENABLE_CMD;
  sCommand.AddressMode       = QSPI_ADDRESS_NONE;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode          = QSPI_DATA_NONE;
  sCommand.DummyCycles       = 0;
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  if (HAL_QSPI_Command(hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return -1;
  }

  /* Configure automatic polling mode to wait for write enabling ---- */
  sConfig.Match           = 0x02;
  sConfig.Mask            = 0x02;
  sConfig.MatchMode       = QSPI_MATCH_MODE_AND;
  sConfig.StatusBytesSize = 1;
  sConfig.Interval        = 0x10;
  sConfig.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

  sCommand.Instruction    = READ_STATUS_REG1_CMD;
  sCommand.DataMode       = QSPI_DATA_1_LINE;

  if (HAL_QSPI_AutoPolling(hqspi, &sCommand, &sConfig, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
	  return -1;
  }

  return 0;
}

/**
  * @brief  This function read the SR of the memory and wait the EOP.
  * @param  hqspi: QSPI handle
  * @retval None
  */
static int  QSPI_AutoPollingMemReady(QSPI_HandleTypeDef *hqspi)
{
  QSPI_CommandTypeDef     sCommand;
  QSPI_AutoPollingTypeDef sConfig;

  /* Configure automatic polling mode to wait for memory ready ------ */
  sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction       = READ_STATUS_REG1_CMD;
  sCommand.AddressMode       = QSPI_ADDRESS_NONE;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode          = QSPI_DATA_1_LINE;
  sCommand.DummyCycles       = 0;
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  sConfig.Match              = 0x00;
  sConfig.Mask               = 0x01;
  sConfig.MatchMode          = QSPI_MATCH_MODE_AND;
  sConfig.StatusBytesSize    = 1;
  sConfig.Interval           = 0x10;
  sConfig.AutomaticStop      = QSPI_AUTOMATIC_STOP_ENABLE;

  if (HAL_QSPI_AutoPolling_IT(hqspi, &sCommand, &sConfig) != HAL_OK)
  {
    return -1;
  }

  return 0;
}


