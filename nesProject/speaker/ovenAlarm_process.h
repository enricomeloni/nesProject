
#ifndef NESPROJECT_OVENALARM_PROCESS_H
#define NESPROJECT_OVENALARM_PROCESS_H

#include "contiki.h"

extern process_event_t oven_start_preheating;
extern process_event_t oven_start_alarm;
extern process_event_t oven_stop_alarm;

extern unsigned char isOvenAlarmOn;

PROCESS_NAME(ovenAlarm_process);

#endif //NESPROJECT_OVENALARM_PROCESS_H
