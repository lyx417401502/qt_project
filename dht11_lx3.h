#ifndef DHT11_LX3_H
#define DHT11_LX3_H

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>


#define uchar   unsigned char
#define dht11_pin 7
class dht11_lx3
{
public:
    dht11_lx3();
//    ~dht11_lx3();
    void pininit();
    void dht11_reset(void);
    uint dht11_read_bit(void);
    uint dht11_read_byte(void);
    uint dht11_read_data(uchar buffer[5]);

};

#endif // DHT11_LX3_H
