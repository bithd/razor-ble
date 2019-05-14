#ifndef _SYS_H_
#define _SYS_H_

#include <stdint.h>
#include "IO_CTL.h"
#include "si2cdrv.h"
#include "Usr_battery.h"
#include "app_util.h"
#include "Usr_battery.h"

//#define DEVICE_NAME                     "RAZOR0000001"//"RAZOR"//                          /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME               "bithd.com"                       /**< Manufacturer. Will be passed to Device Information Service. */
#define FW_VERSION                			"4.0.0"                       /**< FW. Will be passed to Device Information Service. */
c

#define ONE_50MS_INTERVAL			   	APP_TIMER_TICKS(50) 
#define ONE_SECOND_INTERVAL			   	APP_TIMER_TICKS(500) 
#define T_SHUTDOWN_TIMEOUT				APP_TIMER_TICKS(3000)
#define SECURITY_REQUEST_DELAY          APP_TIMER_TICKS(400)                        /**< Delay after connection until Security Request is sent, if necessary (ticks). */

#define SHUTDOWN_TIME					200  //3*x200=600s

#define DATAFIELD_MAX_LEN		1024
#define APDU_HEAD_LEN			8
#define APDU_MAX_LEN			(APDU_HEAD_LEN+DATAFIELD_MAX_LEN+8)


//定义存放NAME&TK地址
#define   NAME_ADDR						0x30000
//无效标示
#define		CONTENT_INVALIOD			0xFFFFFFFF
//支持的最大绑定个数
#define		MAX_BONDINGS						8
//定义A7是进入低功耗还是控制电源
#define		SUPPORT_A7_POWERDOWN		0 
//keepalive定义
#define	KEEPALIVE_PROCESSING				0x01
#define	KEEPALIVE_TUPNEEDED					0x02

//定义存放绑定信息FLASH地址和偏移，第一个record为name&tk
#define		SYSBASE_ADDR			0x7E000	
#define		BONDING_SIZE			140

//版本号定义
#define		VERSION_H				0x21
#define		VERSION_L				0x02
//工作模式值定义
#define		SHORT_POWERON									0x01
#define		LONG_POWERON									0x02
//
#define		BLENAMELEN			7
#define		BLETKLEN				6
#define		NAMETK_LEN			18

//按键状态定义
#define	KEY_IDLE											0
#define	KEY_NEED											1
#define	KEY_DOWN											2
#define	KEY_UP												4

#define ERR_NONE  		0
#define ERR_INVALID_CMD  	1
#define ERR_INVALID_PAR  	2
#define ERR_INVALID_LEN  	3
#define ERR_INVALID_SEQ  	4
#define ERR_MSG_TIMEOUT  	5
#define ERR_OTHER  		127

#define	CMD_MSG_FIDO										0x83	
#define	CMD_PING_FIDO										0x81  		
#define	CMD_ERROR_FIDO									0xBF  

#define		MAX_RECEIVE_LEN								1536
#define		COUNTER_HARD_POWEROFF					120  //60s
#define		COUNTER_TIMER									50U  // 500ms

//LCD display mode (INIT\BALANCE\VERSION)
#define COUNT_DIS		0x03
#define DIS_INIT		0x00
#define DIS_BALANCE		0x01
#define DIS_VERSION		0x02
#define DIS_PAIR		0x03
#define DIS_DEFAULT		0xFF

//Main status
#define MAIN_STATUS_INIT		0x00
#define MAIN_STATUS_BAT_CHANGE	0x01
#define MAIN_STATUS_DOWNLOAD	0x02
#define MAIN_STATUS_BLEKEY		0x03
#define MAIN_STATUS_PAIRING		0x04
#define MAIN_STATUS_FIRMWARE	0x05

//KEY press 
#define KEY_PRESS_SHORT		0x01
#define KEY_PRESS_LONG		0x02
#define PUT_UP				GPIO_HIGH
#define PRESS_DOWN			GPIO_LOW
#define HAS_PRESS			0x01
#define HAS_DIS				0x02

//BLE CONN
#define BLE_DEFAULT_FLAG				0
#define BLE_CONNECT_FLAG 			1
#define BlE_DISCONNECT_FLAG			2

//BLE recerve and send
#define RCV_INIT						0
#define RCV_FINISH						1


//UART
#define OPT_UART_FLAG			1
#define DATA_UART_FLAG			2

//BAT
#define	NO_UPDATE				0
#define TO_UPDATE				1

#define INIT_CHARGE				0
#define MV_CHARGE 				1
#define YES_CHARGE				2

