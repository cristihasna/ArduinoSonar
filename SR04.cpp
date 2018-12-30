
#include "SR04.h"

SR04::SR04(int echoPin, int triggerPin) {
    _echoPin = echoPin;
    _triggerPin = triggerPin;
    pinMode(_echoPin, INPUT);
    pinMode(_triggerPin, OUTPUT);
    _autoMode = false;
    _distance = 999;
}


double SR04::Distance() {
    double d = 0;
    _duration = 0;
    digitalWrite(_triggerPin, LOW);
    delayMicroseconds(2);
    digitalWrite(_triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(_triggerPin, LOW);
    delayMicroseconds(2);
    _duration = pulseIn(_echoPin, HIGH, PULSE_TIMEOUT);
    d = MicrosecondsToCentimeter(_duration);
    delay(5);
    return d;
}

double SR04::DistanceAvg(int wait, int count) {
    double min, max, avg, d;
    min = 999;
    max = 0;
    avg = d = 0;

    if (wait < 25) {
        wait = 25;
    }

    if (count < 1) {
        count = 1;
    }

    for (int x = 0; x < count + 2; x++) {
        d = Distance();

        if (d < min) {
            min = d;
        }

        if (d > max) {
            max = d;
        }

        avg += d;
    }

    // substract highest and lowest value
    avg -= (max + min);
    // calculate average
    avg /= count;
    return avg;
}

void SR04::Ping() {
    _distance = Distance();
}

double SR04::getDistance() {
    return _distance;
}

double SR04::MicrosecondsToCentimeter(long duration) {
    double d = (duration * 100) / 5882.0;
    if (d == 0 || d > MAX_RANGE) d = MAX_RANGE;
    return d;
}
