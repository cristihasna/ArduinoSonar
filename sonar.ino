#include <AccelStepper.h>
#include "SR04.h"
#include "strings.h"

#define TRIG_PIN 7
#define ECHO_PIN 6

#define MOTOR_PIN_1 8
#define MOTOR_PIN_2 9
#define MOTOR_PIN_3 10
#define MOTOR_PIN_4 11

#define BUZZER 3
#define TONE 988

#define TIMER 3500
#define BUZZ_TIME 50

#define DEG_PER_STEP 0.087890625 //degrees per step
#define STEPS_PER_REV 4096		 //number of steps for a full revolution of 28BYJ-48 stepper
#define ROT_SPEED 1000			 //120 rpm

int buzzer = 1;
int rotating = 1;
int prevRotating = 1;
unsigned long last_time = 0;

double dist;
float angle;
float target;

SR04 sr04(ECHO_PIN, TRIG_PIN);
AccelStepper stepper(AccelStepper::HALF4WIRE, MOTOR_PIN_1, MOTOR_PIN_3, MOTOR_PIN_2, MOTOR_PIN_4);

void rotateTo(float deg)
{
	float target = deg / DEG_PER_STEP;
	stepper.setSpeed(ROT_SPEED);
	stepper.setAcceleration(ROT_SPEED);
	stepper.moveTo(target);
	rotating = 2;
	buzzer = buzzer == 2 ? 1 : 0;
}

void continueRotating()
{
	rotating = prevRotating;
	buzzer = buzzer == 2 ? 1 : 0;
}

void pauseRotating()
{
	prevRotating = rotating;
	if (buzzer != 0)
		buzzer = 2;
	rotating = 0;
}

void startBuzzer()
{
	if (rotating)
		buzzer = 1;
}

void stopBuzzer()
{
	buzzer = 0;
}

/**
* commands are of type <<CommandType>>[|<<arg>>] sent by the python application
**/
void processCommand()
{
	char *command = (char *)malloc(16);
	serialRead(command, 16);
	char commandType = command[0];

	if (commandType == 'P')
	{
		//pause rotating
		pauseRotating();
	}
	else if (commandType == 'C')
	{
		//continue rotating;
		continueRotating();
	}
	else if (commandType == 'B')
	{
		//start buzzer
		startBuzzer();
	}
	else if (commandType == 'S')
	{
		//stop buzzer
		stopBuzzer();
	}
	else if (commandType == 'R')
	{
		char *arg = (char *)malloc(16);
		getword(arg, 16, command, (char *)"|", 1);
		int newAngle = atoi(arg);
		rotateTo(newAngle);
		free(arg);
	}
	sendValues();
}

void sendValues()
{
	dist = sr04.Distance();
	int pos = stepper.currentPosition();
	angle = pos * DEG_PER_STEP;
	target = stepper.targetPosition() * DEG_PER_STEP;
	stepper.run();
	Serial.print(angle);
	Serial.print("|");
	Serial.print(dist);
	stepper.run();
	Serial.print("|");
	Serial.print(target);
	Serial.print("|");
	Serial.println(rotating);
	stepper.run();
}

void setup()
{
	Serial.begin(9600);
	stepper.setSpeed(ROT_SPEED);
	stepper.setAcceleration(ROT_SPEED);
	stepper.moveTo(STEPS_PER_REV);
}

void loop()
{
	if (Serial.available() > 0)
		processCommand();

	unsigned long new_time = millis();
	if (buzzer == 1 && new_time - last_time > TIMER)
	{
		last_time = new_time;
		noTone(BUZZER);
		tone(BUZZER, TONE, BUZZ_TIME);
	}

	if (rotating)
	{
		stepper.run();
		if (!stepper.isRunning())
		{
			if (rotating == 2)
			{
				rotating = 0;
				buzzer = 2;
			}
			else if (stepper.currentPosition() == 0)
			{

				stepper.setSpeed(ROT_SPEED);
				stepper.setAcceleration(ROT_SPEED);
				stepper.moveTo(STEPS_PER_REV);
			}
			else
			{

				stepper.setSpeed(ROT_SPEED);
				stepper.setAcceleration(ROT_SPEED);
				stepper.moveTo(0);
			}
		}
		sendValues();
	}
}