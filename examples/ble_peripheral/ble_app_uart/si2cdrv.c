#include <string.h>
#include "app_error.h"
#include "sys.h"
#include "nrf_delay.h"
#include "app_util_platform.h"

//A7������ַ
#define A7_CHIP_BASE_ADDR	(0x90)

/*****************************************************************************
 ����:	vSI2CDRV_DelayUs
 ���룺
		usUs
 �����	
 		��
 ����:
 		��
 ����:
		�ú�������I2C����������ʱ
 ˵��:	
 		SCL�͵�ƽ>=4.7us,SCL�ߵ�ƽ>=4.0us�����ݿ���ʱ��>=4.7us
******************************************************************************/
#define	vSI2CDRV_DelayUs(uiUs)				nrf_delay_us(uiUs);

/*****************************************************************************
 ����:	vSI2CDRV_Start
 ���룺
		��
 �����	
 		��
 ����:
 		��
 ����:
		�ú�������I2C��������ʱ��
 ˵��:	
 		SCL�͵�ƽ>=4.7us,SCL�ߵ�ƽ>=4.0us�����ݿ���ʱ��>=4.7us
******************************************************************************/
static void vSI2CDRV_Start(void)
{
	//SDA��ʼ��Ϊ�ߵ�ƽ"1"
	I2C_SCL_HIGH();								
	vSI2CDRV_DelayUs(SI2CDRV_DELAY_4US);
	
	//��ʼ���ݴ���ʱ��Ҫ��SCLΪ�ߵ�ƽ"1"
	I2C_SDA_HIGH();  							
	vSI2CDRV_DelayUs(SI2CDRV_DELAY_4US);

	//SDA���½��ر���Ϊ�ǿ�ʼ�ź�
	I2C_SDA_LOW();								
	vSI2CDRV_DelayUs(SI2CDRV_DELAY_4US);
	//SCLΪ�͵�ƽʱ��SDA�����ݲ�����仯(�������Ժ�����ݴ��ݣ�
	I2C_SCL_LOW();
	vSI2CDRV_DelayUs(SI2CDRV_DELAY_4US);
	
	
	
}
/*****************************************************************************
 ����:	vSI2CDRV_Stop
 ���룺
		��
 �����	
 		��
 ����:
 		��
 ����:
		�ú�������I2C����ֹͣʱ��
 ˵����
 		SCL�͵�ƽ>=4.7us,SCL�ߵ�ƽ>=4.0us�����ݿ���ʱ��>=4.7us
******************************************************************************/
static void vSI2CDRV_Stop(void)
{	
	//SDA��ʼ��Ϊ�͵�ƽ"0"
	I2C_SDA_LOW();								
	vSI2CDRV_DelayUs(SI2CDRV_DELAY_4US);
	
	//�������ݴ���ʱ��Ҫ��SCLΪ�ߵ�ƽ"1
	I2C_SCL_HIGH();
	vSI2CDRV_DelayUs(SI2CDRV_DELAY_4US);

	//SDA�������ر���Ϊ�ǽ����ź�
	I2C_SDA_HIGH();								
}
/*****************************************************************************
 ����:	ucSI2CDRV_GetAck
 ���룺
		��
 �����	
 		��
 ����:
 		0���ӻ�Ӧ��
 	     1���ӻ���Ӧ��
 ����:
		�ú������ڶ�ȡ�ӻ�Ӧ��λ��Ӧ����Ӧ�𣩣������жϣ��ӻ��Ƿ�ɹ�������������
 ˵����
 		�ӻ����յ�ÿһ���ֽں�Ҫ����Ӧ��λ����������յ���Ӧ����Ӧ����ֹ����
******************************************************************************/
static uint8_t ucSI2CDRV_GetAck(void)
{

	uint8_t ucAck;

	//I2C_SDA_HIGH(); 
	I2C_SDA_INPUT(); 
	
	ucAck = I2C_SDA_STATUS;
	I2C_SCL_HIGH();
	vSI2CDRV_DelayUs(SI2CDRV_DELAY_4US);
	I2C_SCL_LOW();   
	I2C_SDA_OUTPUT();
	I2C_SDA_HIGH();

	if(0x00 == ucAck)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
/*****************************************************************************
 ����:	vSI2CDRV_PutAck
 ���룺
		ucAck = 0������Ӧ��
		ucAck = 1��������Ӧ��
 �����	
 		��
 ����:
 		��
 ����:
		�ú���������������Ӧ��λ��Ӧ����Ӧ�𣩣�����֪ͨ�ӻ��������Ƿ�ɹ����մӻ�����
 ˵����
 		�������յ�ÿһ���ֽں�Ҫ����Ӧ�����յ����һ���ֽ�ʱ��Ӧ��������Ӧ��
******************************************************************************/
static void vSI2CDRV_PutAck(uint8_t ucAck)
{
	if(ucAck==1)
	{
		I2C_SDA_HIGH();  
	}
	if(ucAck==0)
	{
		I2C_SDA_LOW();
	}
	
	I2C_SCL_HIGH();
	
	vSI2CDRV_DelayUs(SI2CDRV_DELAY_4US);

	I2C_SCL_LOW(); 

	//����ack����Ҫdelay��������׼������һ������
	vSI2CDRV_DelayUs(SI2CDRV_DELAY_40US);
}	

/*****************************************************************************
 ����:	vSI2CDRV_SendByte
 ���룺
		ucWRData����Ҫ���͵�����
 �����	
 		��
 ����:
 		��
 ����:
		�ú�������I2C���߷���һ���ֽ����ݡ�
 ˵����
 		
******************************************************************************/
static void vSI2CDRV_SendByte(uint8_t ucWRData)
{

	uint8_t ucCount;
	for(ucCount = 0; ucCount<8; ucCount++)
	{
		//����������
		if((ucWRData<<ucCount)&0x80) 
		{
			I2C_SDA_HIGH();
		}
		else
		{
			I2C_SDA_LOW();
		}
		
		vSI2CDRV_DelayUs(SI2CDRV_DELAY_4US);
		I2C_SCL_HIGH();
		vSI2CDRV_DelayUs(SI2CDRV_DELAY_8US);
		I2C_SCL_LOW();
		vSI2CDRV_DelayUs(SI2CDRV_DELAY_4US);
	}
}
/*****************************************************************************
 ����:	ucSI2CDRV_ReceiveByte
 ���룺
		��
 �����	
 		��
 ����:
 		���յ���1�ֽ�I2C����
 ����:
		�ú������ڴ��豸I2C���߽���һ���ֽ����ݡ�
 ˵����
 		
******************************************************************************/
static uint8_t ucSI2CDRV_ReceiveByte(void)
{
	volatile uint8_t s;
	uint8_t ucDat = 0;

	//�ͷ�SDA���ߣ�׼�����ܴӻ�����
	I2C_SDA_HIGH();
	I2C_SDA_INPUT();
	
	for(s=0;s<8;s++)
	{
		I2C_SCL_LOW();
		vSI2CDRV_DelayUs(SI2CDRV_DELAY_8US);
		I2C_SCL_HIGH();
		vSI2CDRV_DelayUs(SI2CDRV_DELAY_8US);

		ucDat <<= 1;
		if(0 == I2C_SDA_STATUS)
		{
			ucDat |= 0;
		}
		else
		{
			ucDat |= 1;
		}
	}

	I2C_SDA_OUTPUT();
	I2C_SCL_LOW();
//	vSI2CDRV_DelayUs(SI2CDRV_DELAY_4US);
	
	vSI2CDRV_DelayUs(SI2CDRV_DELAY_16US);
	
	return ucDat;	
}

/*****************************************************************************
 ����:	ucSI2CDRV_BlockRead
 ���룺
		ucAddr: 		�豸��ַ
		ucInPcb:		�豸����PCB
		pucDes: 		�豸����PCB
		pucBuffer:	�������ݽ���buff
		
 ����� 
		pusLen:		Ҫ�����ĳ���
 ����:
		������μ�SI2CDRV_ERR_CODE
 ����:
		�ú�������ģ��I2C�ӿڿ��ȡ
******************************************************************************/
static uint8_t ucSI2CDRV_BlockRead(uint8_t ucAddr, uint8_t ucInPcb, uint8_t * pucRecvPcb, uint8_t * pucBuffer, uint16_t * pusLen)
{
	uint8_t i,j,ucLen;
	
	I2C_SCL_HIGH();
	I2C_SDA_HIGH();
	
	vSI2CDRV_DelayUs(SI2CDRV_DELAY_16US);
	
	//�������߲����ʹ�������ַ
	vSI2CDRV_Start();												
	vSI2CDRV_SendByte(ucAddr);
	if(0 == ucSI2CDRV_GetAck())
	{
		return I2C_NO_ACK;
	}
	
	//��������pcb����ȡAck
	vSI2CDRV_SendByte(ucInPcb);	
	if(0 == ucSI2CDRV_GetAck())
	{
		return I2C_NO_ACK;
	}
	
	//������PCB����Ҫ��delayһ�ᣬ������׼���ý���һ������
	vSI2CDRV_DelayUs(SI2CDRV_DELAY_100US);
	j = 5;
	do{
		if(0==j)
		{
			return I2C_NO_ACK;
		}
		vSI2CDRV_DelayUs(SI2CDRV_DELAY_100US);
		//���ʹ�������ַ�Ͷ�ָ����ҽ���Ack
		vSI2CDRV_Start();
		vSI2CDRV_SendByte(ucAddr+1);
		j--;
	}while(0 == ucSI2CDRV_GetAck());
	

	
	//�������ָ�����Ҫ��delayһ�ᣬ������׼���÷����Ժ������
	vSI2CDRV_DelayUs(SI2CDRV_DELAY_100US/*SI2CDRV_DELAY_40US*/);
	
	//��ȡ���Ȳ���ACK
	ucLen = ucSI2CDRV_ReceiveByte();
	vSI2CDRV_PutAck(0);
	if (ucLen > 0)
	{
		*pucRecvPcb= ucSI2CDRV_ReceiveByte();
		if (ucLen < 2)
		{
		 	//���ͷ�Ӧ���ź�
			vSI2CDRV_PutAck(1); 						
		
			vSI2CDRV_Stop();
				
			//ȥ��pcb�ĳ���
			*pusLen = 0;
		
			return I2C_NO_ERROR;
		}
		//����Ӧ���ź�
		vSI2CDRV_PutAck(0);
	}
	else
	{
		return I2C_WRONGLEN ;
	}
	for(i =0;i<(ucLen-2);i++)
	{
		//������
		*pucBuffer = ucSI2CDRV_ReceiveByte();	
		pucBuffer++;
		//����Ӧ���ź�
		vSI2CDRV_PutAck(0);
	}
	//�����һ������
	*pucBuffer = ucSI2CDRV_ReceiveByte();		

	//���ͷ�Ӧ���ź�
	vSI2CDRV_PutAck(1); 						

	vSI2CDRV_Stop();
		
	//ȥ��pcb�ĳ���
	*pusLen = ucLen-1;

	return I2C_NO_ERROR;

}
/*****************************************************************************
 ����:	ucSI2CDRV_BlockRead
 ���룺
		ucAddr: 		�豸��ַ
		ucInPcb:		�豸����PCB
		pucSrc:		��������buff
		ucLen:		Ҫ����ĳ���
		
 ����� 
		��
 ����:
		������μ�SI2CDRV_ERR_CODE
 ����:
		�ú�������ģ��I2C�ӿڿ�д��
******************************************************************************/
static uint8_t ucSI2CDRV_BlockWrite(uint8_t ucAddr,uint8_t ucInPcb ,uint8_t * pucSrc, uint8_t ucLen)
{
	uint8_t i;
	
	I2C_SCL_HIGH();
	I2C_SDA_HIGH();
	
	vSI2CDRV_DelayUs(SI2CDRV_DELAY_16US);

	//�������߲����ʹ�������ַ
	vSI2CDRV_Start();												
	vSI2CDRV_SendByte(ucAddr);
	if(0 == ucSI2CDRV_GetAck())
	{
		return I2C_NO_ACK;
	}
	
	//��������pcb����ȡAck
	vSI2CDRV_SendByte(ucInPcb);	
	if(0 == ucSI2CDRV_GetAck())
	{
		return I2C_NO_ACK;
	}
	if (ucLen > 0)  
	{
		//���ͳ��Ȳ���ȡACK
		vSI2CDRV_SendByte(ucLen);
		ucSI2CDRV_GetAck();
			
		//������PCB����Ҫ��delayһ�ᣬ������׼���ý���һ������
		vSI2CDRV_DelayUs(SI2CDRV_DELAY_100US);
		
		for(i =0;i<ucLen;i++)
		{
			//��������
			vSI2CDRV_SendByte(pucSrc[i]);
			//����Ӧ���ź�
			ucSI2CDRV_GetAck();
		}						
	}
	vSI2CDRV_Stop(); 
	
	return I2C_NO_ERROR;
}

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
void vSI2CDRV_Init(void)
{
	GPIO_OUT_CONFIG(IO_I2C_SDA,NRF_GPIO_PIN_PULLUP);
	GPIO_OUT_CONFIG(IO_I2C_SCL,NRF_GPIO_PIN_PULLUP);
	//����������ʱ����
	I2C_SDA_HIGH();
	I2C_SCL_HIGH();

	nrf_delay_us(1000);
}


/*****************************************************************************
 ����:	vSI2CDRV_Recv
 ���룺
 		ucMode:		1��read/0��write

 �����	
 		��
 ����:
 		��
 ����:
		�ú�������ģ��I2C�ӿڷ��Ϳ�������
******************************************************************************/
uint8_t vSI2CDRV_QuickCmd(uint8_t ucMode)
{
	I2C_SCL_HIGH();
	I2C_SDA_HIGH();
	
	vSI2CDRV_DelayUs(SI2CDRV_DELAY_16US);

	//�������߲����ʹ�������ַ
	vSI2CDRV_Start();		
	
	vSI2CDRV_SendByte(A7_CHIP_BASE_ADDR+ucMode);
	if(0 == ucSI2CDRV_GetAck())
	{
		return I2C_NO_ACK;
	}
	vSI2CDRV_Stop(); 
	
	return I2C_NO_ERROR;	
}

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
uint8_t ucSI2CDRV_WriteDataByPcb(uint8_t ucInPcb ,uint8_t * pucSrc, uint8_t ucLen)
{
	return ucSI2CDRV_BlockWrite(A7_CHIP_BASE_ADDR,ucInPcb,pucSrc, ucLen);
}

/******************************************************************************
 ����:  I2C_ReadDataByPcb
 ����:	
 		ucInPcb:���豸��Pcbֵ
 ���:	pOutBuf:���豸��Ӧ������ָ�루�������е�һ��ΪPCB��
		ucResLen:���豸��Ӧ�����ݳ���,��Ϊ1ʱ��ʾֻ����pcb
 		
 ����:	
 		������μ�SI2CDRV_ERR_CODE
 ����:  
 		�ú�������ģ��I2C�ӿ�ͨ��PCB��ȡ����
******************************************************************************/
uint8_t ucSI2CDRV_ReadDataByPcb(uint8_t ucInPcb,uint8_t *pucRecvPcb ,uint8_t * pucOut, uint16_t * pusOutLen)
{
	return ucSI2CDRV_BlockRead(A7_CHIP_BASE_ADDR,ucInPcb,pucRecvPcb,pucOut,pusOutLen);	
}


