#ifndef SR04_H
#define SR04_H

#define Trig 4
#define Echo 5

#include <QTimer>




class SR04
{
public:
    SR04();
    void pininit();
    void read_distance();
    struct timeval time_1;
    struct timeval time_2;
    long t1, t2;
    float dist = 0.;
};

#endif // SR04_H
