#ifndef TIMER_INTERRUPT_FU_H__
#define TIMER_INTERRUPT_FU_H__

#define TimeOpen  1  
#define TimeClose 0 
#define TimeOut   2


#define   TouchTimerMax 200                                              
#define   Touch_TIMER_TICKS       APP_TIMER_TICKS(100)
#define   Touch_TIMER_Scan        APP_TIMER_TICKS(200) 

#define   NoTouch_Timeout_max     APP_TIMER_TICKS(10000)
#define   LowPowTouch_Timeout_etu APP_TIMER_TICKS(1000) 
#define   NoWorkTimes             1
#define   WorkTimes               1
#define   TotleTimes              (WorkTimes+NoWorkTimes)


#define   ReadStatus_TIMER_Scan  APP_TIMER_TICKS(1000)

#define   ADC_INTERVAL            APP_TIMER_TICKS(500) 
#define   ONE_SECOND_INTERVAL_     APP_TIMER_TICKS(100)

#define   ADC_NOpower             APP_TIMER_TICKS(3000)

#define   bletimeout_INTERVAL            APP_TIMER_TICKS(200)   //200ms

#define   _20ms_INTERVAL            APP_TIMER_TICKS(100)   //10ms

#define   motortimeout_LOOP              APP_TIMER_TICKS(100)

#define   T_3SECOND_TimeOut        APP_TIMER_TICKS(3000)
#define   T_blue_TimeOut        APP_TIMER_TICKS(60000)
#define   T_update_TimeOut        APP_TIMER_TICKS(1200000)
#define   KeepTouchTime            60                                  
#define   T_1SECOND_TimeOut        APP_TIMER_TICKS(3000)
#define   T_10SECOND_TimeOut       APP_TIMER_TICKS(10000)
#define   T_3SECOND_TimeOutremind  APP_TIMER_TICKS(3000)
#define   T_5SECOND_TimeOutremind  APP_TIMER_TICKS(5000)
#define   T_60SECOND_TimeOutremind  APP_TIMER_TICKS(60000)
#define   T_5Mini_TimeOutremind  APP_TIMER_TICKS(300000)

#define m_app_TIMER_T APP_TIMER_TICKS(1000)  //debug ms


//extern app_timer_id_t                 m_app_timer_id;
extern unsigned char                  Touch_timercount;

extern unsigned char                  LowPowTouch_times;
extern unsigned char                  Timeout3Sec_StarFlag;
extern unsigned char                  Timeout1Sec_f_StarFlag;

extern unsigned char                   motor_timeout;


extern unsigned char                   Timeout1Sec_Uart_StarFlag;
extern unsigned char                  chargstatustime_flag;


extern void Balance_handler(void * p_context);
extern void ADCwork_handler(void * p_context);
extern void Motor_timeout_handler(void * p_context);
extern void Ble_timeout_handler(void * p_context);
extern void TimerClock_handler(void * p_context);
extern void TimeOutMain_handler(void * p_context);
extern void TimeOut1sec_handler(void * p_context);
extern void TimeOutUart_handler(void * p_context);
extern void Timechargestatus_handler(void * p_context);



#endif

