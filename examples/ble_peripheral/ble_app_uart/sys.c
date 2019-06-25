//----------------------------------------------------------------------
//      File: sys.C
//----------------------------------------------------------------------
//      Update History: (mm/dd/yyyy)
//      11/02/2016 - V1.00 - IQ.CHEN :  First official release
//----------------------------------------------------------------------
//      Description:
//
//      ��ģ������һЩȫ�ֱ��������ͨ�ú���
//----------------------------------------------------------------------
#include "sys.h"
//#include <string.h>
#include "nordic_common.h"
#include "sdk_common.h"
#include "ble_nus.h"
#include "app_timer.h"
#include "pca10040.h"
#include "boards.h"
#include "nrf_delay.h"
#include "ble_hci.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "peer_manager.h"
#include "Timer_Interrupt_Function.h"
#include "program.h"
#include "ble_bithd.h"
#include "uart_bithd.h"
#include "timedis.h"
#include "flashmcu_bithd.h"

///////////////////////////////////
uint8_t  g_timeout_shutdown = SHUTDOWN_TIME;



////////////ȫ�ֱ�������///////////////////////////////////////////////////
uint8_t		g_ucBitFlag;   
StructBle  UsartSta0;      							// �ṹ���������
uint16_t	g_uiSwitchCounter ; 					// 5�볤����ת��ģʽ
uint16_t  g_uiRecvOverTimeCounter;  		// �������ݳ�ʱ

uint16_t g_usRecLen ;            //�Ѿ��������ݳ���
uint16_t g_ucBleHadSendLen;
uint16_t g_usMTUSize = 20;   // MTU-3��ֵ
//uint8_t g_ucTxCompleteFlag ;  //������ɵı�־

uint8_t g_ucPacketSeq ;     //���հ������
uint8_t g_ucBleRevingFlag ; //
uint8_t g_ucBleLinkFlag;   		//�����Ƿ�����
//uint8_t g_ucTxPacketCounter;   		//���Է������ݰ�����
uint8_t g_ucAdvertisingMode;   		//�㲥ģʽ
uint8_t g_ucPowerOnMode;					//����ģʽ
uint8_t   g_uiKey1ValTimers ;  // ������Чʱ��
uint8_t		g_ucKey1bordFlag ;		// ���ܰ�����־;
uint8_t		g_ucKey1bordStatus;  //����״̬
uint8_t g_ucBtName[BLENAMELEN+6] = "BITHDABCDEF" ;  // ���������㲥����
uint8_t g_ucBleTK[BLETKLEN] = "123456" ; 
char DEVICE_NAME[20]="RAZOR";//"bithd001122334455";

static uint8_t 	s_uaTempSendBuf[16];
uint8_t g_transfer_buff[512];

uint8_t g_ucCurrentVccLevel = 0;
uint8_t g_DisProcessFlag = 0;
uint8_t g_Key1Status = PUT_UP;
uint8_t g_Key2Status = PRESS_DOWN;
uint8_t g_MainStatus = MAIN_STATUS_INIT;

uint8_t g_apdu[APDU_MAX_LEN];
uint16_t g_apdu_length;
uint8_t g_FlagApdufinsh=RCV_INIT;
uint8_t g_UartFlag=0;
uint8_t g_ChargeFlag = INIT_CHARGE;
uint16_t g_BatLevel = 0;
uint16_t g_backBatLevel = 0;
uint32_t g_flashbuff[64];
uint16_t		buf_size=64;
volatile uint8_t write_flag=0;
coin_Attr coinbalance={0};
uint16_t g_bak_AdcSample=0;

//之后要替换成别的变量
unsigned char touch_key      =KEY_NO;                 
unsigned char touch_key_flag =0;                      
unsigned char KeyWorkflag=0;                          
//static unsigned char KeepTouch_timer_flag=0;      

APP_TIMER_DEF(m_50ms_timer_id);												/*data process*/
APP_TIMER_DEF(m_1second_timer_id);												/*data process*/
APP_TIMER_DEF(m_shutdown_timer_id);
APP_TIMER_DEF(m_sec_req_timer_id);
APP_TIMER_DEF(m_battery_timer_id);  											/**< Battery measurement timer. */  
APP_TIMER_DEF(Timeout3Sec_id);
APP_TIMER_DEF(balance_id);
APP_TIMER_DEF(Timeout1Sec_f_id);
APP_TIMER_DEF(Timeout2ms_blekey_id);
APP_TIMER_DEF(Motor_id);
APP_TIMER_DEF(Timeout1Sec_Uart_id);
APP_TIMER_DEF(chargestatus_time_id);
APP_TIMER_DEF(wallClockID);



