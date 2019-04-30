#ifndef BLUETOOTH_APPPROTOCOL_H_
#define BLUETOOTH_APPPROTOCOL_H_
#include <stdint.h>

#define version_0 0x04
#define version_1 0x00
#define version_2 0x00

#define recivestatus_cmdid 9  

#define setup_time            0x01  
#define setup_balance         0x02 
#define setup_timeout         0x03
#define setup_getpower        0x04
#define setup_getversion      0x05
#define setup_turnoff         0x06


typedef struct BluetoothDataStruct		
{
	uint8_t *label;      
	uint8_t *length;    
	uint8_t *cmd_id;    
	uint8_t *data;       
	uint8_t *crc16;      
} BluetoothData;

extern BluetoothData communicationBluetooth;


void Send_bluetoothdata(unsigned short len);


#endif

