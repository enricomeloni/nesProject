#include "ovenAlarm_process.h"

#include "stdio.h"
#include "contiki.h"
#include "dev/leds.h"
#include "sys/etimer.h"

#include "commons/constants.h"
#include "commons/alarm_process.h"


extern process_event_t oven_start_preheating;
extern process_event_t oven_start_alarm;
extern process_event_t oven_stop_alarm;

unsigned char ovenAlarmState = OA_STOPPED;

PROCESS(ovenAlarm_process, "Oven Alarm blinking process");

PROCESS_THREAD(ovenAlarm_process, ev, data)
{
	static struct etimer alarmBlinkingTimer;
	
	PROCESS_BEGIN();
	
		while(1)
		{
			PROCESS_WAIT_EVENT();

            //if global alarm is on, ignore everything
			if(!isAlarmOn)
            {
                if(ev == oven_start_preheating)
                {
                    ovenAlarmState = OA_PREHEATING;                       
                    leds_on(LEDS_RED);
                }
                else if(ev == oven_start_alarm)
                {
                    ovenAlarmState = OA_ALARM;
                    etimer_set( &alarmBlinkingTimer, ALARM_LED_PERIOD * CLOCK_SECOND / 2 );
                    leds_toggle(LEDS_RED);
                }
                else if(ev == oven_stop_alarm)
                {
                    ovenAlarmState = OA_STOPPED;
                    etimer_stop(&alarmBlinkingTimer);
                    leds_off(LEDS_RED);
                }
                else if ( ovenAlarmState == OA_ALARM &&
                          ev == PROCESS_EVENT_TIMER &&
                          etimer_expired(&alarmBlinkingTimer) )
                {
                    #if DEBUG
                    printf("Alarm blinking\n");
                    #endif
                    leds_toggle(LEDS_RED);
                    etimer_reset(&alarmBlinkingTimer);
                }
            }
		}
	
	PROCESS_END();
}
