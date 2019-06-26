#include <string.h>
#include "Bluetooth_APPprotocol.h"
#include "Bluetooth_HWprotocol.h"
#include "Timer_Interrupt_Function.h"
#include "CRC16_BITHD.h"
#include "IO_CTL.h"
#include "sys.h"
#include "nrf_delay.h"
#include "program.h"
#include "uart_bithd.h"
#include "timedis.h"
#include "ADC_BITHD.h"
#include "bluetoothKEY_bithd.h"
#include "pstorage.h"
#include "flashmcu_bithd.h"
#include "app_scheduler.h"
#include "peer_manager.h"

BluetoothData communicationBluetooth={&g_apdu[0],&g_apdu[1],&g_apdu[3],&g_apdu[4]};//save recive data pointer
unsigned char CMD09_oldlabel=0;   
unsigned char oldCRC[2]={0,0};
unsigned char CMD09_SW[2]={0,0};       

void Recive_bluetoothdata_point(void)
{
	communicationBluetooth.crc16=&g_apdu[g_apdu_length-2];
	blueRecivSendflag=bluetoothNOdata;
}

/*************************************
len the length of data is KEY+value
**************************************/
void Send_bluetoothdata(unsigned short len)
{
	unsigned short l=len;
	unsigned short crc16;
	//Organize the data according to the protocol
	l=l+3;
	communicationBluetooth.length[0]=l>>8;
	communicationBluetooth.length[1]=0x00ff&l;

	g_apdu_length=l+3;
	
	communicationBluetooth.crc16=&g_apdu[g_apdu_length-2];  //according send data updata crc data
    crc16=bd_crc16(0,g_apdu,g_apdu_length-2);              
	communicationBluetooth.crc16[0]=crc16>>8;
	communicationBluetooth.crc16[1]=crc16&0x00ff;

	blueRecivSendflag=bluetoothSenddata;                    //send enable	
}

/****************************
check crc success or fail
*****************************/
unsigned char bluetoothjudge_crc16(void)
{
	unsigned short crc0,crc1;
	crc0=bd_crc16(0,g_apdu,g_apdu_length-2);
	crc1=communicationBluetooth.crc16[0];
	crc1=(crc1<<8)|communicationBluetooth.crc16[1];
	
	if(crc1!=crc0)
	{
		return 1;   //fail
	}
	return 0;       //success
}


void recivestatus_F(void)
{	
	communicationBluetooth.data[0]=CMD09_oldlabel;
	communicationBluetooth.data[1]=CMD09_SW[0];	
	communicationBluetooth.data[2]=CMD09_SW[1];	

	Send_bluetoothdata(3);

}


void setup_time_f(unsigned char* value)
{
	//time_union_t time;
	//time.data = 0;
	//time.data |= value[3];
	//time.data |= value[2] << 8;
	//time.data |= value[1] << 16;
	//time.data |= value[0] << 24;
	//set_system_clock(time); 
  Send_bluetoothdata(1);
  
	if(	poweronkey_flag==0)
	{
		firmwaredownload_GPIO_H();      //enter APP mode
		nrf_gpio_pin_clear(SlectPin);
		PowerOn_key();                  //stm32 poweron
	}

	if(Main_status!=Main_status_timedisplay)
	{
		if(Ack_recive_enable==0)
		{
			  KEYwork_flag=0;
			Main_status=Main_status_timedisplay;    
			g_apdu[stm32uartBUFstar]=timerstm32;//send cmd ,enter timer display mode
			CmdSendUart(Changestatuscmd_uart,&g_apdu[stm32uartBUFstar],1);
		}
	}

}

