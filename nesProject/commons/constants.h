//
// Created by enric on 27/10/2017.
//

#ifndef NESPROJECT_CONSTANTS_H
#define NESPROJECT_CONSTANTS_H

#define COMMANDS_NUMBER 5

#define ALARM_TOGGLE_COMMAND 1
#define GATELOCK_TOGGLE_COMMAND 2
#define DOORS_OPEN_COMMAND 3
#define AVERAGE_TEMPERATURE_COMMAND 4
#define LIGHT_VALUE_COMMAND 5

//oven state
#define O_STOPPED 0
#define O_PREHEATING 1
#define O_READY_COOK 2
#define O_COOKING 3

//preheating time
#define O_PREHEAT_TIME 5

//oven commands
#define O_TOGGLE_ACTION 1
#define O_STOP 2

//oven alarm state
#define OA_STOPPED 0
#define OA_PREHEATING 1
#define OA_ALARM 2

//commands for hometheater
#define HT_TOGGLE_PLAY 1
#define HT_STOP 2

#define HT_STOPPED 0
#define HT_PLAYING 1
#define HT_PAUSED 2

//commands for light room
#define RL_OFF 0
#define RL_DIM 1
#define RL_BRIGHTEN 2
#define RL_TURN_ON 3

#define COMMAND_TIMEOUT 4
#define ALARM_LED_PERIOD 2

#define AUTO_OPENING_LED_PERIOD 2
#define AUTO_OPENING_BLINKINGS 16
#define DOOR_AUTO_OPENING_DELAY 14

#define TEMPERATURE_MEASURING_PERIOD 10
#define MAX_TEMPERATURE_READINGS 5

#define LIGHT_DEFAULT_INTENSITY 300

#define MAX_RETRANSMISSIONS 5

#define DEBUG 0

#endif //NESPROJECT_CONSTANTS_H
