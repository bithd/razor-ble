#ifndef _I2C_DRV_H_
#define _I2C_DRV_H_

#include "IO_CTL.h"
//---------------------------------------
//��ʱ����
//ע��:�˴���оƬ��Ƶ�ȹأ�����ʱ��Ҫʹ��
//ʾ�������ο�оƬ�ֲ�
//----------------------------------------
#define SI2CDRV_DELAY_4US 		4
#define SI2CDRV_DELAY_8US 		8
#define SI2CDRV_DELAY_16US 		16

#define SI2CDRV_DELAY_40US 		40
#define SI2CDRV_DELAY_100US 	100
#define SI2CDRV_DELAY_200US 	200

#define SI2CDRV_DELAY_1MS 		1000


//--------------------------------------
//I2C�����붨��
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
 ����:	vSI2CDRV_Init
 ���룺
 		��
 �����	
 		��
 ����:
 		��
 ����:
		�ú�������ģ��I2C�ӿڳ�ʼ��
******************************************************************************/
extern void vSI2CDRV_Init(void);

uint8_t vSI2CDRV_QuickCmd(uint8_t ucMode);
/***********************R******************************************************
 ����:  I2C_ReadDataByPcb
 ����:	
 		ucInPcb:���豸��Pcbֵ
 ���:	pOutBuf:���豸��Ӧ������ָ�루�������е�һ��ΪPCB��
		ucResLen:���豸��Ӧ�����ݳ���,��Ϊ1ʱ��ʾֻ����pcb
 		
 ����:	false/true
 		
 ����:  
 		�ú�������ģ��I2C�ӿ�ͨ��PCB��ȡ����
******************************************************************************/
extern uint8_t ucSI2CDRV_ReadDataByPcb(uint8_t ucInPcb,uint8_t * pucRecvPcb ,uint8_t * pucOut, uint16_t * pusOutLen);

/******************************************************************************
 ����:  I2C_ReadDataByPcb
 ����:	
 		ucInPcb:���豸��Pcbֵ
 		pucSrc:���豸��Ӧ������ָ��
 		ucLen:���豸�Ĵ������ݳ���
 ���:
 		��
 ����:	
 		������μ�SI2CDRV_ERR_CODE
 ����:  
 		�ú�������ģ��I2C�ӿ�ͨ��PCB��ȡ����
******************************************************************************/
extern uint8_t ucSI2CDRV_WriteDataByPcb(uint8_t ucInPcb ,uint8_t * pucSrc, uint8_t ucLen);

#endif