/***********************
balance seting
*************************/
void setup_balance_f(unsigned char* value)
{	
	unsigned char	*p;
	//ret_code_t 		ret;
	//uint16_t 		words;
	uint8_t 		file_id=0x03;
	//uint8_t 		flag_file_id=0x06;
	//uint32_t 		back_buff[64];
	
	memset(g_flashbuff,0,sizeof(g_flashbuff));
    //update balance
    memcpy(&coinbalance,value,balnace_usefsize);
    
	p=(unsigned char*)&coinbalance;

	uint8_t* pfb = (uint8_t*) g_flashbuff;

	memcpy(pfb,p, balnace_usefsize);
	//flash storage
	//Save balance data to flash

	fds_test_write(file_id, g_flashbuff,60/4);	

	//test
	//ret = fds_read(file_id, back_buff, &words);

	//Save coinkind and flag
	//memset(g_flashbuff,0x55,sizeof(g_flashbuff));
	
	//ret = fds_test_write2(flag_file_id,g_flashbuff,16); 	

	//ret = fds_read2(flag_file_id, buf123, &words);

	Send_bluetoothdata(1);
}

void setup_timeout_f(unsigned char* value)
{
	unsigned int i=value[0]*10000;
	unsigned int timeout=APP_TIMER_TICKS(i);

  if(i!=0)
	{
	app_timer_stop(Timeout3Sec_id); 
	app_timer_start(Timeout3Sec_id,timeout, NULL);
	Timeout3Sec_StarFlag=TimeOpen; 
	Send_bluetoothdata(1);		
	}

}

void setup_getpower_f(unsigned char* value)
{
	uint8_t batlvl=0;
	if(adc_sample<Lowest_Voltage){batlvl=0;}
	else
	{
		if(adc_sample<Full_Voltage){batlvl=(adc_sample-Lowest_Voltage)*100/(Full_Voltage-Lowest_Voltage);}//get battery level
		else{batlvl=100;}
	}
	communicationBluetooth.data[1]=batlvl;	
	Send_bluetoothdata(2);

}

void setup_getversion_f(unsigned char* value)
{

	communicationBluetooth.data[1]=version_0;	
	communicationBluetooth.data[2]=version_1;
	communicationBluetooth.data[3]=version_2;
	Send_bluetoothdata(4);

}

void setup_turnoff_f(void)
{
	app_timer_stop(Timeout3Sec_id);                                 //stop timer
	Timeout3Sec_StarFlag=TimeClose;                                 //clear flag
	Time_stuts=TimeDataDisPl;

	Main_status=Main_status_closeoled;                              //states chang to shutdown mode
	STM_POWER_OFF();
	PowerOff_key();                                                 //stm32 power off
	Send_bluetoothdata(1);
}


/**************************************************
seting cmd manage function
***************************************************/
void Setup_F(void)
{	
	   //according different key
		switch(communicationBluetooth.data[0])
		{
			case setup_time:           setup_time_f(&communicationBluetooth.data[1]);
			break;
			
			case setup_balance:        setup_balance_f(&communicationBluetooth.data[1]);
			break;
			
			case setup_timeout:        setup_timeout_f(&communicationBluetooth.data[1]);
			break;

			case setup_getpower:       setup_getpower_f(&communicationBluetooth.data[1]);
			break;
			
			case setup_getversion:     setup_getversion_f(&communicationBluetooth.data[1]);
			break;
			
			case setup_turnoff:        setup_turnoff_f();
      break;
		}			
}


