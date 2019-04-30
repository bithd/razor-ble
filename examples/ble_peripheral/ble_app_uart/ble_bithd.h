#ifndef BLE_BITHD_H__
#define BLE_BITHD_H__
#include <stdint.h>


#define BLE_connect                       1
#define BLE_Disconnect                    0

#define BLE_AuthSuccess                   1
#define BLE_AuthFail                      0

extern unsigned char Ble_ConnnectFlag;
extern unsigned char ParingDataBuf[6];                                
extern unsigned char ParingDataDisFlag;         
extern unsigned char Ble_AuthFlag;

extern void blueDisconnect(void);
extern void BleCmdProcess(void * p_event_data, uint16_t event_size);
extern void Mac_address_get(void);


#endif

