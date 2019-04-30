#ifndef A7_I2C_H_
#define A7_I2C_H_
#include "stdint.h"
#include "nrf_delay.h"
//#include "IO_CTL.h"

uint8_t I2C_PowerDown(uint8_t ucMode);
uint8_t I2C_ReadDataByPcb( uint8_t ucInPcb,uint8_t *revice_pcb ,uint8_t *pOutBuf, uint16_t *pusOutLen);

uint8_t I2C_BlockWrite( uint8_t ucInPcb, uint8_t *pInBuf, uint16_t ucInLen);

uint8_t I2C_nBYTE_WRITE(uint8_t saddr, uint8_t reg_addr, uint8_t *buffer, uint16_t len);
uint8_t I2C_nBYTE_WRITE2(uint8_t saddr, uint8_t *buffer, uint16_t len);


uint8_t I2C_PCB_BYTE_WRITE(uint8_t ucPcb);

uint8_t I2C_nBYTE_READ(uint8_t saddr,  uint8_t *reg_pcb, uint8_t *buffer, uint16_t *len);
uint8_t I2C_nBYTE_READ2(uint8_t *buffer, uint16_t *len);
uint8_t I2C_WritePcb_GetLenPcb(uint8_t ucInPcb, uint8_t * ucOutPcb);
void twi_init(void);


#endif

