#include <string.h>
#include "timedis.h"
#include "ble_bithd.h"
#include "ADC_BITHD.h"
#include "IO_CTL.h"
#include "sys.h"
#include "uart_bithd.h"
#include "Timer_Interrupt_Function.h"
#include "Bluetooth_APPprotocol.h"
#include "program.h"

unsigned char	Time_stuts=TimeDataDisPl; //time/balance function select
unsigned char stm32timer[9];              //save time cmd data
unsigned char flagtimerstm32=0;
unsigned char flagbalancestm32=0;

//debug test
UTCTime SecondCountRTC=0;

/********************************************
get the connect state of bluetooth
********************************************/
void BluToDis(void)
{
	if(Ble_ConnnectFlag==BLE_Disconnect)
	{
		stm32timer[7]=stm32timer[7]&0xf0;//bluetooth disconnect 
	}
	else
	{
		stm32timer[7]=stm32timer[7]|0x01;//bluetooth connectted
	}                                
}

/*******************************************
get time and date datas to save
*******************************************/
void TimDataToDis(void)
{

}

void Bat(void)
{
	uint8_t batlvl=0;
	
	if(adc_sample<Lowest_Voltage){batlvl=0;}
	else
	{
		if(adc_sample<Full_Voltage){batlvl=(adc_sample-Lowest_Voltage)*100/(Full_Voltage-Lowest_Voltage);}//get battery level
		else{batlvl=100;}
	}
	stm32timer[8]=batlvl;                                                                                 //save battery level
	
	stm32timer[7]=stm32timer[7]&0x0f;
	if(USB_connectFLag==USBconnect)                                                                       //judge usb connect
	{                                                                                                     //usb connecting 
		if(FullVoltage_flag==1){stm32timer[7]=stm32timer[7]|0x20;}                                        //full voltage
		else{stm32timer[7]=stm32timer[7]|0x10;}                                                           //power is not full
	}
	else{}                                                                                                //usb not connect
}

void f_BalanceDisplay(void)
{	
	if(touch_key==KEY_1)                         	                                                     //judge the button touch
	{                                                                                                    //have button touch
		if(Ack_recive_enable==0)
		{
			app_timer_stop(balance_id);                                                                  //stop time for Cycle to send
			flagbalancestm32=0;		
			app_timer_stop(Timeout3Sec_id);                                                              //stop time for Shutdown Timeouts
			Timeout3Sec_StarFlag=TimeClose;			
			touch_key=KEY_NO;                                                                            //clear flag
			KeyWorkflag=1;                                                                               //button touch have been dell			
			memcpy(&g_apdu[stm32uartBUFstar],&DEVICE_NAME[5],12);
			g_apdu[stm32uartBUFstar+12]=version_0;
			g_apdu[stm32uartBUFstar+13]=version_1;
			g_apdu[stm32uartBUFstar+14]=version_2;
			CmdSendUart(BlueNameCmd,&g_apdu[stm32uartBUFstar],15);		                                 //send bluetooth name
			Time_stuts=BrodcastnameDisplay;					
	 	}
	}
	else
		{
			if(flagbalancestm32==0)
			{
				flagbalancestm32=1;
				app_timer_start(balance_id, bletimeout_INTERVAL, NULL);    
			}
		}
}


