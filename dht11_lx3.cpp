#include "dht11_lx3.h"
#include "mainwindow.h"

#include "ui_mainwindow.h"
#include "wiringPi.h"



dht11_lx3::dht11_lx3()
{

}


void dht11_lx3::pininit()
{
    wiringPiSetup();
    pinMode(dht11_pin,OUTPUT);
    digitalWrite( dht11_pin, HIGH );
}

void dht11_lx3::dht11_reset(void)
{
    digitalWrite( dht11_pin, LOW );
    delay(20);
    digitalWrite( dht11_pin, HIGH );
    delayMicroseconds(30);
    pinMode(dht11_pin,INPUT);
    pullUpDnControl(dht11_pin,PUD_UP);

}


uint dht11_lx3::dht11_read_bit(void)
{
    int count1=0;
    while (digitalRead( dht11_pin ) == 0);
    delayMicroseconds(35);
    if (digitalRead( dht11_pin ) == 1)
    {
        //        while (digitalRead( dht11_pin ) == 1);
        while (digitalRead( dht11_pin ) == 1)
        {
            delayMicroseconds(1);
            count1++;
            if(count1>50)
                break;
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

uint dht11_lx3::dht11_read_byte(void)
{
    uint i;
    uint data = 0;
    for (i = 0; i < 8; i++)
    {
        data <<= 1;
        data |= dht11_read_bit();
    }
    return data;
}

uint dht11_lx3::dht11_read_data(uchar buffer[5])
{
    uint i = 0;
    uchar checksum;
    dht11_reset();
    if (digitalRead( dht11_pin ) == 0)
    {
        while (digitalRead( dht11_pin ) == 0);
        while (digitalRead( dht11_pin ) == 1);
        for (i = 0; i < 5; i++)
        {
            buffer[i] = dht11_read_byte();
        }
        while (digitalRead( dht11_pin ) == 0);
        pinMode( dht11_pin, OUTPUT );
        digitalWrite( dht11_pin, HIGH );

        checksum = buffer[0] + buffer[1] + buffer[2] + buffer[3];
        if (checksum != buffer[4])
        {
            // checksum error
            return 1;
        }
    }

    return 0;
}
