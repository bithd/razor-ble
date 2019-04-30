#include "IO_CTL.h"
#include "nrf_delay.h"
#include "uart_bithd.h"
#include "program.h"


unsigned char USB_connectFLag=USBDisconnect;
unsigned char USB_ChangingFLAG=USBnoChanging;
unsigned char poweronkey_flag=0;


void usr_gpio_init(void)
{
	uint16_t counter=0;
		
	//KEY
	nrf_gpio_cfg_input(KEY1_ST_PIN,NRF_GPIO_PIN_PULLDOWN);
	nrf_gpio_cfg_input(KEY2_ST_PIN,NRF_GPIO_PIN_PULLUP);

	//FUNCTION
	//nrf_gpio_cfg_output(FUNC1);
	//nrf_gpio_cfg_output(FUNC2);
	nrf_gpio_cfg_output(FUNC3);
	//nrf_gpio_cfg_input(FUNC3,NRF_GPIO_PIN_PULLUP);	

	//CHARGER
	nrf_gpio_cfg_input(USB_INSERT_PIN,NRF_GPIO_PIN_PULLUP);	
	nrf_gpio_cfg_input(CHARG_STA_PIN,NRF_GPIO_PIN_PULLUP);	

	firmwaredownload_GPIO_H();
	
	//POWEER Control
	BT_POWER_OFF();
	STM_POWER_OFF();
	nrf_gpio_cfg_output(BT_POWER_CTL_PIN);
	nrf_gpio_cfg_output(STM_POWER_CTL_PIN);

	if(GET_USB_INSERT() == GPIO_LOW)
	{
		BT_POWER_ON();
		STM_POWER_ON(); 
	}
	
	while(GET_PWKEY_STATUS() == GPIO_HIGH)
	{
		nrf_delay_ms(50);
		counter++;
		if(counter>20)		//Long Press 1.5S
		{
			BT_POWER_ON();
			STM_POWER_ON(); 		
		}
		if(counter>60000)
		{
			break;
		}
	}

	CLEAR_FUNC1();
	CLEAR_FUNC2();
	CLEAR_FUNC3();
	
	//SET_FUNC1();
	//SET_FUNC2();	
	//SET_FUNC3();
}

void ResetFunctionIo(void)
{
	CLEAR_FUNC1();
	CLEAR_FUNC2();
	CLEAR_FUNC3();
}
void SendKey1Action(void)
{	
//	uint16_t count=2000;

	CLEAR_FUNC1();
	CLEAR_FUNC2();
	CLEAR_FUNC3();
	
	#if 1

	#if 0
	count = 1;
	while(count--)
	{
		CLEAR_FUNC1();
		nrf_delay_ms(5);
		SET_FUNC1();
		nrf_delay_ms(5);
		CLEAR_FUNC1();
	}
	return;
	#endif

	SET_FUNC1();
	
	//CLEAR_FUNC1();
	//SET_FUNC2();

	#if 0
	while((count != 0)||(GET_FUNC3_STATUS() != GPIO_HIGH))
	{
		count--;
	}
	
	CLEAR_FUNC1();
	SET_FUNC2();

	while((count != 0)||(GET_FUNC3_STATUS() != GPIO_HIGH))
	{
		count--;
	}
	
	INIT_FUNC();
	#endif
	#endif
}

void SendKey2Action(void)
{
//	uint16_t count=2000;
	#if 0
	count = 1;
	while(count--)
	{
		CLEAR_FUNC2();
		nrf_delay_ms(5);
		SET_FUNC2();
		nrf_delay_ms(5);
		CLEAR_FUNC2();
	}
	return;

	#endif
			
	//CLEAR_FUNC1();
	//CLEAR_FUNC2();
	//CLEAR_FUNC3();
	
	//SET_FUNC2();	
	CLEAR_FUNC1();

	#if 0
	while((count != 0)||(GET_FUNC3_STATUS() != GPIO_HIGH))
	{
		count--;
	}

	SET_FUNC1();
	CLEAR_FUNC2();

	while((count != 0)||(GET_FUNC3_STATUS() != GPIO_HIGH))
	{
		count--;
	}
	
	INIT_FUNC();
	#endif
}

void SendBatPercent(void)
{
	uint16_t count=2000;
	
	CLEAR_FUNC1();
	SET_FUNC2();
	while((count != 0)||(GET_FUNC3_STATUS() != GPIO_HIGH))
	{
		count--;
	}

	SET_FUNC1();
	SET_FUNC2();

	while((count != 0)||(GET_FUNC3_STATUS() != GPIO_HIGH))
	{
		count--;
	}
	
	INIT_FUNC();
}

void SendChargeInsert(void)
{
	uint16_t count=2000;
	
	SET_FUNC1();
	CLEAR_FUNC2();
	while((count != 0)||(GET_FUNC3_STATUS() != GPIO_HIGH))
	{
		count--;
	}

	CLEAR_FUNC1();
	SET_FUNC2();

	while((count != 0)||(GET_FUNC3_STATUS() != GPIO_HIGH))
	{
		count--;
	}
	
	INIT_FUNC();
}

void SendRemoveCharge(void)
{
	uint16_t count=2000;
	
	SET_FUNC1();
	CLEAR_FUNC2();
	while((count != 0)||GET_FUNC3_STATUS() != GPIO_HIGH)
	{
		count--;
	}

	SET_FUNC1();
	CLEAR_FUNC2();	

	while((count != 0)||(GET_FUNC3_STATUS() != GPIO_HIGH))
	{
		count--;
	}
	
	INIT_FUNC();
}

void FirmwareDownload(void)
{
	uint16_t count=2000;
	SET_FUNC1();
	CLEAR_FUNC2();
	while((count != 0)||GET_FUNC3_STATUS() != GPIO_HIGH)
	{
		count--;
	}

	SET_FUNC1();
	SET_FUNC2();

	while((count != 0)||(GET_FUNC3_STATUS() != GPIO_HIGH))
	{
		count--;
	}
	
	INIT_FUNC();
}

void firmwaredownload_GPIO_H(void)
{
	nrf_gpio_cfg_output(14);  
	nrf_gpio_pin_set(14);      
}

void firmwaredownload_GPIO_L(void)
{
	nrf_gpio_cfg_output(14);   
	nrf_gpio_pin_clear(14);   
}


void PowerOn_key(void)
{
	//nrf_gpio_pin_set(STM_POWER_CTL_PIN);
  	//nrf_delay_ms(delaytime);
	if(uart_enable_flag==0)
	{
		uart_init();
	}	
	//nrf_delay_ms(delaytime);
	poweronkey_flag=1;
}

void PowerOff_key(void)
{
	poweronkey_flag=0;
	//nrf_gpio_pin_clear(STM_POWER_CTL_PIN);
	nrf_gpio_pin_clear(SlectPin);
	if(uart_enable_flag==1)
	{
		Uart_close();
	}
	CRC_old=0;
}