void f_TimeDataDisPl(void)
{
	BluToDis();                                       //get the bluetooth state
	Bat();                                            //get battery level
	TimDataToDis();                                   //get time update the display buffer
	
	if(touch_key==KEY_1)                              //judge the button touch
	{ 
		if(Ack_recive_enable==0)
		{
			#if 1
			app_timer_stop(balance_id);                                                                  //stop time for Cycle to send
			flagbalancestm32=0;		
			app_timer_stop(Timeout3Sec_id);                                                              //stop time for Shutdown Timeouts
			Timeout3Sec_StarFlag=TimeClose;			
			touch_key=KEY_NO;                                                                            //clear flag
			KeyWorkflag=1;                                                                               //button touch have been dell			
			memcpy(&g_apdu[stm32uartBUFstar],&DEVICE_NAME[5],12);
			g_apdu[stm32uartBUFstar+12]=version_0;
			g_apdu[stm32uartBUFstar+13]=version_1;
			g_apdu[stm32uartBUFstar+14]=version_2;
			CmdSendUart(BlueNameCmd,&g_apdu[stm32uartBUFstar],15);		                                 //send bluetooth name
			Time_stuts=BrodcastnameDisplay;			
			#else
			app_timer_stop(Motor_id);                 //stop time for Cycle to send
			flagtimerstm32=0;
			app_timer_stop(Timeout3Sec_id);           //stop time for Shutdown Timeouts
			Timeout3Sec_StarFlag=TimeClose;
			touch_key=KEY_NO;                         //clear flag
			KeyWorkflag=1;                            //button touch have been dell
			Time_stuts=BalanceDisplay;                //State change to balance display
			g_apdu[stm32uartBUFstar]=balancestm32;    //Send cmd to enter balance display
			CmdSendUart(Changestatuscmd_uart,&g_apdu[stm32uartBUFstar],1);
			#endif	
		}
	}
	else
	{
		if(flagtimerstm32==0)
		{
			flagtimerstm32=1;
		  app_timer_start(Motor_id, bletimeout_INTERVAL, NULL);    
		}

	}
		 

	
}


void f_BrodcastnameDisplay(void)
{
	if(touch_key==KEY_1)                        
	{   
		if(Ack_recive_enable==0)
		{

		app_timer_stop(Timeout3Sec_id);                                     //stop time for Shutdown Timeouts
		Timeout3Sec_StarFlag=TimeClose;
		touch_key=KEY_NO;                     
		KeyWorkflag=1;                           
		g_apdu[stm32uartBUFstar]=timerstm32;                                //Send cmd to enter Time display
		CmdSendUart(Changestatuscmd_uart,&g_apdu[stm32uartBUFstar],1);		
		Time_stuts=TimeDataDisPl;					
		}
	}
	else{}
}


void timedisplay(void * p_event_data, uint16_t event_size)
{
	if(Timeout3Sec_StarFlag!=TimeClose)                                     //judge the timeout timer have been turn on?
	{                                                                       //timer is open
		if(Timeout3Sec_StarFlag==TimeOut)
		{                                                                  //timeout
			app_timer_stop(Timeout3Sec_id);                                 //stop timer
			Timeout3Sec_StarFlag=TimeClose;                                 //clear flag
			Time_stuts=TimeDataDisPl;
			
			Main_status=Main_status_closeoled;                              //states chang to shutdown mode
			PowerOff_key();                                                 //stm32 power off
		}
		else{}
	}
	else
	{
		if(Time_stuts==TimeDataDisPl){app_timer_start(Timeout3Sec_id,T_5SECOND_TimeOutremind, NULL);}
		else{app_timer_start(Timeout3Sec_id,T_60SECOND_TimeOutremind, NULL);}
		Timeout3Sec_StarFlag=TimeOpen;                          
	}

	switch(Time_stuts)
	{
		case TimeDataDisPl: 	   f_TimeDataDisPl();
		break;
		case BalanceDisplay:       f_BalanceDisplay();
		break;
		case BrodcastnameDisplay:  f_BrodcastnameDisplay();
		break;
		default:
		break;
	}

	if(Main_status!=Main_status_timedisplay)
	{
		app_timer_stop(Timeout3Sec_id);   
		Timeout3Sec_StarFlag=TimeClose;  
		Time_stuts=TimeDataDisPl;
	}

}

unsigned char pstorage_times=0;
#include "pstorage.h"
#include "flashmcu_bithd.h"

void update_wall_clock(void * p_event_data, uint16_t event_size)
{
//    unsigned char buf[16]={0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8};	
    SecondCountRTC++;                           
		 
    if(Main_status==Main_status_closeoled)
		{
			  pstorage_times++;
				if(pstorage_times<60)
				{}
				else
				{
					#if 0
					pstorage_times=0;
					//Save time data to flash
					pstorage_block_identifier_get(&base_handle,64, &USEblock_handle);			
					pstorage_wait_flag=1;
					pstorage_clear(&USEblock_handle, 16);
					while(pstorage_wait_flag==1);
					pstorage_wait_flag=1;
					memcpy(buf,(unsigned char*)(&SecondCountRTC),4);
					pstorage_store(&USEblock_handle,buf,16,0);
					while(pstorage_wait_flag==1);
					#endif
				}
		}
}

