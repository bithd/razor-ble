#ifndef _BLUETOOTH_HWPROTOCOL_H__
#define _BLUETOOTH_HWPROTOCOL_H__
#include "stdint.h"


#define bluetoothNOdata     0 
#define bluetoothRecivedata 1 
#define bluetoothSenddata   2

#define PackRecive 20                          
#define WaitRertyTims 3              
#define TimeOver      1              

#define DATAFIELD_MAX_LEN		1024
#define APDU_HEAD_LEN			8
#define APDU_MAX_LEN			(APDU_HEAD_LEN+DATAFIELD_MAX_LEN+8)


typedef unsigned char * pu8;
typedef unsigned char	uint8;
typedef struct TimerProgram		
{
	uint32_t T_0;   
	uint32_t T_1;   
} TimProg;

//hardware device 
extern uint32_t volatile timer0_sec_count;
extern unsigned char DataP[PackRecive];       
extern unsigned char Len_DATA;                
extern unsigned char Recivedataflag;           

//app
extern uint8_t g_apdu[APDU_MAX_LEN];
extern unsigned char blueRecivSendflag;
extern void Bluetooth_ReciveANDSend(void * p_event_data, uint16_t event_size);


extern uint8_t CheckExist(uint8_t PackN, uint8_t *Msg);
extern void phone_to_ble_data_parse(uint8_t * buf, uint16_t buf_len);

#endif

