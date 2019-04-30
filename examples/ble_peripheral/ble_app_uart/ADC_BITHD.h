#ifndef _ADC_DMWZ_H_
#define _ADC_DMWZ_H_
#include <stdint.h>


#define Full_Voltage             (4000)                     //Full power 4.2V
#define Warning_Voltage          (3650)                     //warning    3.8V
#define Lowest_Voltage           (3500)                     //NO power
#define ADV_BLE_VOLTAGE			 (3450)
#define DIS_BLE_VOLTAGE			 (3400)


extern volatile int32_t adc_sample;      //storge batterylevel
extern unsigned char FullVoltage_flag;

void battery_adc_read_once(uint16_t* batt_lvl_in_milli_volts);
void adc_config(void);
void ReadAdc(void * p_event_data, uint16_t event_size);

void Chargingandmanagerdisplay(void * p_event_data, uint16_t event_size);

#endif
