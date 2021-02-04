#ifndef NOV_SRV_QSPI_H
#define NOV_SRV_QSPI_H

/*! \file   nov_srv_qspi.h

    \brief  Header for low level QSPI flash operations
    
    
*/

#define QSPI_OFFSET	 0x90000000
#define QSPI_START  ((volatile uint8_t *)(QSPI_OFFSET))


/* GD25Q32C GigaDevice memory - 32M */
/* Size of the flash */
#define QSPI_FLASH_SIZE                      22  	/* 22 address lines  */
#define QSPI_PAGE_SIZE                       256
#define QSPI_SECTOR_SIZE					 4096

/* Reset Operations */
#define RESET_ENABLE_CMD                     0x66
#define RESET_MEMORY_CMD                     0x99

/* Identification Operations */
#define READ_ID_CMD                          0x9F
#define READ_SERIAL_FLASH_DISCO_PARAM_CMD    0x5A

/* Read Operations */
#define READ_CMD                             0x03
#define FAST_READ_CMD                        0x0B
#define DUAL_OUT_FAST_READ_CMD               0x3B
#define DUAL_INOUT_FAST_READ_CMD             0xBB
#define QUAD_OUT_FAST_READ_CMD               0x6B
#define QUAD_INOUT_FAST_READ_CMD             0xEB

/* Write Operations */
#define WRITE_ENABLE_CMD                     0x06
#define WRITE_DISABLE_CMD                    0x04

/* Register Operations */
#define READ_STATUS_REG1_CMD                 0x05
#define READ_STATUS_REG2_CMD                 0x35
#define READ_STATUS_REG3_CMD                 0x15
#define WRITE_STATUS_REG1_CMD                0x01
#define WRITE_STATUS_REG2_CMD                0x31
#define WRITE_STATUS_REG3_CMD                0x11

/* Identification */
#define READ_DEVICE_ID						 0x90
#define READ_DEVICE_ID_QUAD					 0x94

/* Program Operations */
#define PAGE_PROG_CMD                        0x02
#define QUAD_IN_FAST_PROG_CMD                0x32

/* Erase Operations */
#define SECTOR_4K_ERASE_CMD       	         0x20
#define BLOCK_32K_ERASE_CMD                  0x52
#define BLOCK_64K_ERASE_CMD                  0xD8
#define BULK_ERASE_CMD                       0xC7
#define PROG_ERASE_RESUME_CMD                0x7A
#define PROG_ERASE_SUSPEND_CMD               0x75

/* One-Time Programmable Operations */
#define READ_OTP_ARRAY_CMD                   0x4B
#define PROG_OTP_ARRAY_CMD                   0x42


/* Default dummy clocks cycles */
#define DUMMY_CLOCK_CYCLES_READ              8
#define DUMMY_CLOCK_CYCLES_READ_QUAD         10



int32_t  srv_nov_qspi_init(void);
int32_t  srv_nov_qspi_write(const uint8_t * data, size_t size, uint32_t flash_offset);
int32_t  srv_nov_qspi_read(uint8_t * data, size_t size, uint32_t flash_offset);






#endif //NOV_SRV_H


