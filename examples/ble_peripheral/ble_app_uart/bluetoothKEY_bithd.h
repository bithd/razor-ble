#ifndef BLUETOOTHKEY_BITHD_H_
#define BLUETOOTHKEY_BITHD_H_
#include <stdint.h>

#define BleKEYchoose     0   
#define BleKEYchooseOver 1  

extern unsigned char *APDU_blekey;
extern unsigned short Apdu_len_blekey;
//extern unsigned blekeydisp_status;
void Bluetooyhkeydisplay(void * p_event_data, uint16_t event_size);
void BlueUPfirmware(void * p_event_data, uint16_t event_size);
#endif