/*****************************************************************************
 ����:  vSYS_Timer1SHandler
 ����:	p_context
 ���:	��
 		
 ����:	��
 		
 ����:
			500ms��ʱ���жϺ������ڸ��ּ�ʱ���ܣ���Ϊkeepalive��500ms
******************************************************************************/
extern uint16_t                         m_conn_handle ;
extern void advertising_init(void);
/*****************************************************************************
 ����:  vSYS_Timer50MSHandler
 ����:	p_context
 ���:	��
 		
 ����:	��
 		
 ����:
			50ms��ʱ���жϺ�������led��˸
******************************************************************************/
static void vSYS_Timer50MSHandler(void * p_context)
{
	static uint8_t s_LongPressCount = 0;
	
	UNUSED_PARAMETER(p_context);	
	
	//Power key--key1
	if(g_Key1Status == PUT_UP)
	{
		if(GET_PWKEY_STATUS() == GPIO_HIGH)
		{			
			g_timeout_shutdown = SHUTDOWN_TIME;
			g_Key1Status = PRESS_DOWN;
		}
	}
	else
	{			
		if(GET_PWKEY_STATUS() == GPIO_LOW)
		{	
			if((Main_status_download==Main_status)||(Main_status_blekey==Main_status))
			{}
			else
			{
				//if(touch_key_flag==1)
				{
					//if(0==KeyWorkflag)
					{
						touch_key = KEY_1;
						g_Key1Status = PUT_UP;
						s_LongPressCount = 0;     //Release reset 0						
					}
				}
			}	
			//touch_key_flag=0x00;
			//KeyWorkflag=0;
		}
	}

	//long press
	if(GET_PWKEY_STATUS() == GPIO_HIGH)
	{
		s_LongPressCount++;
		if(s_LongPressCount >= 30)
		{
			s_LongPressCount = 0;
			if(g_ChargeFlag == INIT_CHARGE)
			{
				STM_POWER_OFF(); 
				BT_POWER_OFF();	
				while(1);
			}						
		}		
	}
	
	//key2
	if(g_Key2Status == PUT_UP)
	{
		if(GET_KEY2_STATUS() == GPIO_LOW)
		{			
			g_timeout_shutdown = SHUTDOWN_TIME;
			g_Key2Status = PRESS_DOWN;
			
		}
	}
	else
	{	
			
		if(GET_KEY2_STATUS() == GPIO_HIGH)
		{
			if((Main_status_download==Main_status)||(Main_status_blekey==Main_status))
			{}
			else
			{
				//if(touch_key_flag==1)
				{
					//if(0==KeyWorkflag)
					{
						touch_key = KEY_1;
						g_Key2Status = PUT_UP;
						s_LongPressCount = 0;     //Release reset 0						
					}
				}
			}	
			//touch_key_flag=0x00;
			//KeyWorkflag=0;
			
			//touch_key = KEY_1;
			//g_Key2Status = PUT_UP;
		}
	}

	//CHARGE
	if(GET_USB_INSERT() == GPIO_LOW)
	{
		g_ChargeFlag = YES_CHARGE;
		g_timeout_shutdown = SHUTDOWN_TIME;
	}
	else
	{
		g_ChargeFlag = INIT_CHARGE;
	}
	
}

