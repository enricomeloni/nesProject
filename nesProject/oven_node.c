#include "stdio.h"
#include "contiki.h"
#include "sys/etimer.h"
#include "dev/leds.h"
#include "dev/button-sensor.h"

#include "commons/constants.h"
#include "commons/command_process.h"
#include "speaker/ovenAlarm_process.h"
#include "oven/ovenRimeStack.h"

PROCESS(oven_node_init, "Oven init process");
AUTOSTART_PROCESSES(&oven_node_init, &command_process);

unsigned char ovenState = O_STOPPED;

process_event_t start_preheating;

void setLeds(unsigned char state)
{
    leds_off(LEDS_ALL);
    switch(state)
    {
        case O_STOPPED:
            leds_on(LEDS_RED);;
            break;
        case O_PREHEATING:
            leds_on(LEDS_BLUE);
            break;
        case O_READY_COOK:
            leds_on(LEDS_BLUE | LEDS_GREEN);
            break;
        case O_COOKING:
            leds_on(LEDS_GREEN);
            break;
    }
}

void setState(unsigned char state)
{
    ovenState = state;
    setLeds(state);
}

void command_switch(unsigned char command)
{
    if(command == O_TOGGLE_ACTION)
    {
        if(ovenState == O_STOPPED)
        {
            setState(O_PREHEATING);
            printf("Starting to preheat\n");
            process_post_synch(&oven_node_init, start_preheating, NULL);
        }

        if(ovenState == O_READY_COOK)
        {
            setState(O_COOKING);
            printf("Started cooking!\n");
            unsigned char cmd = OA_STOPPED;
            sendToSpeakers(&cmd, 1);
        }
    }
    if(command == O_STOP)
    {
        if(ovenState != O_STOPPED)
        {
            unsigned char cmd = OA_STOPPED;
            printf("Oven turned off\n");
            sendToSpeakers(&cmd, 1);
            setState(O_STOPPED);
        }
    }
}

PROCESS_THREAD(oven_node_init, ev, data)
{
    static struct etimer ovenPreheatTimer;

    PROCESS_BEGIN();
        initOvenRimeStack();
        setLeds(ovenState);
        start_preheating = process_alloc_event();
        while(1)
        {
            PROCESS_WAIT_EVENT();
            if( ev == start_preheating )
            {
                unsigned char cmd = OA_PREHEATING;
                sendToSpeakers(&cmd, 1);
                etimer_set(&ovenPreheatTimer, O_PREHEAT_TIME*CLOCK_SECOND);
            }
            if( ev == PROCESS_EVENT_TIMER &&
                etimer_expired(&ovenPreheatTimer))
            {
                printf("Oven is preheated\n");
                unsigned char command = OA_ALARM;
                sendToSpeakers(&command, 1);
                setState(O_READY_COOK);
            }
        }

    PROCESS_END();
}