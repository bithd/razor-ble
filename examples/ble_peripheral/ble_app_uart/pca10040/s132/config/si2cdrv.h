#ifndef _I2C_DRV_H_
#define _I2C_DRV_H_

#include "IO_CTL.h"
//---------------------------------------
//延时定义
//注意:此处与芯片主频先关，调整时需要使用
//示波器并参考芯片手册
//----------------------------------------
#define SI2CDRV_DELAY_4US 		4
#define SI2CDRV_DELAY_8US 		8
#define SI2CDRV_DELAY_16US 		16

#define SI2CDRV_DELAY_40US 		40
#define SI2CDRV_DELAY_100US 	100
#define SI2CDRV_DELAY_200US 	200

#define SI2CDRV_DELAY_1MS 		1000


//--------------------------------------
//I2C错误码定义
//--------------------------------------
enum SI2CDRV_ERR_CODE
{
    I2C_NO_ERROR     = 0,       /*!< I2C no error */
    I2C_CONFLICT     = 1,       /*!< I2C conflict */
    I2C_NO_ACK       = 2,       /*!< I2C no ack */
    I2C_TIMEOUT      = 3,        /*!< I2C timeout */
		I2C_WRONGLEN     = 4       /*!< I2C timeout */
};


/*****************************************************************************
 函数:	vSI2CDRV_Init
 输入：
 		无
 输出：	
 		无
 返回:
 		无
 功能:
		该函数用于模拟I2C接口初始化
******************************************************************************/
extern void vSI2CDRV_Init(void);

uint8_t vSI2CDRV_QuickCmd(uint8_t ucMode);
/***********************R******************************************************
 函数:  I2C_ReadDataByPcb
 输入:	
 		ucInPcb:主设备的Pcb值
 输出:	pOutBuf:从设备相应的数据指针（数据域中第一个为PCB）
		ucResLen:从设备相应的数据长度,当为1时表示只返回pcb
 		
 返回:	false/true
 		
 功能:  
 		该函数用于模拟I2C接口通过PCB读取数据
******************************************************************************/
extern uint8_t ucSI2CDRV_ReadDataByPcb(uint8_t ucInPcb,uint8_t * pucRecvPcb ,uint8_t * pucOut, uint16_t * pusOutLen);

/******************************************************************************
 函数:  I2C_ReadDataByPcb
 输入:	
 		ucInPcb:主设备的Pcb值
 		pucSrc:主设备相应的数据指针
 		ucLen:主设备的传输数据长度
 输出:
 		无
 返回:	
 		错误码参见SI2CDRV_ERR_CODE
 功能:  
 		该函数用于模拟I2C接口通过PCB读取数据
******************************************************************************/
extern uint8_t ucSI2CDRV_WriteDataByPcb(uint8_t ucInPcb ,uint8_t * pucSrc, uint8_t ucLen);

#endif
