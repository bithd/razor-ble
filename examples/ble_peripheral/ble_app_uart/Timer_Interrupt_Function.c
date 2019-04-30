#include <string.h>
#include "Timer_interrupt_Function.h"
#include "bluetooth_HWprotocol.h"
#include "sys.h"
#include "timedis.h"
#include "program.h"
#include "uart_bithd.h"
#include "ADC_BITHD.h"
#include "app_scheduler.h"

//app_timer_id_t                 m_sec_req_timer_id;
//app_timer_id_t                 m_app_timer_id;
//app_timer_id_t                 KeepTouch_timer_id;              
unsigned char                  Touch_timercount=0;               
//app_timer_id_t                 Scan_Touch_timer_id;            
//app_timer_id_t                 wallClockID;     
//app_timer_id_t                 ADCworkID;    

unsigned char                  Timeout3Sec_StarFlag=TimeClose;    

//app_timer_id_t                 Timeout1Sec_f_id;                  
unsigned char                  Timeout1Sec_f_StarFlag=TimeClose;  

//app_timer_id_t                 Timeout2ms_blekey_id;           

//app_timer_id_t                 Motor_id;

//app_timer_id_t                 balance_id;

//app_timer_id_t                 Timeout1Sec_Uart_id;                  
unsigned char                  Timeout1Sec_Uart_StarFlag=TimeClose;  

//app_timer_id_t                 chargestatus_time_id;
unsigned char                  chargstatustime_flag=0;

void Balance_handler(void * p_context)
{
	if((flagbalancestm32==1)&&(Main_status==Main_status_timedisplay)&&(Time_stuts==BalanceDisplay))
	{
		if(Ack_recive_enable==0){
		memcpy(&g_apdu[stm32uartBUFstar],&coinbalance,balnace_usefsize);	
    CmdSendUart(Balancecmd_uart,&g_apdu[stm32uartBUFstar],balnace_usefsize);
	}
	}
	else
	{
		app_timer_stop(balance_id);
		flagbalancestm32=0;
	}

}

void Ble_timeout_handler(void * p_context)
{
	timer0_sec_count++; 
}

unsigned char clock=0;
void TimerClock_handler(void * p_context)
{
	clock++;
	if(clock>9)
	{
		app_sched_event_put(NULL,0, update_wall_clock);
   	clock=0;
	}
}

void ADCwork_handler(void * p_context)
{
	app_sched_event_put(NULL,0, ReadAdc); 
}

void TimeOutMain_handler(void * p_context)
{
	//Timeout3Sec_StarFlag=TimeOut;
}

void TimeOut1sec_handler(void * p_context)
{
	Timeout1Sec_f_StarFlag=TimeOut;
}
void TimeOutUart_handler(void * p_context)
{
	Timeout1Sec_Uart_StarFlag=TimeOut;
}

void Motor_timeout_handler(void * p_context)
{
	
	if((flagtimerstm32==1)&&(Main_status==Main_status_timedisplay))
	{
		if(Ack_recive_enable==0){
		memcpy(&g_apdu[stm32uartBUFstar],stm32timer,sizeof(stm32timer));
  		CmdSendUart(EnterTimercmd_uart,&g_apdu[stm32uartBUFstar],sizeof(stm32timer));
	}
	}
	else
	{
		app_timer_stop(Motor_id);
		flagtimerstm32=0;
	}
}


#define NoCharg  0
#define Charging 1
#define Charged  2
#define _1200ms_ 1200
#define _3500ms_ 3500
unsigned char chargflag=NoCharg;
unsigned char H_count=0;
unsigned char L_count=0;
void Timechargestatus_handler(void * p_context)
{

  USB_ChangingFLAG=USBnoChanging;
  switch(chargflag){
		case (NoCharg):
			if(nrf_gpio_pin_read(CHARG_STA_PIN)==0)
			{
				USB_ChangingFLAG=USBChanged;
				H_count++;
				USB_connectFLag=USBconnect;
				chargflag=Charging;
			}
			else
			{
				app_timer_stop(chargestatus_time_id);
				chargstatustime_flag=0;
				H_count=0;
				L_count=0;
				USB_connectFLag=USBDisconnect;
			}
			break;
		case (Charging):
				if(nrf_gpio_pin_read(CHARG_STA_PIN)==0)
				{
					H_count++;
					L_count=0;
					USB_ChangingFLAG=USBnoChanging;
					if(H_count>(_1200ms_/100))
					{
						chargflag=Charged;
						H_count=0;
					}
				}
				else
				{
					H_count=0;
					L_count++;
					if(L_count>(_1200ms_/100))
					{
						chargflag=NoCharg;
						L_count=0;
						app_timer_stop(chargestatus_time_id);
						chargstatustime_flag=0;
						USB_connectFLag=USBDisconnect;
					}
				}
			break;
		case (Charged):
			if(nrf_gpio_pin_read(CHARG_STA_PIN)==1)
				{
					H_count=0;
					L_count++;
					if(L_count>(_3500ms_/100))
					{
            chargflag=NoCharg;
						L_count=0;
						app_timer_stop(chargestatus_time_id);
						chargstatustime_flag=0;
						USB_connectFLag=USBDisconnect;
					}
				}
				else
				{
					L_count=0;
					H_count=0;
				}
			break;
   }
}


