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
	* Return distance in cm (double)
	*/
	double Distance();
	
	/**
	* Do an average of count Distance measurements
	* remove min and max values for noise reduction
	*/
	double DistanceAvg(int wait=DEFAULT_DELAY, int count=DEFAULT_PINGS);
	
	

private:
	double MicrosecondsToCentimeter(long duration);
	
	double _currentDistance;
	int _echoPin, _triggerPin;
	long _duration, _distance;
	bool _autoMode;
};
#endif