void download_cmdid_F(void)
{

  	if(communicationBluetooth.data[0]==0x01)
	{
		app_timer_stop(Timeout3Sec_id);          //turn off timer
		Timeout3Sec_StarFlag=TimeClose;
		if(Main_status==Main_status_closeoled)   //oled is shutdown,you can update firmware
		{
			firmwaredownload_GPIO_L();           //entern update mode
			STM_POWER_ON();
  			nrf_delay_ms(delaytime);
			PowerOn_key();
		
			Main_status=Main_status_download;
			KEYwork_flag=1;
			communicationBluetooth.data[0]=0x02;
			communicationBluetooth.data[1]=0x00;
			communicationBluetooth.data[2]=0x00;
		}
		else
		{
			communicationBluetooth.data[0]=0x02;
			communicationBluetooth.data[1]=0x01;
			communicationBluetooth.data[2]=0x00;				
		}
		Send_bluetoothdata(3); 	            
	}

	if(communicationBluetooth.data[0]==0x03&& Main_status==Main_status_download)
	{
         //apdu cmd
		 APDU_blekey=&communicationBluetooth.data[3];                                       //datas bufer
		 Apdu_len_blekey=(communicationBluetooth.data[1]<<8)|communicationBluetooth.data[2];//data length	
	}
	
	if(communicationBluetooth.data[0]==0x04&& Main_status==Main_status_download)
	{
		//exit update mode
		app_timer_stop(Timeout3Sec_id);        
		Timeout3Sec_StarFlag=TimeClose;

		STM_POWER_OFF();
		PowerOff_key();
		nrf_delay_ms(delaytime);
		firmwaredownload_GPIO_H();//enter APP mode

		nrf_gpio_pin_clear(SlectPin);

		STM_POWER_ON();
		PowerOn_key();

		g_apdu[stm32uartBUFstar]=timerstm32;
		CmdSendUart(Changestatuscmd_uart,&g_apdu[stm32uartBUFstar],1);
		Main_status=Main_status_timedisplay;   

		KEYwork_flag=0;

		communicationBluetooth.data[0]=0x02;
		communicationBluetooth.data[1]=0x00;
		communicationBluetooth.data[2]=0x00;

		Send_bluetoothdata(3); 		
	}
}


void test_cmdid_F(void)
{
//	  printf("test_cmdid_F\r\n");
}


void blueKEY_cmdid_F(void)
{
	if(Main_status==Main_status_download)
	{return;}

	if(communicationBluetooth.data[0]==0x02&&KEYwork_flag==1)
	{//apduָ manage     
		APDU_blekey=&communicationBluetooth.data[3];
		Apdu_len_blekey=(communicationBluetooth.data[1]<<8)|communicationBluetooth.data[2];
	}
	else
	{
		if(communicationBluetooth.data[0]!=0x01)
		{return;}
		app_timer_stop(Timeout3Sec_id);      
		Timeout3Sec_StarFlag=TimeClose;

		if(communicationBluetooth.data[1]==0x01)//&&KEYwork_flag!=1)
		{
			STM_POWER_OFF();
			PowerOff_key();
			//nrf_delay_ms(delaytime);
			firmwaredownload_GPIO_H();//APP mode

      		nrf_gpio_pin_clear(SlectPin);
      		STM_POWER_ON();
      		nrf_delay_ms(delaytime);
			PowerOn_key();

			g_apdu[stm32uartBUFstar]=bluekeystm32;
			CmdSendUart(Changestatuscmd_uart,&g_apdu[stm32uartBUFstar],1);
			KEYwork_flag=1;
			Main_status=Main_status_blekey;
		}
		if(communicationBluetooth.data[1]==0x00)//&&(KEYwork_flag!=0))
		{
			STM_POWER_OFF();
			PowerOff_key();
			nrf_delay_ms(delaytime);
			firmwaredownload_GPIO_H();//APP mode
			nrf_gpio_pin_clear(SlectPin);
			STM_POWER_ON();
			PowerOn_key();
			g_apdu[stm32uartBUFstar]=timerstm32;
			CmdSendUart(Changestatuscmd_uart,&g_apdu[stm32uartBUFstar],1);
			Main_status=Main_status_timedisplay;     
			Time_stuts=TimeDataDisPl;
			KEYwork_flag=0;
			Send_bluetoothdata(1); 	            
		}
	}
}


///////////////////////////////////////FLASH read or write//////////////////////////
void blue_writeflash(unsigned char* buf, uint16_t len)
{
	uint8_t ret_len=1;
	ret_code_t ret;
	uint8_t file_id;

	file_id = buf[0];
	len --;

	uint8_t* pfb = (uint8_t*) g_flashbuff;
	pfb[0] = len >> 8;
	pfb[1] = len & 0xff;
	memcpy(pfb + 2, buf+1, len);

	len += 2;
	if(len<=128)
	{
		ret = fds_test_write(file_id, g_flashbuff, (len + 3) / 4);	
	}
	
	if((ret != 0)||(len>128))
	{
		ret_len += 2;
		communicationBluetooth.data[1] = 0x02;
		communicationBluetooth.data[2] = 0x03;
	}
	Send_bluetoothdata(ret_len); 	         
}

