#include "stdio.h"
#include "contiki.h"
#include "sys/etimer.h"
#include "dev/leds.h"
#include "dev/button-sensor.h"

#include "commons/constants.h"
#include "commons/alarm_process.h"
#include "speaker/ovenAlarm_process.h"
#include "speaker/speakerRimeStack.h"

PROCESS(speaker_node_init, "Speaker init process");
AUTOSTART_PROCESSES(&speaker_node_init, &alarm_process, &ovenAlarm_process);

process_event_t oven_start_preheating;
process_event_t oven_start_alarm;
process_event_t oven_stop_alarm;

extern process_event_t alarm_toggled_event;

void processCUCommand(unsigned char command)
{
	printf("processing cu command\n");
    if( command == ALARM_TOGGLE_COMMAND )
	{
		if(!isAlarmOn)
		{
			process_post_synch(&alarm_process, alarm_toggled_event, NULL);
		}
		else
		{
			process_post_synch(&alarm_process, alarm_toggled_event, NULL);
		}
	}
}

void processOvenCommand(unsigned char command)
{
    if(command == OA_STOPPED)
	{
		process_post_synch(&ovenAlarm_process, oven_stop_alarm, NULL);
	}

	if(command == OA_PREHEATING)
	{
		process_post_synch(&ovenAlarm_process, oven_start_preheating, NULL);
	}

	if(command == OA_ALARM)
	{
		process_post_synch(&ovenAlarm_process, oven_start_alarm, NULL);
	}
}

PROCESS_THREAD(speaker_node_init, ev, data)
{
    PROCESS_BEGIN();
        initSpeakerRimeStack();
		oven_start_preheating = process_alloc_event();
		oven_start_alarm = process_alloc_event();
		oven_stop_alarm = process_alloc_event();
    PROCESS_END();
}