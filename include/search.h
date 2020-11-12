
#ifndef SEARCH_H
#define SEARCH_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "ev3.h"
#include "ev3_port.h"
#include "ev3_tacho.h"
#include "ev3_sensor.h"
#include "movement.h"

#include <unistd.h>
#include <math.h>
#include "config.h"

#ifndef Sleep
#define Sleep( msec ) usleep(( msec ) * 1000 ) 
#endif

#define STARTX 0
#define STARTY 0


uint8_t sn_gyro;
int init_gyro;

extern int random_int(int min, int max);

extern void go_to_position_to_scan(int motor1, int motor2, int speed, float begin);

extern bool indDifference(int indicemin, int i, int diff);

extern void scanSeveral(int motor1, int motor2, int speed, float begin, float end, int radius, float *results, float *distance_to_object);

extern void goUntilDistance(int motor1, int motor2, int speed, float distance_to_obstacle, float distancemin);

extern void scan(int motor1, int motor2, int speed, float end, int radius, float* result, float* distance_to_object);

extern void scanMiddle(int motor1, int motor2, int speed, float end, int radius, float *results, float *distance_to_object);

extern void allScan(int motor1, int motor2, int speed, int radius, float *results, float *distance_to_object, int side);

#endif //SEARCH.H