//涔嬪悗瑕佹浛鎹㈡垚鍒殑
#define KEY_NO    0
#define KEY_1     2
#define KEY_2     1
#define KEY_3     4
#define KEY_4     8
#define KEY_Left  5 
#define KEY_Right 6 
#define KEY_LongTouch     7
//涔嬪悗瑕佹浛鎹㈡垚鍒殑鍙橀噺
extern unsigned char touch_key;
extern unsigned char touch_key_flag;
extern unsigned char KeyWorkflag;                          


//define in main.c 
//#define APP_TIMER_PRESCALER             0                                           /**< Value of the RTC1 PRESCALER register. */
//#define APP_TIMER_OP_QUEUE_SIZE         4                                           /**< Size of timer operation queues. */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(7.5, UNIT_1_25_MS)             /**< Minimum acceptable connection interval (20 ms), Connection interval uses 1.25 ms units. */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(20, UNIT_1_25_MS)             /**< Maximum acceptable connection interval (75 ms), Connection interval uses 1.25 ms units. */
#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds), Supervision Timeout uses 10 ms units. */
#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)  /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(10000) /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */


//状态定义
typedef enum        //运行状态
{ 
    Ble_Disconnect=0,         //未连接待机
	  Ble_ConnectWaitReceive,		 //连接状态等待BLE数据
	  Ble_SendToCos,  //发送给COS数据阶段
		Ble_ReceiveFromCos,       //接收KEY的数据
    Ble_ResponseToApp,         //上报KEY返回结果	

}BLE_status;

typedef  struct
{
    uint16_t  SendCnt;           // 要发送的字节数
    uint16_t  RcvCnt;            // 接收的最大字节数（当RcvTemp=MOS_RCNT_US时）
    uint8_t   SendBuff[MAX_RECEIVE_LEN];    //发送缓存
		uint8_t   RcvBuff[MAX_RECEIVE_LEN];    //发送缓存
		uint8_t 	s_ucPacketConut;   //下行分包个数
		uint8_t 	s_ucErrorFlag;     //通讯错误返回8300026700
		uint8_t 	s_ucBackUpCmd;     //备份当前命令
}StructBle;

typedef struct
{
	unsigned char coin_name[32];
	unsigned char balance[10];
	unsigned char balance_after_di[10];
	unsigned char year[2];
	unsigned char  month;
	unsigned char day;
	unsigned char hour;
	unsigned char minute;
	unsigned char second;
	unsigned char Nousebuf[5];
}coin_Attr;

//bit7  bit6  bit5  bit4  bit3  bit2  bit1  bit0
// |     |     |     |     |     |     |     |
// |     |     |     |     |     |     |      ------停止广播完成 
// |     |     |     |     |     |      ----A7是否进入低功耗
// |     |     |     |     |      ----是否发生更新连接参数
// |     |     |     |      ----是否有USB插入
// |     |     |      ----蓝牙是否连接
// |     |      ----当前是否在绑定流程中
// |      ----是否需要关机
//  ----按键是否有效
extern uint8_t	g_ucBitFlag;
#define	ADV_STOP_ENABLE()			(g_ucBitFlag |= 0x01)			
#define	ADV_STOP_READY()			(g_ucBitFlag&0x01)
#define	ADV_STOP_CLEAR()			(g_ucBitFlag &= 0xfe)
#define	A7_IDLE_ENABLE()			(g_ucBitFlag |= 0x02)			
#define	A7_IDLE_READY()			(g_ucBitFlag&0x02)
#define	A7_IDLE_CLEAR()			(g_ucBitFlag &= 0xFD)
#define	UPDATE_ENABLE()			(g_ucBitFlag |= 0x04)			
#define	UPDATE_READY()			(g_ucBitFlag&0x04)
#define	UPDATE_CLEAR()			(g_ucBitFlag &= 0xFB)
#define	USBINSERT_ENABLE()			(g_ucBitFlag |= 0x08)			
#define	USBINSERT_READY()			(g_ucBitFlag&0x08)
#define	USBINSERT_CLEAR()			(g_ucBitFlag &= 0xF7)
#define	BLELINK_ENABLE()			(g_ucBitFlag |= 0x10)			
#define	BLELINK_READY()			(g_ucBitFlag&0x10)
#define	BLELINK_CLEAR()			(g_ucBitFlag &= 0xEF)
#define	BONDING_ENABLE()			(g_ucBitFlag |= 0x20)			
#define	BONDING_READY()			(g_ucBitFlag&0x20)
#define	BONDING_CLEAR()			(g_ucBitFlag &= 0xDF)
#define	POWEROFF_ENABLE()			(g_ucBitFlag |= 0x40)			
#define	POWEROFF_READY()			(g_ucBitFlag&0x40)
#define	POWEROFF_CLEAR()			(g_ucBitFlag &= 0xBF)
#define	KEY_VALID_ENABLE()			(g_ucBitFlag |= 0x80)			
#define	KEY_VALID_READY()			(g_ucBitFlag&0x80)
#define	KEY_VALID_CLEAR()			(g_ucBitFlag &= 0x7F)
//bit7  bit6  bit5  bit4  bit3  bit2  bit1  bit0
// |     |     |     |     |     |     |     |
// |     |     |     |     |     |     |      ------蓝灯闪烁
// |     |     |     |     |     |      ----绿灯闪烁
// |     |     |     |     |      ----红灯闪烁
// |     |     |     |      ----绿灯常亮
// |     |     |      ----
// |     |      ----
// |      ----
//  ----
#define	LED_BLUELINK					0x01
#define	LED_GREENLINK					0x02
#define	LED_REDLINK						0x04
#define	LED_GREENON						0x08
extern StructBle  UsartSta0;
extern uint16_t	g_uiSwitchCounter ; 
extern uint16_t  g_uiRecvOverTimeCounter; 
extern uint8_t  g_timeout_shutdown;

