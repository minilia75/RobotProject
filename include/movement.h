#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "ev3.h"
#include "ev3_port.h"
#include "ev3_tacho.h"
#include "ev3_sensor.h"

#include <unistd.h>
#include <math.h>
#include "config.h"

#ifndef Sleep
#define Sleep( msec ) usleep(( msec ) * 1000 ) 
#endif

extern void wait_motor_stop(uint8_t motor);

extern void move_forward_cm(int motor1, int motor2, int cm, int speed);

extern void move_backward_cm(int motor1, int motor2, int cm, int speed);

extern void initGyro();

extern float get_value_samples(float *value, int samples);

extern void get_compass_value(float *value);

extern void turn(int motor1, int motor2, int speed, int deg);

extern void turn_precision(int motor1, int motor2, int speed, int deg);

extern void turn_left(int motor1, int motor2, int speed, int deg);

extern void turn_right(int motor1, int motor2, int speed, int deg);

extern void turn_right_precision(int motor1, int motor2, int speed, int deg);

extern void turn_right_precision_wmotors(int motor1, int motor2, int speed, int deg);

extern void turn_left_precision(int motor1, int motor2, int speed, int deg);

#endif //MOVEMENT.H
