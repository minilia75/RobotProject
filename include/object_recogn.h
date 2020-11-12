/*
 * object_recogn.h
 *
 *  Created on: 26 nov. 2019
 *      Author: gael
 */



#ifndef OBJECT_RECOGNITION_OBJECT_RECOGN_H_
#define OBJECT_RECOGNITION_OBJECT_RECOGN_H_

// #include "all_libraries.h"

#define MAX_LENGTH 200
#define DATA_POINTS 115
#define BACKGROUND_THRESHOLD 200
#define EDGE_THRESHOLD 500
#define ERROR_MARGIN 50
#define MARGIN_NEEDS_REDO 60
#define BALL_WIDTH (DATA_POINTS/3)

typedef enum objects{
    CUBE=1, PYR4, PYR3, PYR4_U, PYR3_U, CYL
} Object;

Object most_common(Object results[], int n_tests);

float* object_acquisition(int speed); // Maybe take care because it is a chart and not a unique float value

float mean_square_error(float* shape, float* data);

//float * trim_far_points(float data[]);

float min_value(float* results);


void print_data(char label[], float* test);

//void zero_mean(float shape[], float data[]);



//Return indexx of edge, if none found returns -1
int find_edge(float* data);

void move_to_front(int edge_index, int motor1, int motor2, int speed, float* orientation_move_to_front, float* distance_to_object_second_measurement);

void classify(float* data);

int is_cube(float* data);

int is_ball(float* data);

float* null_invalid(float* data);

int check_reverse_pyramid(float* data);

int check_pyramid_or_cylinder(float* data);



Object object_scan(uint8_t* sn_B, uint8_t *sn_D,float* obstacle_orientation_first_scan, float* distance_to_obstacle_first_scan );

int init_sensor_motors(uint8_t* sn_B, uint8_t* sn_D, float* obstacle_orientation_first_scan, float* distance_to_obstacle_first_scan,int side);
void move_back_to_initial_position(int was_front_since_the_beginning, int motor1, int motor2, int speed);





#endif /* OBJECT_RECOGNITION_OBJECT_RECOGN_H_ */
