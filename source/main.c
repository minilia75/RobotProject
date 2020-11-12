/*
 * main.c
 */
 
#include "all_libraries.h"
#include "config.h"


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
	uint8_t x_coordinates;
	uint8_t y_coordinates;

	printf( "*** ( EV3 ) Hello TEST RECOGN! ***\n" );
	float last_compass_before_scan_move_to_front;
	printf("BEGINNING OF TEST PROGRAM FOR OBJECT_RECOGNITION AND ACQUISITION\n");

	assignSharedI(&play, 0);

	pthread_t send_thread;
	pthread_create(&send_thread, NULL, ServerSendThread, NULL);

	pthread_t rec_thread;
	pthread_create(&rec_thread, NULL, ServerRecThread, NULL);

	// wait for START
	while(readSharedI(&play) == 0){
		printf("waiting for play\n");
		sleep(1);
	}
	printf("play\n");


	//First right Quadran
	side =1;
	init_sensor_motors(&sn_B, &sn_D, &orientation, &distance, side); // = first scan and then goes to the object
	printf("test3\n");
	if (orientation!=-1) {
		objectPosition(&orientation, &distance, &x_coordinates, &y_coordinates);
		Object result_first_right = object_scan(&sn_B, &sn_D,&distance,&orientation); // Scan of the first object
		Message* new_message = OBJ_ID_message(result_first_right, x_coordinates+60, y_coordinates-10);
		print_message(new_message);
	}
	else { //Situation when no ball along the way
	    
		go_and_comeback_right(sn_B, sn_D, MAX_SPEED/4);
		
	
	}

	//First left Quadran
	side =0;
	init_sensor_motors(&sn_B, &sn_D, &orientation, &distance, side); // = first scan and then goes to the object
	if (orientation !=-1) {
	objectPosition(&orientation, &distance, &x_coordinates, &y_coordinates);
	Object result_first_left = object_scan(&sn_B, &sn_D,&distance,&orientation); // Scan of the first object
	Message* new_message2 = OBJ_ID_message(result_first_left, x_coordinates+60, y_coordinates-10);
	print_message(new_message2);
	}
	else {
		go_and_comeback_left(sn_B, sn_D, MAX_SPEED/4);
	}

	//Then goes to the second part of the quadrans
	move_forward_cm(sn_B, sn_D, 30, MAX_SPEED/3);

	//Second right Cadran
	side =1;
	init_sensor_motors(&sn_B, &sn_D, &orientation, &distance, side); // = first scan and then goes to the object
	objectPosition(&orientation, &distance, &x_coordinates, &y_coordinates);
	Object result_second_right = object_scan(&sn_B, &sn_D,&distance,&orientation); // Scan of the first object
	Message* new_message3 = OBJ_ID_message(result_second_right, x_coordinates+60, y_coordinates-10);
	print_message(new_message3);

	side =0;
	init_sensor_motors(&sn_B, &sn_D, &orientation, &distance, side); // = first scan and then goes to the object
	objectPosition(&orientation, &distance, &x_coordinates, &y_coordinates);
	Object result_second_left = object_scan(&sn_B, &sn_D,&distance,&orientation); // Scan of the first object
	Message* new_message4 = OBJ_ID_message(result_second_left, x_coordinates+60, y_coordinates-10);
	print_message(new_message4);


	//In all cases, then balls to the other side in the end
	go_and_comeback_right(sn_B, sn_D, MAX_SPEED/4);
	go_and_comeback_left(sn_B, sn_D, MAX_SPEED/4);
	
	return 0;
}