void blue_readflash(unsigned char address)
{	
	ret_code_t ret;
	uint16_t len=0;
	uint16_t words;
	uint8_t file_id;
	uint8_t* pfb = (uint8_t*)g_flashbuff;

	file_id = address;
	ret = fds_read(file_id, g_flashbuff, &words);
	len = (((uint16_t)pfb[0]) << 8) + pfb[1];
	// TODO check (len + 3) / 4 == words 
	
	if(len<=128)
	{
		memcpy(&communicationBluetooth.data[1], pfb+2, len);
	}
	
	if((ret != 0)||(len>128))
	{
		len+=2;
		communicationBluetooth.data[1] = 0x02;
		communicationBluetooth.data[2] = 0x03;
	}
	
	Send_bluetoothdata(len+1);
}

void flashwriteread_F(void)
{
	uint16_t L=(communicationBluetooth.length[0]<<8)|communicationBluetooth.length[1];

	L -=4;
	switch(communicationBluetooth.data[0])
	{
		case 0x01://write flash
			blue_writeflash(&communicationBluetooth.data[1],L);
			break;
		case 0x02://read flash
			blue_readflash(communicationBluetooth.data[1]);
			break;
	}
}


void firmware_signed_F(void)
{
	nrf_gpio_pin_set(SlectPin);
	STM_POWER_ON();
	nrf_delay_ms(delaytime);
	PowerOn_key();
	Main_status=Main_status_firmware;
}

void bluetoothupdate_F(void)
{
	//unsigned char i;

	Send_bluetoothdata(1);
	app_sched_event_put(NULL,NULL, Bluetooth_ReciveANDSend);

	#if 0
	for(i=0;i<20;i++){
	app_sched_execute(); 
  sd_app_evt_wait();  
	}
//	sd_power_gpregret_set(0xb1);
	NVIC_SystemReset();
	#endif
}


///////////////////////////////////////FLASH read or write END//////////////////////////
/*******************************
according different cmd to work
**********************************/
void BluetoothWork(void)
{
	void (*buf[8])(void)={   &download_cmdid_F,\
							&Setup_F,\
							&flashwriteread_F,\
							&bluetoothupdate_F,\
							&firmware_signed_F,\
							NULL,\
							&blueKEY_cmdid_F,\
							NULL
						};

	void (*f)(void);                              
	//app_timer_stop(Timeout3Sec_id);         
	//Timeout3Sec_StarFlag=TimeClose;
	f=buf[communicationBluetooth.cmd_id[0]-1];	  
	f();                                          
}

void BleCmdProcess(void * p_event_data, uint16_t event_size)
{
	uint16_t L=(communicationBluetooth.length[0]<<8)|communicationBluetooth.length[1];
	  
	if(blueRecivSendflag==bluetoothRecivedata)
	{		
		 //recived data
		if((L+3)==g_apdu_length)
		{
			Recive_bluetoothdata_point();                            //analyze data
																				
			if(recivestatus_cmdid!=communicationBluetooth.cmd_id[0])
			{      
        		if((CMD09_oldlabel!=communicationBluetooth.label[0])||(communicationBluetooth.crc16[0]!=oldCRC[0])||(communicationBluetooth.crc16[1]!=oldCRC[1]))
				{					
					CMD09_oldlabel=communicationBluetooth.label[0];        
					oldCRC[0]=communicationBluetooth.crc16[0];
					oldCRC[1]=communicationBluetooth.crc16[1];
					if(!bluetoothjudge_crc16())                           
					{                                                   
						CMD09_SW[0]=0x90;                                  
						BluetoothWork(); //work according cmd
					}
					else
					{   
						CMD09_SW[0]=0x67;                           
					}
			  }
			}
			else
			{
				recivestatus_F(); 
			}			
		
		}
	}
}

