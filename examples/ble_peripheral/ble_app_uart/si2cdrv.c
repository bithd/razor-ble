#include <string.h>
#include "app_error.h"
#include "sys.h"
#include "nrf_delay.h"
#include "app_util_platform.h"

//A7基础地址
#define A7_CHIP_BASE_ADDR	(0x90)

/*****************************************************************************
 函数:	vSI2CDRV_DelayUs
 输入：
		usUs
 输出：	
 		无
 返回:
 		无
 功能:
		该函数用于I2C总线启动延时
 说明:	
 		SCL低电平>=4.7us,SCL高电平>=4.0us，数据空闲时间>=4.7us
******************************************************************************/
#define	vSI2CDRV_DelayUs(uiUs)				nrf_delay_us(uiUs);

/*****************************************************************************
 函数:	vSI2CDRV_Start
 输入：
		无
 输出：	
 		无
 返回:
 		无
 功能:
		该函数用于I2C总线启动时序。
 说明:	
 		SCL低电平>=4.7us,SCL高电平>=4.0us，数据空闲时间>=4.7us
******************************************************************************/
static void vSI2CDRV_Start(void)
{
	//SDA初始化为高电平"1"
	I2C_SCL_HIGH();								
	vSI2CDRV_DelayUs(SI2CDRV_DELAY_4US);
	
	//开始数据传送时，要求SCL为高电平"1"
	I2C_SDA_HIGH();  							
	vSI2CDRV_DelayUs(SI2CDRV_DELAY_4US);

	//SDA的下降沿被认为是开始信号
	I2C_SDA_LOW();								
	vSI2CDRV_DelayUs(SI2CDRV_DELAY_4US);
	//SCL为低电平时，SDA上数据才允许变化(即允许以后的数据传递）
	I2C_SCL_LOW();
	vSI2CDRV_DelayUs(SI2CDRV_DELAY_4US);
	
	
	
}
/*****************************************************************************
 函数:	vSI2CDRV_Stop
 输入：
		无
 输出：	
 		无
 返回:
 		无
 功能:
		该函数用于I2C总线停止时序。
 说明：
 		SCL低电平>=4.7us,SCL高电平>=4.0us，数据空闲时间>=4.7us
******************************************************************************/
static void vSI2CDRV_Stop(void)
{	
	//SDA初始化为低电平"0"
	I2C_SDA_LOW();								
	vSI2CDRV_DelayUs(SI2CDRV_DELAY_4US);
	
	//结束数据传送时，要求SCL为高电平"1
	I2C_SCL_HIGH();
	vSI2CDRV_DelayUs(SI2CDRV_DELAY_4US);

	//SDA的上升沿被认为是结束信号
	I2C_SDA_HIGH();								
}
/*****************************************************************************
 函数:	ucSI2CDRV_GetAck
 输入：
		无
 输出：	
 		无
 返回:
 		0－从机应答
 	     1－从机非应答
 功能:
		该函数用于读取从机应答位（应答或非应答），用于判断：从机是否成功接收主机数据
 说明：
 		从机在收到每一个字节后都要产生应答位，主机如果收到非应答则应当终止传输
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
 函数:	vSI2CDRV_PutAck
 输入：
		ucAck = 0：主机应答
		ucAck = 1：主机非应答
 输出：	
 		无
 返回:
 		无
 功能:
		该函数用于主机产生应答位（应答或非应答），用于通知从机：主机是否成功接收从机数据
 说明：
 		主机在收到每一个字节后都要产生应答，在收到最后一个字节时，应当产生非应答
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

	//发完ack后需要delay，让总线准备好下一个数据
	vSI2CDRV_DelayUs(SI2CDRV_DELAY_40US);
}	

/*****************************************************************************
 函数:	vSI2CDRV_SendByte
 输入：
		ucWRData：需要发送的数据
 输出：	
 		无
 返回:
 		无
 功能:
		该函数用于I2C总线发送一个字节数据。
 说明：
 		
******************************************************************************/
static void vSI2CDRV_SendByte(uint8_t ucWRData)
{

	uint8_t ucCount;
	for(ucCount = 0; ucCount<8; ucCount++)
	{
		//置数据总线
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
 函数:	ucSI2CDRV_ReceiveByte
 输入：
		无
 输出：	
 		无
 返回:
 		接收到的1字节I2C数据
 功能:
		该函数用于从设备I2C总线接受一个字节数据。
 说明：
 		
******************************************************************************/
static uint8_t ucSI2CDRV_ReceiveByte(void)
{
	volatile uint8_t s;
	uint8_t ucDat = 0;

	//释放SDA总线，准备接受从机数据
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
 函数:	ucSI2CDRV_BlockRead
 输入：
		ucAddr: 		设备地址
		ucInPcb:		设备传入PCB
		pucDes: 		设备传出PCB
		pucBuffer:	传出数据接收buff
		
 输出： 
		pusLen:		要传出的长度
 返回:
		错误码参见SI2CDRV_ERR_CODE
 功能:
		该函数用于模拟I2C接口块读取
******************************************************************************/
static uint8_t ucSI2CDRV_BlockRead(uint8_t ucAddr, uint8_t ucInPcb, uint8_t * pucRecvPcb, uint8_t * pucBuffer, uint16_t * pusLen)
{
	uint8_t i,j,ucLen;
	
	I2C_SCL_HIGH();
	I2C_SDA_HIGH();
	
	vSI2CDRV_DelayUs(SI2CDRV_DELAY_16US);
	
	//开启总线并发送从器件地址
	vSI2CDRV_Start();												
	vSI2CDRV_SendByte(ucAddr);
	if(0 == ucSI2CDRV_GetAck())
	{
		return I2C_NO_ACK;
	}
	
	//发送请求pcb并读取Ack
	vSI2CDRV_SendByte(ucInPcb);	
	if(0 == ucSI2CDRV_GetAck())
	{
		return I2C_NO_ACK;
	}
	
	//发送完PCB后需要多delay一会，让总线准备好接下一个数据
	vSI2CDRV_DelayUs(SI2CDRV_DELAY_100US);
	j = 5;
	do{
		if(0==j)
		{
			return I2C_NO_ACK;
		}
		vSI2CDRV_DelayUs(SI2CDRV_DELAY_100US);
		//发送从器件地址和读指令，并且接受Ack
		vSI2CDRV_Start();
		vSI2CDRV_SendByte(ucAddr+1);
		j--;
	}while(0 == ucSI2CDRV_GetAck());
	

	
	//发送完读指令后需要多delay一会，让总线准备好发送以后的数据
	vSI2CDRV_DelayUs(SI2CDRV_DELAY_100US/*SI2CDRV_DELAY_40US*/);
	
	//获取长度并给ACK
	ucLen = ucSI2CDRV_ReceiveByte();
	vSI2CDRV_PutAck(0);
	if (ucLen > 0)
	{
		*pucRecvPcb= ucSI2CDRV_ReceiveByte();
		if (ucLen < 2)
		{
		 	//发送非应答信号
			vSI2CDRV_PutAck(1); 						
		
			vSI2CDRV_Stop();
				
			//去除pcb的长度
			*pusLen = 0;
		
			return I2C_NO_ERROR;
		}
		//发送应答信号
		vSI2CDRV_PutAck(0);
	}
	else
	{
		return I2C_WRONGLEN ;
	}
	for(i =0;i<(ucLen-2);i++)
	{
		//读数据
		*pucBuffer = ucSI2CDRV_ReceiveByte();	
		pucBuffer++;
		//发送应答信号
		vSI2CDRV_PutAck(0);
	}
	//读最后一个数据
	*pucBuffer = ucSI2CDRV_ReceiveByte();		

	//发送非应答信号
	vSI2CDRV_PutAck(1); 						

	vSI2CDRV_Stop();
		
	//去除pcb的长度
	*pusLen = ucLen-1;

	return I2C_NO_ERROR;

}
/*****************************************************************************
 函数:	ucSI2CDRV_BlockRead
 输入：
		ucAddr: 		设备地址
		ucInPcb:		设备传入PCB
		pucSrc:		传入数据buff
		ucLen:		要传入的长度
		
 输出： 
		无
 返回:
		错误码参见SI2CDRV_ERR_CODE
 功能:
		该函数用于模拟I2C接口块写入
******************************************************************************/
static uint8_t ucSI2CDRV_BlockWrite(uint8_t ucAddr,uint8_t ucInPcb ,uint8_t * pucSrc, uint8_t ucLen)
{
	uint8_t i;
	
	I2C_SCL_HIGH();
	I2C_SDA_HIGH();
	
	vSI2CDRV_DelayUs(SI2CDRV_DELAY_16US);

	//开启总线并发送从器件地址
	vSI2CDRV_Start();												
	vSI2CDRV_SendByte(ucAddr);
	if(0 == ucSI2CDRV_GetAck())
	{
		return I2C_NO_ACK;
	}
	
	//发送请求pcb并读取Ack
	vSI2CDRV_SendByte(ucInPcb);	
	if(0 == ucSI2CDRV_GetAck())
	{
		return I2C_NO_ACK;
	}
	if (ucLen > 0)  
	{
		//发送长度并读取ACK
		vSI2CDRV_SendByte(ucLen);
		ucSI2CDRV_GetAck();
			
		//发送完PCB后需要多delay一会，让总线准备好接下一个数据
		vSI2CDRV_DelayUs(SI2CDRV_DELAY_100US);
		
		for(i =0;i<ucLen;i++)
		{
			//发送数据
			vSI2CDRV_SendByte(pucSrc[i]);
			//接收应答信号
			ucSI2CDRV_GetAck();
		}						
	}
	vSI2CDRV_Stop(); 
	
	return I2C_NO_ERROR;
}

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
void vSI2CDRV_Init(void)
{
	GPIO_OUT_CONFIG(IO_I2C_SDA,NRF_GPIO_PIN_PULLUP);
	GPIO_OUT_CONFIG(IO_I2C_SCL,NRF_GPIO_PIN_PULLUP);
	//拉高数据与时钟线
	I2C_SDA_HIGH();
	I2C_SCL_HIGH();

	nrf_delay_us(1000);
}


/*****************************************************************************
 函数:	vSI2CDRV_Recv
 输入：
 		ucMode:		1：read/0：write

 输出：	
 		无
 返回:
 		无
 功能:
		该函数用于模拟I2C接口发送快速命令
******************************************************************************/
uint8_t vSI2CDRV_QuickCmd(uint8_t ucMode)
{
	I2C_SCL_HIGH();
	I2C_SDA_HIGH();
	
	vSI2CDRV_DelayUs(SI2CDRV_DELAY_16US);

	//开启总线并发送从器件地址
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
uint8_t ucSI2CDRV_WriteDataByPcb(uint8_t ucInPcb ,uint8_t * pucSrc, uint8_t ucLen)
{
	return ucSI2CDRV_BlockWrite(A7_CHIP_BASE_ADDR,ucInPcb,pucSrc, ucLen);
}

/******************************************************************************
 函数:  I2C_ReadDataByPcb
 输入:	
 		ucInPcb:主设备的Pcb值
 输出:	pOutBuf:从设备相应的数据指针（数据域中第一个为PCB）
		ucResLen:从设备相应的数据长度,当为1时表示只返回pcb
 		
 返回:	
 		错误码参见SI2CDRV_ERR_CODE
 功能:  
 		该函数用于模拟I2C接口通过PCB读取数据
******************************************************************************/
uint8_t ucSI2CDRV_ReadDataByPcb(uint8_t ucInPcb,uint8_t *pucRecvPcb ,uint8_t * pucOut, uint16_t * pusOutLen)
{
	return ucSI2CDRV_BlockRead(A7_CHIP_BASE_ADDR,ucInPcb,pucRecvPcb,pucOut,pusOutLen);	
}


