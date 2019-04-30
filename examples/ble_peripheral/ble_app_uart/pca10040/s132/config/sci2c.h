#ifndef SCI2C_H_
#define SCI2C_H_

#include"stdbool.h"
#include"stdint.h"

//slave status define
#define		SLAVE_READY					0
#define		SLAVE_ACTIVE					1
#define		SLAVE_PWRSAV					2
#define		SLAVE_WAITRESET			4
#define		SLAVE_PROCESSING			8

//processing status Code define 
#define		STATUS_SHIFT							4
#define		PROCESSING_READY					0x00
#define		PROCESSING_BUSY						0x01
#define		PROCESSING_NOMOREINFO			0x08
#define		PROCESSING_OVERCLOCKING 	0x09
#define		PROCESSING_UNEXPECTEDSEQ	0x0A
#define		PROCESSING_INVALIDDATALEN	0x0B
#define		PROCESSING_UNEXPECTCMD		0x0C
#define		PROCESSING_INVALIDDEC			0x0E
#define		PROCESSING_OTHERERROR			0x0F

//定义ATR中的TAG
#define		ATR_LOW_LEVEL							0xB8
#define		ATR_PROTOCOL_BINDING			0xB9
#define		ATR_HIGH_LAYER						0xBA
#define		ATR_OPERATION_SYSTEM			0xBB
#define		ATR_OPERATION_SYSTEM			0xBB

//
#define		MASTER_SUPPORT_EDC				1
#define		MASTER_NOSUPPORT_EDC			0
// Defines for the SCI2C protocol implementation part...
#define SCI2C_PCB_WAKEUP         0x0F
#define SCI2C_PCB_STATUS         0x07
#define SCI2C_PCB_STATUS_MASK    0x0F
#define SCI2C_PCB_DATA_PENDING   0x10
#define SCI2C_PCB_UNEXPECTED_CMD 0xC0
#define SCI2C_PCB_NO_DATA_PENDING 0x00
#define SCI2C_PCB_ERROR_MASK     0xF0
#define SCI2C_PCB_SOFTRESET      0x1F
#define SCI2C_PCB_POWERDOWN      0xCF // Required for SC1W
#define SCI2C_PCB_READATR        0x2F
#define SCI2C_PCB_PE_MAX         0xFF  // 最大支持的
#define SCI2C_PCB_PE_CDBIMS_MASK 0xC0
#define SCI2C_PCB_PE_CDBIMS_SHIFT 6
#define SCI2C_PCB_DATA_MASK      0x03 // mask to check for data exchange command
#define SCI2C_PCB_DATAMS         0x00 // master to slave data exchange
#define SCI2C_PCB_DATASM         0x02 // slave to master data exchange
#define SCI2C_PCB_DATA_EDC_MASK  0x0C // error detection code
#define SCI2C_PCB_DATA_EDC_SHIFT 0x02
#define SCI2C_PCB_DATA_EDC_NONE  0x00 // no error detection code used
#define SCI2C_PCB_DATA_EDC_LRC   0x04 // LRC detection code used
#define SCI2C_PCB_DATA_SC_MASK   0x70 // sequence counter
#define SCI2C_PCB_DATA_SC_SHIFT  4
#define SCI2C_PCB_DATA_SC_VALIDB 0x07 // sequence counter valid bits
#define SCI2C_PCB_DATA_M         0x80 // more bit

//slave pcb 
#define SCI2C_PCBSM_SOFTRESET    0x00
#define SCI2C_PCBSM_READATR      0x00
#define SCI2C_PCBSM_EXCEPTION    0x01 // slave signals an exception
#define SCI2C_PCBSM_PE           0x00
#define SCI2C_PCBSM_PE_MASK      0x03
#define SCI2C_PCBSM_PE_CDBISM_MASK 0x0C
#define SCI2C_PCBSM_PE_CDBISM_SHIFT 2
#define SCI2C_PCBSM_PE_CDBISM_C_MASK 0x30
#define SCI2C_PCBSM_PE_CDBISM_C_SHIFT 4
#define SCI2C_PCBSM_PE_CDBIMS_C_MASK 0xC0
#define SCI2C_PCBSM_PE_CDBIMS_C_SHIFT 6

//master => slave
#define SCI2C_CDBMS_DEFAULT       32 //!< by default the master is allowed to place up to 32 bytes into a single master to slave data transmission
#define SCI2C_CDBMS_I0            32 //!< CDBIMS=0 => CDBMS=32
#define SCI2C_CDBMS_I1            64 //!< CDBIMS=1 => CDBMS=64
#define SCI2C_CDBMS_I2           128 //!< CDBIMS=2 => CDBMS=128
#define SCI2C_CDBMS_I3           253 //!< CDBIMS=3 => CDBMS=253
//slave => master
#define SCI2C_CDBSM_DEFAULT       29 //!< by default the slave is allowed to place up to 29 bytes into a single slave to master data transmission
#define SCI2C_CDBSM_I0            29 //!< CDBISM=0 => CDBSM=29
#define SCI2C_CDBSM_I1            61 //!< CDBISM=1 => CDBSM=61
#define SCI2C_CDBSM_I2           125 //!< CDBISM=2 => CDBSM=125
#define SCI2C_CDBSM_I3           253 //!< CDBISM=3 => CDBSM=253

//retry timers
#define	SCI2C_RETRY_TIMERS				2

// Timing
#define tCMDG  	4 //180  //4*50=200us
#define tMD   	2  //ms
#define tRSTG 	5  //ms
#define tFW			10  //ms

#define tKB					5  //ms 按键防抖时间
#define tMAXTRY_MS			50  //ms 写最大等待时间
#define tMAXTRY_SM			200  //ms 读最大等待时间

extern uint8_t g_ucI2CRecCompleteFlag ;
extern uint16_t g_usI2COffset ;

extern bool bSCI2C_PowerDown(void);
extern uint8_t bSCI2C_WakeUp(void);
extern bool bSCI2C_SoftReset(void);
extern bool bSCI2C_GetStatus(uint8_t *pStatus);
extern void vSCI2C_Init(uint8_t ucSleep);
extern bool bSCI2C_ReadATR(void);
extern bool bSCI2C_ParameterExchange(void);
extern bool bSCI2C_DataExchangeMToS(uint8_t *pInBuf, uint16_t usLen);
extern bool bSCI2C_DataExchangeSToM(uint8_t *pOutBuf, uint16_t *usLen);
//extern void vSCI2C_ErrorDeal(void);
extern void vSCI2C_PowerOn(void);
bool i2ctest (void);
#endif   //end pi2c_h
