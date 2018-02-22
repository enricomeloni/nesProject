#include "stdio.h"
#include "contiki.h"
#include "dev/leds.h"
#include "dev/button-sensor.h"
#include "net/rime/rime.h"

#include "commons/constants.h"
#include "commons/command_process.h"
#include "home_theater/htRimeStack.h"

PROCESS(ht_node_init, "Mbox Node init Process");
AUTOSTART_PROCESSES(&ht_node_init, &command_process);

/*
    state = 0 stopped
    state = 1 playing
    state = 2 paused
*/
unsigned char state = HT_STOPPED;

void setLeds(unsigned char currentState)
{
    leds_off(LEDS_ALL);
    switch(currentState)
    {
        case HT_STOPPED:
            leds_on(LEDS_RED);
            break;
        case HT_PLAYING:
            leds_on(LEDS_GREEN);
            break;
        case HT_PAUSED:
            leds_on(LEDS_BLUE);
            break;
    }
}

void command_switch(unsigned char command)
{
    unsigned char rlCommand;
	if(command == HT_TOGGLE_PLAY)
	{
        switch(state)
        {
            case HT_STOPPED:
            case HT_PAUSED:
                state = HT_PLAYING;
                rlCommand = RL_DIM;
		        printf("Send DIM command\n");
                break;
            case HT_PLAYING:
                state = HT_PAUSED;
                rlCommand = RL_BRIGHTEN;
                printf("Send BRIGHTEN command\n");
                break;
        }
		sendToRoomLightNode(&rlCommand, 1);
	}
	else if(command == HT_STOP)
	{
		if(state == HT_PLAYING || state == HT_PAUSED)
        {
            state = HT_STOPPED;
            rlCommand = RL_TURN_ON;
            printf("Send TURN_ON command\n");
		    sendToRoomLightNode(&rlCommand, 1);
        }
        else if(state == HT_STOPPED)
            printf("Home teather is already stopped\n");
	}
    else{
        printf("Command not regonized, ignored\n");
        return;
    }

    setLeds(state);
}

void print_commands()
{
    printf("Available commands:\n");
    if(state == HT_STOPPED)
    {
        printf("1. Play");
    }
    else if(state == HT_PLAYING)
    {
        printf("1. Pause\n2. Stop\n");
    }
    else if(state == HT_PAUSED)
    {
        printf("1. Play\n2. Stop\n");
    }
}

PROCESS_THREAD(ht_node_init, ev, data)
{
	PROCESS_BEGIN();
		initHTRimeStack();
        setLeds(HT_STOPPED);
        print_commands();
	PROCESS_END();
}
