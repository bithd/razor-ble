#ifndef IO_CTL_H_
#define IO_CTL_H_
#include "pca10040.h"
#include "Nrf_gpio.h"
#include "sys.h"

extern unsigned char poweronkey_flag;
extern unsigned char USB_connectFLag;
extern unsigned char USB_ChangingFLAG;

#define USBconnect    1  
#define USBDisconnect 0 
#define USBnoChanging 0 
#define USBChanged    1 

//KEY
#define KEY1_ST_PIN				8	//PWER Key
#define KEY2_ST_PIN				6

//FUNCTION
#define FUNC1					21
#define FUNC2					20
#define FUNC3					10

//CHARGER
#define USB_INSERT_PIN        	30
#define CHARG_STA_PIN			31

#define SlectPin 				10

#define STM_POWER_CTL_PIN    	16
#define BT_POWER_CTL_PIN	 	9

#define GPIO_HIGH				1
#define GPIO_LOW				0

#define GET_USB_INSERT()				(nrf_gpio_pin_read(USB_INSERT_PIN))

#define GET_BATTERY_FULL()				(nrf_gpio_pin_read(CHARG_STA_PIN))

#define GET_PWKEY_STATUS()				(nrf_gpio_pin_read(KEY1_ST_PIN))
#define GET_KEY2_STATUS()				(nrf_gpio_pin_read(KEY2_ST_PIN))

#define BT_POWER_ON()					nrf_gpio_pin_set(BT_POWER_CTL_PIN)
#define BT_POWER_OFF()					nrf_gpio_pin_clear(BT_POWER_CTL_PIN)

#define STM_POWER_ON()					nrf_gpio_pin_set(STM_POWER_CTL_PIN)
#define STM_POWER_OFF()					nrf_gpio_pin_clear(STM_POWER_CTL_PIN)

//Funtion IO
#define SET_FUNC1()						nrf_gpio_pin_set(FUNC1)
#define CLEAR_FUNC1()					nrf_gpio_pin_clear(FUNC1)

#define SET_FUNC2()						nrf_gpio_pin_set(FUNC2)
#define CLEAR_FUNC2()					nrf_gpio_pin_clear(FUNC2)

#define SET_FUNC3()						nrf_gpio_pin_set(FUNC3)
#define CLEAR_FUNC3()					nrf_gpio_pin_clear(FUNC3)

#define GET_FUNC3_STATUS()				(nrf_gpio_pin_read(FUNC3))		

#define INIT_FUNC()						CLEAR_FUNC1();\
										CLEAR_FUNC2();\
										CLEAR_FUNC3()
										
#define BLE_CONNECT()					CLEAR_FUNC1();\
										CLEAR_FUNC2();\
										SET_FUNC3()	
										
#define BlE_DISCONNECT()				CLEAR_FUNC1();\
										SET_FUNC2();\
										CLEAR_FUNC3()
										
#define BATPERCENT_FUNC()				CLEAR_FUNC1();\
										SET_FUNC2();\
										SET_FUNC3()

#define CHARGE_FUNC()					SET_FUNC1();\
										CLEAR_FUNC2();\
										CLEAR_FUNC3()

#define MVCHARGE_FUNC()					SET_FUNC1();\
										CLEAR_FUNC2();\
										SET_FUNC3()

#define FWDOWNLD_FUNC()					SET_FUNC1();\
										SET_FUNC2();\
										CLEAR_FUNC3()
										
#define PAIR_FUNC()						SET_FUNC1();\
										SET_FUNC2();\
										SET_FUNC3()

void gpio_init(void);
void firmwaredownload_GPIO_L(void);
void ResetFunctionIo(void);
//END JYJ

//GPIO define

//output difine
#define	GPIO_OUT_CONFIG(pin_number,pull_config)		nrf_gpio_cfg(pin_number,NRF_GPIO_PIN_DIR_OUTPUT,NRF_GPIO_PIN_INPUT_DISCONNECT,pull_config,NRF_GPIO_PIN_S0S1,NRF_GPIO_PIN_NOSENSE);
//input define
#define	GPIO_IN_CONFIG(pin_number,pull_config)		nrf_gpio_cfg_input(pin_number,pull_config)
					
			
#define GPIO_LOW						0
#define GPIO_HIGH						1



///////////////////////////////////////////////////////////////////////////////////////////////

void usr_gpio_init(void);
void vIO_KeyboardMangement(void);
void SendKey1Action(void);
void SendKey2Action(void);
void SendBatPercent(void);
void SendChargeInsert(void);
void SendRemoveCharge(void);
void FirmwareDownload(void);

void firmwaredownload_GPIO_H(void);
void PowerOff_key(void);
void PowerOn_key(void);

//void vIO_SetGpioInput(void);
//void vIO_SetGpioOutput(void);

#endif