static void vSYS_Timer1SHandler(void * p_context)
{
//	uint32_t err_code ;
//	ble_gap_conn_sec_mode_t sec_mode; 
	
	UNUSED_PARAMETER(p_context);
	
	if(ParingDataDisFlag==1)
	{
		if(Ack_recive_enable==0)
		{	
			if((Main_status!=Main_status_download)&&(Main_status!=Main_status_blekey))
			{
				if(poweronkey_flag==0)
				{
					firmwaredownload_GPIO_H();                      //enter APP mode
					nrf_gpio_pin_clear(SlectPin);
					PowerOn_key();
				}
				memcpy(&g_apdu[stm32uartBUFstar],ParingDataBuf,sizeof(ParingDataBuf));
				CmdSendUart(BlueParingDatacmd,&g_apdu[stm32uartBUFstar],sizeof(ParingDataBuf));//paring datas display
				
				app_timer_stop(Timeout3Sec_id);    
				Timeout3Sec_StarFlag=TimeClose;   
				app_timer_stop(Timeout1Sec_f_id);   
				Timeout1Sec_f_StarFlag=TimeClose;
				Time_stuts=TimeDataDisPl;
				Main_status=Main_status_ParingDis;
			}
			ParingDataDisFlag=0;
	 	}
	}
#if 0
	g_uiHardPowerDownCounter++ ;
	g_uiOPPowerDownCounter++;
	if (g_ucBleRevingFlag==0x01)
	{
		if(g_uiRecvOverTimeCounter++ > 5)
		{
			UsartSta0.s_ucErrorFlag = ERR_MSG_TIMEOUT ;
		}
	}
	if (g_uiHardPowerDownCounter > COUNTER_HARD_POWEROFF) // 60s hard poweroff
	{
		POWEROFF_ENABLE();
	}	
	if (g_ucKey1bordStatus == KEY_NEED) //�ȴ����ܰ���
	{			
			if ((g_uiKey1ValTimers == 0x00)||(g_ucKey1bordFlag == 0x00))
			{
				vSYS_ReturnKeepAlive(KEEPALIVE_TUPNEEDED);		
				g_uiOPPowerDownCounter = 0 ; //�ȴ��������ݵĹ������������
			}
			else
			{
				vSYS_ReturnKeepAlive(KEEPALIVE_PROCESSING);	
			}
	}
	if (g_ucPowerOnMode == LONG_POWERON)  //pairs mode is 60s
	{
			if (g_uiOPPowerDownCounter > 120) //60s
			{
				POWEROFF_ENABLE();
			}			
	}
	else                                  // no pairs 
	{
		if (BLELINK_READY()!=0x00)  //connect is 35s
		{	
			if (g_uiOPPowerDownCounter > 70) 
			{
				POWEROFF_ENABLE();
			}			
		}
		else //disconnect is 15s
		{
			if (g_uiOPPowerDownCounter > 30) //15s
			{
				POWEROFF_ENABLE();
			}	
		}
	}
	if (g_ucKey1bordFlag & 0x03) //button press down
	{
		g_uiSwitchCounter++ ;
		g_uiKey1ValTimers++;
		if (g_uiKey1ValTimers > 120)  // 60s����Ч�ڵ���
		{
			g_uiKey1ValTimers = 0 ;
			g_ucKey1bordFlag = 0 ;

		}		
		if (g_uiSwitchCounter > 9)   //��5s��
		{
			g_uiSwitchCounter = 0 ;
			if (g_ucPowerOnMode == SHORT_POWERON)
			{
				g_ucPowerOnMode = LONG_POWERON ;
				g_ucAdvertisingMode = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE ;
				if(BLELINK_READY()!=0x00)  //disconnect if connect
				{
					err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
					APP_ERROR_CHECK(err_code);
				}
				sd_ble_gap_adv_stop();  //stop adv first 
				BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
				err_code = sd_ble_gap_device_name_set(&sec_mode,&g_ucBtName[6],BLENAMELEN);	//set name again		
				APP_ERROR_CHECK(err_code);
				advertising_init();  //init variable
				err_code = ble_advertising_start(BLE_ADV_MODE_FAST); //adv again
				APP_ERROR_CHECK(err_code);
			}
		}
		
	}
#endif
}

static void timer_autoshutdown_handler(void * p_context)
{
	UNUSED_PARAMETER(p_context);

	if(g_timeout_shutdown >1)
	{
		g_timeout_shutdown--;
	}
	else
	{
		STM_POWER_OFF(); 
		BT_POWER_OFF();	
		while(1);		
	}
}
/**@brief Function for handling the Security Request timer timeout.
 *
 * @details This function will be called each time the Security Request timer expires.
 *
 * @param[in] p_context  Pointer used for passing some arbitrary information (context) from the
 *                       app_start_timer() call to the timeout handler.
 */
static void sec_req_timeout_handler(void * p_context)
{
    ret_code_t err_code;

    if (m_conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        // Initiate bonding.
//        NRF_LOG_DEBUG("Start encryption\r\n");
        err_code = pm_conn_secure(m_conn_handle, false);
        if (err_code != NRF_ERROR_INVALID_STATE)
        {
            APP_ERROR_CHECK(err_code);
        }
    }
}


