#ifndef SCHEDULER_BITHD_H
#define SCHEDULER_BITHD_H
#include "app_scheduler.h"
//#include "app_timer.h"

#define SCHED_MAX_EVENT_DATA_SIZE       128//sizeof(app_timer_event_t)                   /**< Maximum size of scheduler events. Note that scheduler BLE stack events do not contain any data, as the events are being pulled from the stack in the event handler. */
#define SCHED_QUEUE_SIZE                20    

void scheduler_init(void);


#endif

