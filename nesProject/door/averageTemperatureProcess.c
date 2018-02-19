#include "averageTemperatureProcess.h"

#include <stdio.h>
#include <stdlib.h>
#include "random.h"
#include "clock.h"
#include "contiki.h"
#include "dev/sht11/sht11-sensor.h"

#include "commons/constants.h"

double averageTemperature;

double average(double v[], int size)
{
	double sum = 0;
	int i;
	for (i = 0; i < size; ++i)
	{
		sum += v[i];
	}
	return sum/size;
}

PROCESS(averageTemperatureProcess, "Temperature measuring process");

PROCESS_THREAD(averageTemperatureProcess, ev, data)
{
	static struct etimer temperatureTimer;
	static double temperatureReadings[MAX_TEMPERATURE_READINGS];
	static int lastTemperatureIndex = 0;
	static int didIndexWrapAround = 0;
	
	PROCESS_BEGIN();
		
		etimer_set(&temperatureTimer, TEMPERATURE_MEASURING_PERIOD * CLOCK_SECOND);
		random_init(clock_time());
		
		while(1)
		{
			PROCESS_WAIT_UNTIL(ev == PROCESS_EVENT_TIMER && etimer_expired(&temperatureTimer));
			
			SENSORS_ACTIVATE(sht11_sensor);
				int rawTemperatureReading = sht11_sensor.value(SHT11_SENSOR_TEMP);
			SENSORS_DEACTIVATE(sht11_sensor);
			
			// Sht11 header file tells us that this is the conversion formula
			// Temperature in Celsius (t in 14 bits resolution at 3 Volts)
			// T = -39.60 + 0.01*t
			
			double measuredTemperature = -39.60 + 0.01*rawTemperatureReading;
			//printf("Temp measured %d\n", (int)measuredTemperature);

    		double randomVariation = (double)random_rand()/RANDOM_RAND_MAX*2.0-1.0;//float in range -1 to 1
			randomVariation *= MAX_TEMP_VARIATION;
			measuredTemperature += randomVariation;
			
			temperatureReadings[lastTemperatureIndex] = measuredTemperature;
			lastTemperatureIndex = (lastTemperatureIndex + 1) % MAX_TEMPERATURE_READINGS;
			
			if(lastTemperatureIndex == 0)
				didIndexWrapAround = 1;
			
			if(didIndexWrapAround)
				averageTemperature = average(temperatureReadings, MAX_TEMPERATURE_READINGS);
			else
				averageTemperature = average(temperatureReadings, lastTemperatureIndex);
			
			etimer_reset(&temperatureTimer);
			
			//printf("Measured temperature: %d ; Average temperature: %d \n",
			//	   (int) measuredTemperature,
			//	   (int) averageTemperature);
		}
	
	PROCESS_END();
}