extern uint16_t g_usRecLen ;
extern uint16_t g_ucBleHadSendLen;
//extern uint8_t g_ucTxCompleteFlag ;
extern uint16_t g_usMTUSize ;
extern uint8_t g_ucPacketSeq ;  
extern uint8_t g_ucBleRevingFlag ;
//extern uint8_t g_ucTxPacketCounter; 
extern uint8_t g_ucAdvertisingMode; 
extern uint8_t g_ucPowerOnMode;
extern uint8_t g_uiKey1ValTimers ;  
extern uint8_t g_ucKey1bordFlag;	
extern uint8_t g_ucKey1bordStatus;
extern uint8_t g_ucBtName[BLENAMELEN+6];
extern uint8_t g_ucBleTK[BLETKLEN] ; 
extern uint8_t g_transfer_buff[512];
extern uint8_t g_ucCurrentVccLevel;
extern uint8_t g_apdu[APDU_MAX_LEN];
extern uint16_t g_apdu_length;
extern uint8_t g_FlagApdufinsh;
extern uint8_t g_UartFlag;
extern uint8_t g_ChargeFlag;
extern uint16_t g_BatLevel;
extern uint8_t g_Key1Status;
extern char DEVICE_NAME[20];
extern uint32_t g_flashbuff[64];
extern uint16_t		buf_size;
extern volatile uint8_t write_flag;
extern coin_Attr coinbalance;


OTHER_TIMER_ID(m_50ms_timer_id);												/*data process*/
OTHER_TIMER_ID(m_1second_timer_id);												/*data process*/
OTHER_TIMER_ID(m_shutdown_timer_id);
OTHER_TIMER_ID(m_sec_req_timer_id);
OTHER_TIMER_ID(m_battery_timer_id);  											/**< Battery measurement timer. */  
OTHER_TIMER_ID(balance_id);
OTHER_TIMER_ID(Timeout1Sec_Uart_id);
OTHER_TIMER_ID(Timeout3Sec_id);
OTHER_TIMER_ID(ADCworkID);
OTHER_TIMER_ID(Timeout1Sec_f_id);
OTHER_TIMER_ID(Timeout2ms_blekey_id);
OTHER_TIMER_ID(Motor_id);
OTHER_TIMER_ID(chargestatus_time_id);
OTHER_TIMER_ID(wallClockID);


extern uint32_t start_req_timer(void);
extern uint32_t stop_req_timer(void);

void vSYS_UpdateConnPrarmter(void);
void uiSYS_ReadNameTKRecord(void);
void vSYS_APPTimers_Start(void);
void system_init(void);
void vSYS_ConnectedInit(void);
void vSYS_ReturnErrorCode(uint8_t ucErrorCode);
void vSYS_JuageNeedKey( void );
bool bSYS_FidoToApdu(uint8_t *pData,uint16_t *len);
void vSYS_ReturnKeepAlive(uint8_t usStstus);
void vSYS_WriteUICR( uint32_t *pBuf,uint8_t ucDwordLen,uint8_t ucOffset);

extern uint32_t uiSRAM_UcharToDword(uint8_t * pucSrc);

#endif /* _SYS_H_ */