void vSYS_UpdateConnPrarmter(void)
{
	uint32_t err_code ;
	ble_gap_conn_params_t   gap_conn_params;
	gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
	gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL; //(75*1000) /1250 ;//
	gap_conn_params.slave_latency     = SLAVE_LATENCY;
	gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;			
	err_code = ble_conn_params_change_conn_params(&gap_conn_params);			
	APP_ERROR_CHECK(err_code);
	
}

/*****************************************************************************
 ����:  vSYS_CheckWorkMode
 ����:	��
 ���:	��
 		
 ����:	��
 		

����:
		����Ƿ���������ģʽ,�ϵ�����
******************************************************************************/
void vSYS_CheckWorkMode(void)
{	
	//usb������
	if(GPIO_HIGH == GET_USB_INSERT())
	{
		while(1)
		{
			if (GET_BATTERY_FULL()== GPIO_HIGH)
			{
			}
			nrf_delay_ms(100);
			if (GET_USB_INSERT() == GPIO_LOW)
			{
			}			
		}
	}
	g_ucPowerOnMode = SHORT_POWERON ;	
	g_ucAdvertisingMode = BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED ;//BLE_GAP_ADV_FLAGS_LE_NO_DISC_MODE;//

	
	// ����Ҳ���ǰ�����Ч������IO3
	g_ucKey1bordFlag = 0x03; //��ʱ��Ϊ0��ԭ��Ϊ3

}


static void usr_ram_init(void)
{
	g_uiSwitchCounter = 0 ;
	g_uiRecvOverTimeCounter = 0 ;
	g_usRecLen = 0;
	g_usMTUSize = 20;
	g_ucPacketSeq = 0 ;
	g_ucBleRevingFlag = 0 ;
	g_ucBitFlag = 0 ;
	g_ucAdvertisingMode = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;//BLE_GAP_ADV_FLAGS_LE_NO_DISC_MODE; ;
	g_ucPowerOnMode = LONG_POWERON ;//SHORT_POWERON; //
	touch_key=KEY_1;
}
/*****************************************************************************
 ����:  vSYS_APPTimers_Start
 ����:	��
 ���:	��
 		
 ����:	��
 		
 ����:
			�����û���ʱ������
******************************************************************************/
void vSYS_APPTimers_Start(void)
{
	uint32_t err_code;

	err_code = app_timer_start(m_50ms_timer_id, ONE_50MS_INTERVAL, NULL);
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_start(m_1second_timer_id, ONE_SECOND_INTERVAL, NULL); 
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_start(m_shutdown_timer_id, T_SHUTDOWN_TIMEOUT, NULL); 
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_start(m_battery_timer_id, BATTERY_LEVEL2_MEAS_INTERVAL, NULL);
	APP_ERROR_CHECK(err_code);

	//
	err_code = app_timer_start(wallClockID, ONE_SECOND_INTERVAL_, NULL);    
	APP_ERROR_CHECK(err_code);

	err_code = app_timer_start(Timeout2ms_blekey_id, bletimeout_INTERVAL, NULL);   
	APP_ERROR_CHECK(err_code);
}

uint32_t start_req_timer(void)
{
		ret_code_t err_code;
	
		// Start Security Request timer.
		err_code = app_timer_start(m_sec_req_timer_id, SECURITY_REQUEST_DELAY, NULL);
		return err_code;
}
uint32_t stop_req_timer(void)
{
		ret_code_t err_code;
	
		err_code = app_timer_stop(m_sec_req_timer_id);
		return err_code;
}



