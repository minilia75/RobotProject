/*
 * test_recogn.c
 *
 *  Created on: 8 janv. 2020
 *      Author: gael
 */
#include "all_libraries.h"
#include "config.h"
#include "../include/object_recogn.h"

// Why is this needed?
#include "object_recogn.c"
#include "movement.c"
#include "SharedData.c"
#include "sensor_thread.c"
//#include "server_thread.c"
#include "gyro_thread.c"
#include "messages.c"
#include "search.c"
#include "launcher.c"
//#include "server_send_thread.c"
//#include "server_receive_thread.c"


/**
 * Main Function of the code
 * The one we launched the day of the final competition
 * Objective: decompose the arena in 4 quadrans, the robot will scan each one of them in order to find if there is an object in it
 * Once
 * Written by Gaël
 */
int main( void ) {

	// Threads t = init_threads();
	//pthread_t thread;
    //pthread_create(&thread, NULL, SensorThread, NULL);

	//sn = sequence number
	uint8_t sn_sonar; // For the sonar
	uint8_t sn_D; // For the motor D
	uint8_t sn_B; // For the motor B

	char s[256]; // To get some string values
	float distance;
	float orientation;
	int side; //Left Side = 0
			  // Right Side =1
	int8_t x_coordinates;
	int8_t y_coordinates;

	printf( "*** ( EV3 ) Hello TEST RECOGN! ***\n" );
	float last_compass_before_scan_move_to_front;
	printf("BEGINNING OF TEST PROGRAM FOR OBJECT_RECOGNITION AND ACQUISITION\n");

	assignSharedI(&play, 0);

	// pthread_t send_thread;
	// pthread_create(&send_thread, NULL, ServerSendThread, NULL);

	// pthread_t rec_thread;
	// pthread_create(&rec_thread, NULL, ServerRecThread, NULL);

	//wait for START
	// while(readSharedI(&play) == 0){
	// 	printf("waiting for play\n");
	// 	sleep(1);
	// }
	// printf("play\n");

	// initialize gyro thread
	initGyro();
    
	pthread_t sensor_thread;
    pthread_create(&sensor_thread, NULL, SensorThread, NULL);

	while (readSharedI(&sonar_found) == 0)
	{
		Sleep(10);
	}
	

	//First total scan
	//float orientation_to_obstacles[3];
	//float distance_to_obstacles[3];
	//first_scan(&sn_B, &sn_D, orientation_to_obstacles, distance_to_obstacles);

	//First right Quadran
	side = 1;
	init_sensor_motors(&sn_B, &sn_D, &orientation, &distance, side); // = first scan and then goes to the object
	printf("test2\n");
	printf("Orientation: %f\n",*&orientation);
	printf("Distance: %f\n",distance);
	if (orientation!=-1) {
		objectPosition(orientation, distance, &x_coordinates, &y_coordinates);
		printf("X: %d\nY: %d \n", x_coordinates, y_coordinates);
		Object result_first_right = object_scan(&sn_B, &sn_D,&distance,&orientation); // Scan of the object
		Message* new_message = OBJ_ID_message(result_first_right, x_coordinates+60, y_coordinates+10); //Construct the message accordingly
		print_message(new_message); //Here it is a print but normally it is send the message to the server
	}
	else { //Situation when no ball along the way
		//go_and_comeback_right(sn_B, sn_D, MAX_SPEED/4);
	}

	//First left Quadran
	side =0;
	init_sensor_motors(&sn_B, &sn_D, &orientation, &distance, side); // = scan and then goes to the object
	if (orientation !=-1) {
		objectPosition(orientation, distance, &x_coordinates, &y_coordinates);
		Object result_first_left = object_scan(&sn_B, &sn_D,&distance,&orientation); // Scan of the object
		Message* new_message2 = OBJ_ID_message(result_first_left, x_coordinates+60, y_coordinates-10); //Construct the message accordingly
		print_message(new_message2); //Here it is a print but normally it is send the message to the server
	}
	else { //Situation when no ball along the way
		//go_and_comeback_left(sn_B, sn_D, MAX_SPEED/4);
	}

	//Then goes to the second part of the quadrans
	move_forward_cm(sn_B, sn_D, 30, MAX_SPEED/3);

	//Second right Cadran
	side = 1;
	init_sensor_motors(&sn_B, &sn_D, &orientation, &distance, side); // = scan and then goes to the object
	objectPosition(orientation, distance, &x_coordinates, &y_coordinates);
	Object result_second_right = object_scan(&sn_B, &sn_D,&distance,&orientation); // Scan of the object
	Message* new_message3 = OBJ_ID_message(result_second_right, x_coordinates+60, y_coordinates-10); //Construct the message accordingly
	print_message(new_message3); //Here it is a print but normally it is send the message to the server

	side = 0;
	init_sensor_motors(&sn_B, &sn_D, &orientation, &distance, side); // = scan and then goes to the object
	objectPosition(orientation, distance, &x_coordinates, &y_coordinates);
	Object result_second_left = object_scan(&sn_B, &sn_D,&distance,&orientation); // Scan of the object
	Message* new_message4 = OBJ_ID_message(result_second_left, x_coordinates+60, y_coordinates-10); //Construct the message accordingly
	print_message(new_message4); //Here it is a print but normally it is send the message to the server


	//In all cases, throw balls to the other side in the end
	go_and_comeback_right(sn_B, sn_D, MAX_SPEED/4);
	go_and_comeback_left(sn_B, sn_D, MAX_SPEED/4);

	return 0;
}	

