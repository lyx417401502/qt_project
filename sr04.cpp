#include "sr04.h"
#include "wiringPi.h"
#include <sys/time.h>

SR04::SR04()
{

}

void SR04::pininit()
{
    wiringPiSetup();
    pinMode(Trig, OUTPUT);
    pinMode(Echo, INPUT);
    digitalWrite(Trig, LOW);

}

void SR04::read_distance()
{
    digitalWrite(Trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(Trig, LOW);

    while(digitalRead(Echo) == 0);
    gettimeofday(&time_1, nullptr);

    while(digitalRead(Echo) == 1);
    gettimeofday(&time_2, nullptr);

    t1 = time_1.tv_sec * 1000000 + time_1.tv_usec;
    t2 = time_2.tv_sec * 1000000 + time_2.tv_usec;

    dist = static_cast<float>(t2 - t1) / 1000000 * 34000 / 2;
}
