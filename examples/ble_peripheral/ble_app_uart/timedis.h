#ifndef TIMEDIS_H__
#define TIMEDIS_H__
#include <stdint.h>

#define TimeDataDisPl       0
#define BalanceDisplay      1
#define BrodcastnameDisplay 2

#define balance_storgeblock 0
#define balnace_usefsize 59
#define numcoin8 8

#define   T_5SECOND_TimeOutremind  APP_TIMER_TICKS(5000)
#define   T_60SECOND_TimeOutremind  APP_TIMER_TICKS(60000)



//debug test
typedef uint32_t UTCTime; /* used to store the second counts for RTC */


extern unsigned char	Time_stuts;
extern unsigned char stm32timer[9];
extern unsigned char timedisp_status;
extern unsigned char flagtimerstm32;
extern unsigned char flagbalancestm32;
//extern unsigned char flagnamestm32;
extern UTCTime SecondCountRTC;


void timedisplay(void * p_event_data, uint16_t event_size);
void update_wall_clock(void * p_event_data, uint16_t event_size);


#endif