static void usr_timers_init(void)
{
    uint32_t err_code;

    // Initialize timer module.
    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

	//50ms timer
	err_code = app_timer_create(&m_50ms_timer_id,
								APP_TIMER_MODE_REPEATED,
								vSYS_Timer50MSHandler);
	APP_ERROR_CHECK(err_code);
	//1s timer
	err_code = app_timer_create(&m_1second_timer_id,
								APP_TIMER_MODE_REPEATED,
								vSYS_Timer1SHandler);
    APP_ERROR_CHECK(err_code);

	//battery timer
	err_code = app_timer_create(&m_battery_timer_id,
								APP_TIMER_MODE_REPEATED,
								battery_level_meas_timeout_handler);
    APP_ERROR_CHECK(err_code);
    
    //System Shut Down
	err_code = app_timer_create(&m_shutdown_timer_id, 
								APP_TIMER_MODE_REPEATED, 
								timer_autoshutdown_handler);  
	APP_ERROR_CHECK(err_code);
	
    // Create Security Request timer.
    err_code = app_timer_create(&m_sec_req_timer_id,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                sec_req_timeout_handler);
    APP_ERROR_CHECK(err_code);  
	//---------------------------------------------------------
	//Balance id
	err_code = app_timer_create(&balance_id,
								APP_TIMER_MODE_REPEATED,
								Balance_handler); 
	APP_ERROR_CHECK(err_code);	
	
	err_code = app_timer_create(&wallClockID, 
								APP_TIMER_MODE_REPEATED, 
								TimerClock_handler);                
    APP_ERROR_CHECK(err_code);
	
    
	err_code = app_timer_create(&Timeout3Sec_id,
								APP_TIMER_MODE_SINGLE_SHOT, 
								TimeOutMain_handler);		  
	APP_ERROR_CHECK(err_code);

	err_code = app_timer_create(&Timeout1Sec_f_id, 
								APP_TIMER_MODE_SINGLE_SHOT, 
								TimeOut1sec_handler);        
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&Timeout2ms_blekey_id, 
    							APP_TIMER_MODE_REPEATED, 
    							Ble_timeout_handler);
    APP_ERROR_CHECK(err_code); 

    err_code = app_timer_create(&Motor_id, 
    							APP_TIMER_MODE_REPEATED, 
    							Motor_timeout_handler);
    APP_ERROR_CHECK(err_code);  	
	
	err_code = app_timer_create(&Timeout1Sec_Uart_id, 
								APP_TIMER_MODE_SINGLE_SHOT, 
								TimeOutUart_handler); 	
	APP_ERROR_CHECK(err_code);
	err_code = app_timer_create(&chargestatus_time_id, 
								APP_TIMER_MODE_REPEATED, 
								Timechargestatus_handler);	   
	APP_ERROR_CHECK(err_code);

}
/*****************************************************************************
 ����:  vSYS_ConnectedInit
 ����:	��
 ���:	��
 		
 ����:	��
 		
 ����:
			�����豸�����Ӻ�ĳ�ʼ��
******************************************************************************/
void vSYS_ConnectedInit(void)
{
	g_ucBleRevingFlag=0x00;
	g_usRecLen = 0 ;
	UsartSta0.s_ucPacketConut = 0 ;
	UsartSta0.s_ucErrorFlag = 0 ;
	BLELINK_ENABLE();
}

void change_NFCPIN(void)
{
	//change NFC pin function
	 if ((NRF_UICR->NFCPINS & UICR_NFCPINS_PROTECT_Msk) == (UICR_NFCPINS_PROTECT_NFC << UICR_NFCPINS_PROTECT_Pos)){
            NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;
            while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
            NRF_UICR->NFCPINS &= ~UICR_NFCPINS_PROTECT_Msk;
            while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
            NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;
            while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
            NVIC_SystemReset();
        }
}


void system_init(void)
{		
	change_NFCPIN();
	
	usr_gpio_init();
	
	usr_ram_init();

	usr_timers_init();	
	
}
/*****************************************************************************
 ����:  bSYS_FidoToApdu
 ���룺	pData:Ҫת��������
				len:	Ҫת�����ݳ���
 ����� pData:ת����Ľ��
				len: ת���������
 ����:	false/true
 		
 ����:
			������ķ�������fido��ʽ������ת��Ϊapdu
******************************************************************************/
bool bSYS_FidoToApdu(uint8_t *pData,uint16_t *len)
{
	uint8_t seq=0;
	uint16_t packetconut,apdulen,off,tlen;
	
	tlen = *len ;
	apdulen = (pData[1]<<8) | pData[2] ;	
	packetconut = 0 ;

	if (tlen > g_usMTUSize) // ���
	{
		packetconut = (tlen-g_usMTUSize)/(g_usMTUSize) ;
		if ((tlen-g_usMTUSize)%(g_usMTUSize)) // ������������+1
		{
			packetconut++ ;
		}	
		//packetconut������������ݵİ���������Ҫ-1�ѵ�һ��ȥ������Ϊ�Ѿ�+3��
		if ((apdulen+3+packetconut) !=tlen) 
		{
			return false ;
		}
		
		off = g_usMTUSize ;
		tlen -= g_usMTUSize ;
		while(tlen > g_usMTUSize)
		{
			if ( pData[off] != seq )
			{
				return false ;
			}
			memmove(pData+off, pData+off+1, tlen-1) ;
			off += (g_usMTUSize-1) ;
			tlen -= g_usMTUSize ;
			seq++ ;
		}
		if ( pData[off] != seq )
		{
			return false ;
		}
		memmove(pData+off, pData+off+1, tlen-1) ;		
	}
	else   // ֻ��һ������
	{
		if ((apdulen+3) != tlen)
		{
			return false ;		
		}
	}	
	if (*pData != CMD_PING_FIDO)
	{
		memmove(pData, pData+3, apdulen) ;  //ȥ��ǰ3�ֽ�
	}
	*len = apdulen;
	return true;
}


 /*****************************************************************************
 ����:  vSYS_ReturnKeepAlive
 ����:	usStstus:״̬
 ���:	��
 		
 ����:	��
 		
 ����:
		ͨѶ�����Ƿ���1�ֽ�״̬
******************************************************************************/
void vSYS_ReturnKeepAlive(uint8_t usStstus)
{
	s_uaTempSendBuf[0]=0x82;
	s_uaTempSendBuf[1]=0x00;
	s_uaTempSendBuf[2]=0x01;
	s_uaTempSendBuf[3]=usStstus;
	ble_nus_string_send(&m_nus,s_uaTempSendBuf,4);	
}

