//----------------------------------------------------------------------
//      File: sci2c.C
//----------------------------------------------------------------------
//      Update History: (mm/dd/yyyy)
//      11/28/2015 - V1.00 - IQ.CHEN :  First official release
//----------------------------------------------------------------------
//      Description:
//
//      ��ģ��ΪI2CЭ��ӿڷ�װ
//----------------------------------------------------------------------
#include <string.h>
#include "app_error.h"
#include "sys.h"
//#include "nrf_drv_twi.h"
#include "nrf_delay.h"
#include "app_util_platform.h"

uint8_t g_ucI2CRecCompleteFlag = 0 ;
uint16_t g_usI2COffset = 0  ;
//static uint8_t  s_ucSlaveMaxNum = SCI2C_CDBSM_DEFAULT;  //��=>���豸���������
//static uint8_t  s_ucMasterMaxNum = SCI2C_CDBMS_DEFAULT;  //��=>���豸���������
static uint8_t  s_ucMasterSeqCount = 0;	//���豸�˵�seqֵ
static uint8_t  s_ucFWICounter = 0x05 ;   //������ȴ�ʱ������


 /*****************************************************************************
 ����:  bSCI2C_SelectApplet
 ����:	��
 ���:	��
 		
 ����:	false/true
 		
 ����:  �����ϵ�ѡ��U2F��Ӧ�� A000000527100201
				
******************************************************************************/
static void bSCI2C_SelectApplet(void)
{
		uint16_t usLen;
		uint8_t counter,ucbuf[128];
	//00A4040008 A000000527100201
		ucbuf[0]=0x00; ucbuf[1]=0xA4;
		ucbuf[2]=0x04; ucbuf[3]=0x00;
		ucbuf[4]=0x08; ucbuf[5]=0xA0;
		ucbuf[6]=0x00; ucbuf[7]=0x00;
		ucbuf[8]=0x05; ucbuf[9]=0x27;
		ucbuf[10]=0x10; ucbuf[11]=0x02;
		ucbuf[12]=0x01; 
		usLen = 13 ;
		bSCI2C_DataExchangeMToS(ucbuf,usLen);
		nrf_delay_ms(30);
		counter = 0 ;
		g_ucI2CRecCompleteFlag = 0x00;
		while((g_ucI2CRecCompleteFlag == 0)&&(counter < 50))
		{
			nrf_delay_ms(10);
			counter++ ;
			if (false==bSCI2C_DataExchangeSToM(ucbuf,&usLen))
			{
				break;
			}
		}
		g_ucI2CRecCompleteFlag = 0x00;
}

 /*****************************************************************************
 ����:  vSCI2C_Init
 ����:	ucSleep:�Ƿ���sleep״̬
 ���:	��
 		
 ����:	��
 		
 ����:  ��ʼ��
				
******************************************************************************/
void vSCI2C_Init(uint8_t ucSleep)
{
	//twi_init();
	vSI2CDRV_Init();
	if ( ucSleep )
	{
		return ;
	}

#if (SUPPORT_A7_POWERDOWN > 0)
		vSCI2C_PowerOn();
		nrf_delay_ms(tRSTG);
		bSCI2C_PowerDown();
#endif
    TURN_OFF_I2C() ;              
}
 /*****************************************************************************
 ����:  vSCI2C_PowerOn
 ����:	��
 ���:	��
 		
 ����:	��
 		
 ����:  ��A7�ϵ粢����
				
******************************************************************************/
void vSCI2C_PowerOn(void)
{
		A7_POWER_ON();
		nrf_delay_ms(tRSTG);
		bSCI2C_SoftReset();
		nrf_delay_ms(tRSTG);
		bSCI2C_ReadATR();
		nrf_delay_ms(tRSTG);
		bSCI2C_ParameterExchange();
		nrf_delay_ms(tRSTG);
		bSCI2C_SelectApplet();
		//TURN_OFF_I2C() ;
}
 /*****************************************************************************
 ����:  bSCI2C_CalcLRC
 ����:	pBuf:�������ݵ��׵�ַ
				usLen:�������ݵĳ���
 ���:	��
 		
 ����:	XORֵ
 		
 ����:  �����������ݵ�XORֵ
				
******************************************************************************/
uint8_t bSCI2C_CalcLRC( uint8_t *pBuf, uint16_t usLen)
{
	uint8_t lrc = 0;
	uint16_t i;

	for (i=0; i<usLen; i++) 
	{
		lrc ^= pBuf[i];
	}
	return lrc;
}

 /*****************************************************************************
 ����:  bSCI2C_PowerDown
 ����:	ucMode:0 is powerdown;1 is powerdown_for_coldreset
 ���:	��
 		
 ����:	false/true
 		
 ����:  �Դ��豸ִ��PowerDown�����л���PWRSAV״̬��
				
******************************************************************************/
bool bSCI2C_PowerDown(void)
{	
	uint8_t i ;

	for(i=0;i<SCI2C_RETRY_TIMERS;i++)
	{
		if (I2C_NO_ERROR==vSI2CDRV_QuickCmd(0x00))
		{
			return true;
		}
		nrf_delay_ms(tRSTG);
	}	
	return false;
}

 /*****************************************************************************
 ����:  bSCI2C_PowerDownForColdReset
 ����:	ucMode:0 is powerdown;1 is powerdown_for_coldreset
 ���:	��
 		
 ����:	false/true
 		
 ����:  �Դ��豸ִ��PowerDown�����л���PWRSAV״̬��
				
******************************************************************************/
bool bSCI2C_PowerDownForColdReset(void)
{	
	return vSI2CDRV_QuickCmd(0x01);
}

 /*****************************************************************************
 ����:  bSCI2C_WakeUp
 ����:	��
 ���:	��
 		
 ����:	false/true
 		
 ����:  �Ѵ��豸��PWRSAV�л�״̬ΪACTIVE/Ready
				
******************************************************************************/
uint8_t bSCI2C_WakeUp(void)
{
//	uint8_t buf[2];
	
	ucSI2CDRV_WriteDataByPcb(SCI2C_PCB_WAKEUP,NULL,0);
	nrf_delay_ms(tRSTG);	
	return ucSI2CDRV_WriteDataByPcb(SCI2C_PCB_WAKEUP,NULL,0);
}

 /*****************************************************************************
 ����:  usSCI2C_GetWFTByWFI
 ����:	��
 ���:	��
 		
 ����:	���ٺ���
 		
 ����:  ����FWI����ȴ�ʱ��FWT,tFW = 10ms x 2FWI
				
******************************************************************************/
uint16_t usSCI2C_GetFWTByFWI(void)
{
		uint8_t i;
		uint16_t ret=1;
		
		for(i=0;i<s_ucFWICounter;i++)
		{
			ret = ret*2 ;
		}
			
		return (10*ret);
		
}
 /*****************************************************************************
 ����:  bSCI2C_SoftReset
 ����:	��
 ���:	��
 		
 ����:	false/true
 		
 ����:  �Դ��豸������λ�л�״̬ΪReady
				
******************************************************************************/
bool bSCI2C_SoftReset(void)
{
	uint8_t i,BufOut[8] ;
	uint8_t    outpcb;//,BufIn[2];
	uint16_t   RespLen = 0;
//	bool ret ;

	for(i=0;i<SCI2C_RETRY_TIMERS;i++)
	{
//		outpcb = SCI2C_PCBSM_EXCEPTION;
//		ret = I2C_WritePcb_GetLenPcb(SCI2C_PCB_SOFTRESET,&outpcb) ;
//		if ((SCI2C_PCBSM_SOFTRESET == outpcb)&&(true == ret))
		if (I2C_NO_ERROR == ucSI2CDRV_ReadDataByPcb(SCI2C_PCB_SOFTRESET,&outpcb,BufOut,&RespLen))
		{
			s_ucMasterSeqCount = 0 ;
			s_ucMasterMaxNum = SCI2C_CDBMS_DEFAULT;
			nrf_delay_ms(tRSTG);
			return true;
		}
		nrf_delay_ms(tRSTG);
	}
	return false;
}

 /*****************************************************************************
 ����:  bSCI2C_ReadATR
 ����:	��
 ���:	��
 		
 ����:	false/true
 		
 ����:  ��ȡ���豸��ATR,APP�㲻��ҪATR��������ʱ�����ͳ�ȥ
				
******************************************************************************/
bool bSCI2C_ReadATR(void)
{
	uint8_t i ;
	uint8_t    outpcb,BufOut[64];
	uint16_t   RespLen = 0;
	outpcb = 0 ;
	for(i=0;i<SCI2C_RETRY_TIMERS;i++)
	{
		//if (true == I2C_ReadDataByPcb(SCI2C_PCB_READATR,&outpcb,BufOut,&RespLen))
		if (I2C_NO_ERROR ==ucSI2CDRV_ReadDataByPcb(SCI2C_PCB_READATR,&outpcb,BufOut,&RespLen))
		{
			if((outpcb!=SCI2C_PCBSM_READATR) ||(RespLen < 4))  //atr����������3�ֽ�
			{
				return false;
			}
			s_ucMasterSeqCount = 0 ;
			s_ucFWICounter = BufOut[4] ;

			return true;
		}
		nrf_delay_ms(tMD);
	}
	return false;
}

 /*****************************************************************************
 ����:  bSCI2C_ParameterExchange
 ����:	��
 ���:	��
 		
 ����:	false/true
 		
 ����:  �����豸֮��Э��ͨѶ���ݰ���С��������Ҫ����ATR����
				
******************************************************************************/
bool bSCI2C_ParameterExchange(void)
{
	uint8_t i ,BufOut[64];
	uint8_t    outpcb ;//= 0xff;
	uint16_t   RespLen = 0;

	for(i=0;i<SCI2C_RETRY_TIMERS;i++)
	{
		//if (true == I2C_WritePcb_GetLenPcb(SCI2C_PCB_PE_MAX,&outpcb))
		if (I2C_NO_ERROR == ucSI2CDRV_ReadDataByPcb(SCI2C_PCB_PE_MAX,&outpcb,BufOut,&RespLen))
		{
			//if (outpcb!=0xff)
			{
				s_ucMasterMaxNum = SCI2C_CDBMS_I3 ;  
				if (((outpcb&0x0C)>>2) == 0)
					s_ucSlaveMaxNum = SCI2C_CDBSM_I0;
				else if (((outpcb&0x0C)>>2) == 1)
					 s_ucSlaveMaxNum = SCI2C_CDBSM_I1;
				else if (((outpcb&0x0C)>>2) == 2)
					s_ucSlaveMaxNum = SCI2C_CDBSM_I2;
				else if (((outpcb&0x0C)>>2) == 3)
					 s_ucSlaveMaxNum = SCI2C_CDBSM_I3;
				return true;
			}
		}
		nrf_delay_ms(tRSTG);
	}
	return false;
}
 /*****************************************************************************
 ����:  bSCI2C_GetStatus
 ����:	��
 ���:	pStatus:���һ��״̬�ֽ�,��4bit��Ч
 		
 ����:	false/true
 		
 ����:  �������豸��ȡ���豸�ĵ�ǰ״̬
				
******************************************************************************/
bool bSCI2C_GetStatus(uint8_t *pStatus)
{
	uint8_t    i,Result,outpcb,BufOut[2];
	uint16_t   RespLen = 0;
	outpcb = 0 ;
	for (i=0;i<SCI2C_RETRY_TIMERS;i++)
	{
		nrf_delay_ms(tMD);
		Result = ucSI2CDRV_ReadDataByPcb(SCI2C_PCB_STATUS,&outpcb,BufOut,&RespLen);
		if (I2C_NO_ACK == Result)
		{
			nrf_delay_ms(tRSTG);
			*pStatus = PROCESSING_BUSY ;  // ����æ���ϲ������ȡ״̬
			return true;
		}
		else if (I2C_NO_ERROR == Result)
		{
			*pStatus = ((outpcb&SCI2C_PCB_ERROR_MASK)>>STATUS_SHIFT) ;  // high 4bit 
			return true;		
		}
//		else
//		{
//			//I2C״̬�������½��г�ʼ��					
//			vSCI2C_Init(0x01);
//			nrf_delay_ms(20);		
//		}
	}
	return false;
}

 /*****************************************************************************
 ����:  bSCI2C_DataExchangeMToS
 ����:	pInBuf:Ҫ���͸����豸�������׵�ַ
				usLen:Ҫ���͸����豸�����ݳ���
 ���:	
 		
 ����:	false/true
 		
 ����:  �������豸�����ݷ��͸����豸,
				
******************************************************************************/
bool bSCI2C_DataExchangeMToS(uint8_t *pInBuf, uint16_t usLen)
{
	uint8_t i,Pcb,maxpacklen,slaveStatus,pcbEDC;//,status ;
	uint16_t  offset,OvertimeCount;

	//���ȴ�I2C����
	//��һ������
	nrf_delay_ms(tRSTG);
	slaveStatus = bSCI2C_WakeUp();
#if (SUPPORT_A7_POWERDOWN > 0)
	nrf_delay_ms(tFW);
#endif
	maxpacklen = s_ucMasterMaxNum;
	pcbEDC = SCI2C_PCB_DATA_EDC_NONE;
	offset = 0 ;
	OvertimeCount = 0 ;
	
	do
	{
		nrf_delay_ms(tRSTG);

		//����֮ǰ��ȡ���豸״̬	
		if (false==bSCI2C_GetStatus(&slaveStatus))
		{
			return false;
		}
		if (slaveStatus == PROCESSING_READY)
		{
			//nrf_delay_ms(tRSTG);
			for(i=0;i<SCI2C_RETRY_TIMERS;i++)
			{
				if ((usLen-offset) > maxpacklen)
				{
					Pcb = (uint8_t)(pcbEDC | SCI2C_PCB_DATAMS | ((s_ucMasterSeqCount&SCI2C_PCB_DATA_SC_VALIDB)<<SCI2C_PCB_DATA_SC_SHIFT) | SCI2C_PCB_DATA_M);
					//if(true == I2C_BlockWrite(Pcb,pInBuf+offset,maxpacklen))
					if (I2C_NO_ERROR==ucSI2CDRV_WriteDataByPcb(Pcb,pInBuf+offset,maxpacklen))
					{
							offset += maxpacklen ;							
							s_ucMasterSeqCount = (s_ucMasterSeqCount>=7)?0:++s_ucMasterSeqCount ;
							break;
					}
					else  //I2C״̬�������½��г�ʼ��
					{
						vSCI2C_Init(0x01);
						nrf_delay_ms(20);	
					}
				}
				else
				{
					Pcb = (uint8_t)(pcbEDC | SCI2C_PCB_DATAMS | ((s_ucMasterSeqCount&SCI2C_PCB_DATA_SC_VALIDB)<<SCI2C_PCB_DATA_SC_SHIFT));
					//if(true == I2C_BlockWrite(Pcb,pInBuf+offset,usLen-offset))
					if (I2C_NO_ERROR==ucSI2CDRV_WriteDataByPcb(Pcb,pInBuf+offset,usLen-offset))
					{
							s_ucMasterSeqCount = (s_ucMasterSeqCount>=7)?0:++s_ucMasterSeqCount ;
							offset = usLen ;
							nrf_delay_ms(10);
							return true;
					}	
					else  //I2C״̬�������½��г�ʼ��
					{
						vSCI2C_Init(0x01);
						nrf_delay_ms(20);	
					}
				}
				//�ط���Ҫ�ӳ�
				nrf_delay_ms(tFW+tMD);
			}
		}
		OvertimeCount++ ;

	}while(OvertimeCount<tMAXTRY_MS);
//	status = 0 ;
	return false;
}

 /*****************************************************************************
 ����:  bSCI2C_DataExchangeSToM
 ����:	��

 ���:	pOutBuf:��Ŵ��豸��������ָ��
				usLen��
 		
 ����:	false/true,ע��û��׼����Ҳ����true
 		
 ����:  �������豸����豸��ȡ����,
				
******************************************************************************/
bool bSCI2C_DataExchangeSToM(uint8_t *pOutBuf, uint16_t *usLen)
{
	uint8_t i,Pcb,outpcb,slaveStatus,status ;
	uint16_t OvertimeCount,retlen=0;
	
	OvertimeCount = 0 ;
	do{
		nrf_delay_ms(15);
		//get status
		if (false==bSCI2C_GetStatus(&slaveStatus))
		{
			return false;
		}	
		if (slaveStatus == PROCESSING_READY)  //ready
		{
			Pcb = SCI2C_PCB_DATASM ;
			for(i=0;i<SCI2C_RETRY_TIMERS;i++)
			{
				outpcb = 0 ;
				status = ucSI2CDRV_ReadDataByPcb(Pcb,&outpcb,pOutBuf+g_usI2COffset,&retlen);
				if (retlen > SCI2C_CDBSM_I3)  //���ճ��Ȳ����ܴ���Э�����ֵ,���ذ���pcb
				{
					return false;
				}
				if (I2C_NO_ERROR == status)
				{
					//�ж��Ƿ���edc
					if ((outpcb&SCI2C_PCB_DATA_EDC_MASK)==SCI2C_PCB_DATA_EDC_LRC)
					{
							uint8_t EDC = 0x00;
							if (retlen > 2) //applet�ڲ�����������»��յ�����Ϊ0������
							{
								EDC = bSCI2C_CalcLRC( pOutBuf+g_usI2COffset, retlen - 1);
								if (EDC == pOutBuf[retlen+g_usI2COffset - 1])
								{
									Pcb = (uint8_t)(Pcb & (~SCI2C_PCB_DATA_M));
									retlen -= 2 ;
								}		
								else
								{
									// Request retransmission
									Pcb = (uint8_t)(Pcb | (SCI2C_PCB_DATA_M));
									continue ;
								}
							}
					}
					g_usI2COffset += retlen ;
					//�ж��Ƿ�Ϊ���һ��
					if((outpcb&SCI2C_PCB_DATA_M)==0)
					{
						*usLen = g_usI2COffset;
						g_usI2COffset = 0 ;
						g_ucI2CRecCompleteFlag =0x01;
					}
					return true;  
				}		
				//�ط���Ҫ�ӳ�
				nrf_delay_ms(tFW+tMD);
			}
			return false;
		}
		OvertimeCount++ ;
	}while(OvertimeCount < tMAXTRY_SM);

	return false;
}
 /*****************************************************************************
 ����:  vSCI2C_ErrorDeal
 ����:	��

 ���:	��
 		
 ����:	��
 		
 ����:  ���ڵ�I2Cͨ�ų����������豸����һ����λ
				
******************************************************************************/
//void vSCI2C_ErrorDeal(void)
//{
//		bSCI2C_SoftReset();
//		nrf_delay_ms(20);
//		bSCI2C_ReadATR();
//		nrf_delay_ms(tRSTG);
//		bSCI2C_ParameterExchange();
//		nrf_delay_ms(tRSTG);
//}
