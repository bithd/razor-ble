#include "ADC_BITHD.h"
#include "IO_CTL.h"
#include "Timer_Interrupt_Function.h"
#include "program.h"
#include "uart_bithd.h"

unsigned char FullVoltage_flag=0;   //power is full flag
volatile int32_t adc_sample=0;      //storge batterylevel

/**************************
get states of charging
*************************/
void chargingBat(void)
{
	if(USB_connectFLag==USBconnect)
	{
		if(nrf_gpio_pin_read(CHARG_STA_PIN)==0)             //judge charing state
		{
//			FullVoltage_flag=1;                              //power is full
		}
		else{}
	}
	else
	{
		FullVoltage_flag=0;                                   //clear charging flag
	}	
}

/*
get battery voltage
*/
void ReadAdc(void * p_event_data, uint16_t event_size)
{ 	 
	adc_sample = g_BatLevel;
	app_timer_stop(m_battery_timer_id);
	app_timer_start(m_battery_timer_id, BATTERY_LEVEL_MEAS_INTERVAL, NULL);
	chargingBat();
}

/*
 * @brief ADC initialization.
 */
void adc_config(void)
{
#if 0
    const nrf_adc_config_t nrf_adc_config = NRF_ADC_CONFIG_DEFAULT;
    // Initialize and configure ADC
    nrf_adc_configure( (nrf_adc_config_t *)&nrf_adc_config);
#endif
}

///////////////////////////////////low power manager/////////////////////

void Chargingandmanagerdisplay(void * p_event_data, uint16_t event_size)
{

	if(USB_connectFLag==USBconnect)
	{
	    if(Timeout1Sec_f_StarFlag==TimeClose)   //timedelay for display
		{
			app_timer_start(Timeout1Sec_f_id, ONE_SECOND_INTERVAL, NULL); 
			Timeout1Sec_f_StarFlag=TimeOpen;
		}
        else
		{
			if(Timeout1Sec_f_StarFlag==TimeOut)
			{
				app_timer_stop(Timeout1Sec_f_id);  
				Timeout1Sec_f_StarFlag=TimeClose;

				g_apdu[stm32uartBUFstar]=timerstm32;   //stm32 entern time display
				CmdSendUart(Changestatuscmd_uart,&g_apdu[stm32uartBUFstar],1);
        KEYwork_flag=0;
				Main_status=Main_status_timedisplay;   
			}
			else
			{}//1sec no timeout
		}				
	}
	else
	{
		if(USB_ChangingFLAG==USBChanged)           //USB Just remove ?
		{                         			//just remove usb
			KEYwork_flag=0;
			Main_status=Main_status_timedisplay;   
			nrf_gpio_pin_clear(SlectPin);
			firmwaredownload_GPIO_H();             //entern APP mode
			PowerOn_key();
			g_apdu[stm32uartBUFstar]=timerstm32;
		    CmdSendUart(Changestatuscmd_uart,&g_apdu[stm32uartBUFstar],1);
	  	}
		else
		{
      		if(Timeout1Sec_f_StarFlag==TimeClose)
			{
				app_timer_start(Timeout1Sec_f_id, ADC_NOpower, NULL); 
				Timeout1Sec_f_StarFlag=TimeOpen;
			}
      		else
			{
				if(Timeout1Sec_f_StarFlag==TimeOut)
				{
					app_timer_stop(Timeout1Sec_f_id);    
					Timeout1Sec_f_StarFlag=TimeClose;

					g_apdu[stm32uartBUFstar]=timerstm32;
					CmdSendUart(Changestatuscmd_uart,&g_apdu[stm32uartBUFstar],1);
          KEYwork_flag=0;
					Main_status=Main_status_timedisplay;
				}
				else
				{
					if(touch_key!=KEY_NO)
					{
						app_timer_stop(Timeout1Sec_f_id);   
						Timeout1Sec_f_StarFlag=TimeClose;

						g_apdu[stm32uartBUFstar]=timerstm32;
						CmdSendUart(Changestatuscmd_uart,&g_apdu[stm32uartBUFstar],1);
						Main_status=Main_status_timedisplay;   					
					  KEYwork_flag=0;
						touch_key=KEY_NO; 
						KeyWorkflag=1;						
					}
				}
			}			
	  	}
	}
}