/*****************************************************************************
 ����:  vSYS_ReturnErrorCode
 ����:	ucErrorCode: ������
 ���:	��
 		
 ����:	��
 		
 ����:
		����FIDOЭ���ϴ��󷵻�1���ֽڴ�����
******************************************************************************/
void vSYS_ReturnErrorCode(uint8_t ucErrorCode)
{
//	uint32_t err_code;
	
	g_usRecLen = 0 ;
	//g_ucKey1bordStatus = KEY_IDLE ;  

	s_uaTempSendBuf[0]=CMD_ERROR_FIDO;
	s_uaTempSendBuf[1]=0x00;
	s_uaTempSendBuf[2]=0x01;
	s_uaTempSendBuf[3]=ucErrorCode;
	//send data
	ble_nus_string_send(&m_nus,s_uaTempSendBuf, 4);

	//set flag
	g_uiRecvOverTimeCounter = 0 ;
	UsartSta0.s_ucErrorFlag = ERR_NONE ;
	g_ucPacketSeq=0;
	UsartSta0.s_ucPacketConut = 0 ;
	memset(UsartSta0.SendBuff,0x00,6);
}

/*****************************************************************************
 ����:  vSYS_JuageNeedKey
 ���룺	��
			
 ����� ��
		
 ����:	��
 		
 ����:
			�ж��Ƿ���Ҫ�����������Ҫled��˸��ʾ
******************************************************************************/
void vSYS_JuageNeedKey( void )
{
	if((UsartSta0.SendBuff[0]==CMD_MSG_FIDO)&&(UsartSta0.SendBuff[3]==0x00)
			&&(UsartSta0.SendBuff[4]==0x01)&&(UsartSta0.SendBuff[6]==0x00))   //ע��
	{	
		g_ucKey1bordStatus = KEY_NEED ;

	}
	else if((UsartSta0.SendBuff[0]==CMD_MSG_FIDO)&&(UsartSta0.SendBuff[3]==0x00)
					&&(UsartSta0.SendBuff[4]==0x02)&&(UsartSta0.SendBuff[5]==0x03))
	{	
		g_ucKey1bordStatus = KEY_NEED ;

	}
}

/*****************************************************************************
 ����:  vSYS_WriteUICR
���룺	pBuf:
				ucDwordLen:
				ucOffset:
 ����� ��
 ����:	��
 		
 ����:
			дUICR��ַ
******************************************************************************/
void vSYS_WriteUICR( uint32_t *pBuf,uint8_t ucDwordLen,uint8_t ucOffset)
{
	uint8_t i ;
	//enable write
	NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen << NVMC_CONFIG_WEN_Pos;
	for(i=0;i<ucDwordLen;i++)
	{
		while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}
		NRF_UICR->CUSTOMER[ucOffset+i] = pBuf[i];
//		while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}

	}
	NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren << NVMC_CONFIG_WEN_Pos;
	while (NRF_NVMC->READY == NVMC_READY_READY_Busy){}	
	NVIC_SystemReset();
}
