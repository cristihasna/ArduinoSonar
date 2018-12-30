#ifndef SR04_H
#define SR04_H

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <inttypes.h>

#define PULSE_TIMEOUT 11764L	// so that maximum distance is 200cm
#define DEFAULT_DELAY 10
#define DEFAULT_PINGS 5
#define MAX_RANGE 200

class SR04 {
public:
	SR04(int echoPin, int triggerPin);

	/*
	* Do a measurment for this sensor. Return distance as long in centimenter
	*/
	long Distance();
	
	/**
	* Do count measurents and calculate the average. 
	* To avoid defilement from ow/high peaks, min/max values are substracted from the average
	* wait - delay between measurements, default = DEFAULT_DELAY/ms
	* count - number of measurements, default DEFAULT_PINGS
	* return long distance in centimeter
	*/
	long DistanceAvg(int wait=DEFAULT_DELAY, int count=DEFAULT_PINGS);
	
	/**
	* Do only a ping. Get result with methode getDistance()
	*/
	void Ping() ;
	
	/**
	* return latest distance in cm. Methode Ping() should be called before
	*/
	long getDistance();
	

private:
	long MicrosecondsToCentimeter(long duration);
	
	long _currentDistance;
	int _echoPin, _triggerPin;
	long _duration, _distance;
	bool _autoMode;
};
#endif
