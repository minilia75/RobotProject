//#define LOCAL_TESTING

#ifdef LOCAL_TESTING
#include "../include/object_recogn.h"
#include <stdio.h>
#endif

#ifndef	LOCAL_TESTING
#include "../include/all_libraries.h"
#endif

/**
 *
 * @param shape
 * @param data
 * @return
 */
float mean_square_error(float* shape, float* data){
    float sum;
	int valid = 0;
	int i;
	for (i = 0; i < DATA_POINTS; i++){
		if (data[i] != 0){
			sum += (shape[i] - data[i])*(shape[i] - data[i]);
			valid++;
		}
	}
	if(!valid){
		return -1;
	} 
    return sum / valid;
}
/**
 *
 * @param results
 * @return
 */
float min_value(float* results){
	int min_index = 0;
	float min_value = results[0];

	int i;
	while(results[i] != '\0'){
		if (results[i] < min_value){
			min_index = i;
			min_value = results[i];
		}
		i++;
	}
	return min_value;
}

//void transpos_mean(float shape[], float data[]){
/**
 *
 * @param data
 * @return
 */
int find_edge(float* data){
	int i;
	int edges[DATA_POINTS];
	int object_detected = 0;
	int n_edges_detected = 0;
	int edge_index;
	int n_large_frames;

	for (i = 0; i < DATA_POINTS; i++){
		if (data[i] > BACKGROUND_THRESHOLD){
			n_large_frames++;
		}
	}

	if (n_large_frames > floor(DATA_POINTS/2)){
		printf("DATA_CORRUPTED: number of n_large_frames: %d\n",n_large_frames);
		n_large_frames=0;
		return DATA_POINTS/2;
	}
	printf("Number of Large_frames: %d\n",n_large_frames);
	n_large_frames=0;

	for (i = 0; i < DATA_POINTS; i++){
		if(!object_detected){					//Ignore leading High values
			if (data[i] > BACKGROUND_THRESHOLD && data[i] < EDGE_THRESHOLD) {
				edges[i] = 0;
			} else {
				object_detected = 1;
				edges[i] = 0;			}
		} else {
			if (data[i] > BACKGROUND_THRESHOLD) {
				edges[i] = 1;
				n_edges_detected++;
			} else {
				edges[i] = 0;
			}
		}
	}


	
	i = DATA_POINTS - 1;
	while (edges[i] == 1){		//Remove trailing high values, might be messy i values, test
		edges[i] = 0;
		n_edges_detected--;
		i--;
	}	

	for (i=0; i < DATA_POINTS; i++){
		printf("%d", edges[i]);
	}
	printf("\n");

	if (n_edges_detected < 1){
		return -1;
	}

	int sum_left = 0;
	int sum_right = 0;
	for(i = 0; i < DATA_POINTS/2; i++){
		sum_left += edges[i];
	}
	for(;i < DATA_POINTS; i++){
		sum_right += edges[i];
	}

	printf("Sum: %d\n", sum_left);


	if (sum_left > sum_right){
		printf("EDGE = LEFT\n");
		i = DATA_POINTS/2;
		while(edges[i] != 1 && i >= 0){
			i--;
		}
	} else {
		printf("EDGE = RIGHT\n");
		i = DATA_POINTS/2 + 1;
		while(edges[i] != 1 && i <= DATA_POINTS){
			i++;
		}
	}
	edge_index = i;
	return i;
}

/**
 *
 * @param data
 * @return
 */
float* null_invalid(float* data){
	static float new[DATA_POINTS];
	int i;
	for (i = 0; i < DATA_POINTS; i++){
		if (data[i] > BACKGROUND_THRESHOLD){
			new[i] = 0;
		} else {
			new[i] = data[i];
		}
	}
	return new;
}
/**
 * detrmine according to the data[] we got from object_acquisition if object is a cube
 * @param data
 * @return
 */
int is_cube(float* data){
	int i;
	float slope_sum = 0.0;
	int valid_slopes = 0;
	int n_valid_points = 0;
	
	float *data_nulled = null_invalid(data);

	for (i = 0; i < DATA_POINTS - 1; i++){
		if (data_nulled[i] != 0) n_valid_points++;
		if (data_nulled[i+1] != 0 && data_nulled [i] != 0){
			float slope = data_nulled[i + 1] - data_nulled[i];
			slope_sum += slope;
			valid_slopes++;
		}
	}
	printf("Slope_sum: %2.2f, valid_slopes: %d\n", slope_sum, valid_slopes);
	float slope_mean = slope_sum / (float)valid_slopes;

	if (n_valid_points < DATA_POINTS/2){
		return 0;
	}

	float valid_point;
	int valid_index;

	i = 0;
	for(;;){
		if(data_nulled[i] == 0){
			i++;
		} else {
			valid_point = data_nulled[i];
			valid_index = i;
			break;
		}
	}

	float shape[DATA_POINTS];
	printf("Valid_point: %2.2f, Valid_index: %d, Slope: %2.2f\n", valid_point, valid_index, slope_mean);
	for (i = 0; i < DATA_POINTS; i++){
		shape[i] = valid_point + (i - valid_index)*slope_mean;
	}

	 
	print_data("Shape: ", shape);
	print_data("Data: ", data_nulled);

	float error = mean_square_error(shape, data_nulled);

	printf("Error: %2.2f\n", error);
	printf("Error_margin: %d\n", ERROR_MARGIN);

	if (error < ERROR_MARGIN) {
		return 1; //Is a cube
	}
	else if (error < MARGIN_NEEDS_REDO) {
		return 2; // Used in the beginning but in the end because of the 3 measurements not needed anymore
	}
	return 0; //Is not a cube
}
/**
 * detrmine according to the data[] we got from object_acquisition if object is a reverse pyramid
 * In the beginning, we thought we could be more precise by checking if values increased before min and decreased after
 * But in the end sonar not precise so not used the code in "comentary" below the function
 * @param data -> all points we had from sonar value
 * written by Gaël
 */
int check_reverse_pyramid(float* data) { // Return 1 if it is a Reverse Pyramid and 0 if not
	int local_compteur;
	int local_compteur1;
	int local_compteur2;
	int position_max_value=0;
	int nb_out_of_range=0;
	float interval_validity=2.0;
	float current_value;
	int number_of_time_current_value_maybe_new_max=0;
	float old_value=data[0];

	float* data_null = null_invalid(data);

	//Get the position of the min value (before floats increases and after floats decreases)
	for (local_compteur=floor(DATA_POINTS/3); local_compteur<floor((2*DATA_POINTS)/3); local_compteur++ ) {
		if (data_null[local_compteur] >= data_null[position_max_value]) {
			//Here check the number of time of this maximum value
			for (local_compteur1=floor(DATA_POINTS/3); local_compteur1<floor((2*DATA_POINTS)/3); local_compteur1++) {
				if (data[local_compteur]-interval_validity <= data[local_compteur1] && data[local_compteur1] <= data[local_compteur]+interval_validity ) {
					number_of_time_current_value_maybe_new_max+=1;
				}
			}
			if (number_of_time_current_value_maybe_new_max>=floor(DATA_POINTS/20)) {
				if (data[local_compteur]<=BACKGROUND_THRESHOLD) {
					position_max_value = local_compteur;
				}
				number_of_time_current_value_maybe_new_max=0;
			}
			else {
				number_of_time_current_value_maybe_new_max=0;
			}
		}
	}
	printf("Position_max_value: %d\n",position_max_value);
	printf("Value associated: %f\n",data_null[position_max_value]);
	for (local_compteur=0;local_compteur<DATA_POINTS;local_compteur++) {
		if (data_null[local_compteur]==0) {
			nb_out_of_range+=1;
		}
	}
	if (nb_out_of_range>=floor(DATA_POINTS/2)) {
		return 0;
	}
	if (position_max_value==0) {
		return 0; //Not a reversed pyramid
	}
	else {
		return 1; //Reverse pyramid
	}

	// First let's check the distance increasing part
	/*for (local_compteur1=1; local_compteur1 < position_max_value;local_compteur1 ++) {
		current_value=data_null[local_compteur1];

		//Test if value nok with old one (out of interval)
		if (current_value+interval_validity<old_value) {
			nb_out_of_range+=1;
			old_value=data_null[local_compteur-1]; // Take the previous in range value
			interval_validity+=interval_validity;
		}
		else {
			// Value was ok and respected the Pyramid scheme
			old_value=data_null[local_compteur];
			interval_validity=3.0;
		}
	}
	// Then lest's check about the distance decreasing part
	old_value=data_null[position_max_value];
	for (local_compteur2=1; local_compteur2<DATA_POINTS-position_max_value; local_compteur2++) {
		current_value=data_null[position_max_value+local_compteur2];
		//Test if value nok with old one (out of interval)
		if (current_value-interval_validity>old_value) {
			nb_out_of_range+=1;
			old_value=data_null[local_compteur-1]; // Take the previous in range value
			interval_validity+=interval_validity;
		}
		else {
			// Value was ok and respected the Pyramid scheme
			old_value=data_null[local_compteur];
			interval_validity=3.0;
		}
	}
	printf("Total Number of values Out of Range %d\n", nb_out_of_range);
	if (nb_out_of_range<=DATA_POINTS/10) { // Maybe change this value later according to the tests
		return 1; // Reversed-Pyramid
	}
	else {
		return 0; // Not a reversed Pyramid
	}*/
}
/**
 * detrmine according to the data[] we got from object_acquisition if object is a pyramid or a cylinder
 * @param data -> all points we had from sonar value
 * written by Gaël
 */
int check_pyramid_or_cylinder(float* data) { // Return 1 if it is a pyramid and 0 if it is not

	int local_compteur;
	int local_compteur1;
	int position_min_value=0;
	int nb_of_min_value=0;
	int number_of_time_current_value_maybe_new_min=0;
	float interval_validity=2.0;

	for (local_compteur=0; local_compteur<DATA_POINTS; local_compteur++) {
		if (data[local_compteur] <= data[position_min_value]) {
			for (local_compteur1=0; local_compteur1<DATA_POINTS; local_compteur1++) {
				if (data[local_compteur]-interval_validity <= data[local_compteur1] && data[local_compteur1] <= data[local_compteur]+interval_validity ) {
					number_of_time_current_value_maybe_new_min+=1;
				}
			}
			if (number_of_time_current_value_maybe_new_min>=floor(DATA_POINTS/20)) {
				position_min_value = local_compteur;
				number_of_time_current_value_maybe_new_min=0;
			}
			else {
				number_of_time_current_value_maybe_new_min=0;
			}
		}
	}
	printf("Min_value = %f\n",data[position_min_value]);

	for (local_compteur1=0; local_compteur1<DATA_POINTS; local_compteur1++) {
		if (data[position_min_value]-interval_validity <= data[local_compteur1] && data[local_compteur1] <= data[position_min_value]+interval_validity ) {
			nb_of_min_value+=1;
		}
	}
	printf("Number_of_min_value %d\n",nb_of_min_value);
	if (nb_of_min_value>=floor(DATA_POINTS/4)) { // For the moment arbitrary value of 4 -> We will have to change it later if needed
			return 1; // This is pyramid
		}
	else {
			return 2; // This is a cylinder
	}
}

/**
 * In the beginning, wanted to check also if object could be a ball, in czse a ball from the other side came to our side
 * Didn't use it in the end
 * @param data -> all points we had from sonar value
 * Written by Andreas
 */

int is_ball(float* data) {

	int edge = find_edge(data);
	data = null_invalid(data);
	int width = 0;
	int i;
	for (i = 0; i < DATA_POINTS; i++){
		if (data[i] != 0) width ++;
	}
	if (width <= BALL_WIDTH && edge == 0){
		return 1;
	}
	return 0;
}

/**
 * Method to "regroup" the results of the diffferent acquisiton and then determine which one is the most common one
 * @param1 results[] -> array with all objects we got from the acquisitons
 * @param2 n_tests -> number of tests we did for the acquisition part
 * Written by Andreas
 */
Object most_common(Object results[], int n_tests){
	int obj_count[6] = {0,0,0,0,0,0};
	int object_type_in_results;
	int i;
	for (i= 0; i < n_tests; i++){
		object_type_in_results=results[i];
		obj_count[object_type_in_results-1]+=1;
	}
	for (i = 0; i < 6; i++){
	}
	int max_i;
	int max_val=0;
	for (i = 0; i < 6; i++){
		if (obj_count[i] >= max_val){
			max_val = obj_count[i];
			max_i = i;
		}
	}
	return max_i+1;
}


#ifndef LOCAL_TESTING
/**
 * Function object acquision -> Makes the whole U-move with the sonar so that we can determine which kind of object we have in front of us
 * @param speed -> speed of the movement
 * Written by Gaël
 */
float* object_acquisition(int speed){

	printf("BEGINNING OF IDENTIFYING OBJECT\n ");
	int port_sonar_motor;
	port_sonar_motor = OUTPUT_A;
	uint8_t sn_A; // For the motor A
	//int max_speed;
	uint8_t sn_sonar_recogn; // For the sonar
	int steps;
	int compteur0;
	int compteur1;
	int compteur2;
	float sonar_local_value;
	float* sonar_values;
	sonar_values=malloc(sizeof(float)* DATA_POINTS);

	ev3_sensor_init(); // Detect connected sensors (-1) if none
	while ( ev3_tacho_init() < 1 ) Sleep( 1000 ); // Detect nb of connected tachos (<1 => none connected)
	printf(" POSITIONNING THE SENSOR IN ORDER TO CAPTURE VALUES\n");

	for (compteur0=0; compteur0<52; compteur0++) {
		if ( ev3_search_tacho_plugged_in(port_sonar_motor,0, &sn_A, 0 )) { // Check if tacho well plugged in Port A
			set_tacho_speed_sp( sn_A, speed ); // Decrease the speed a little to see difference
			set_tacho_ramp_up_sp( sn_A, 0 );
			set_tacho_ramp_down_sp( sn_A, 0 ); // He does not do a climb in power -> instant maxspeed
			set_tacho_position_sp( sn_A, +3 ); //Little Movement (-3 to go back to previous position)
			//Launch Command
			set_tacho_command_inx( sn_A, TACHO_RUN_TO_REL_POS );
		}
	}
	Sleep(1000);

	for (steps=0; steps<DATA_POINTS; steps++) {
		// Going to get Sonar value and then move

		sonar_values[steps]=readSharedF(&sonar_shared_value);
		//printf( "Stored value: \r(%f) \n", sonar_values[steps]);

		//We got the sonar value so now we move the motor
		if ( ev3_search_tacho_plugged_in(port_sonar_motor,0, &sn_A, 0 )) { // Check if tacho well plugged in Port A
			set_tacho_speed_sp( sn_A, speed ); // Decrease the speed a little to see difference
			set_tacho_ramp_up_sp( sn_A, 0 );
			set_tacho_ramp_down_sp( sn_A, 0 ); // He does not do a climb in power -> instant maxspeed
			set_tacho_position_sp( sn_A, -3 );
			//Launch Command
			set_tacho_command_inx( sn_A, TACHO_RUN_TO_REL_POS );
		}
	}
	Sleep(1000); // Wait till capture part is complete
	// If we don't put this Sleep the below part will also executes Parallely to above part!
	// Once the motor is at the opposite  side, we just move him back to the beginning position.
	for (compteur1=0; compteur1<56; compteur1++) {
		if ( ev3_search_tacho_plugged_in(port_sonar_motor,0, &sn_A, 0 )) { // Check if tacho well plugged in Port A
			set_tacho_speed_sp( sn_A, speed ); // Decrease the speed a little to see difference
			set_tacho_ramp_up_sp( sn_A, 0 );
			set_tacho_ramp_down_sp( sn_A, 0 ); // He does not do a climb in power -> instant maxspeed
			set_tacho_position_sp( sn_A, +3 ); //Little Movement (-3 to go back to previous position)
			//Launch Command
			set_tacho_command_inx( sn_A, TACHO_RUN_TO_REL_POS );
		}
	}
	// Now that we got a table whith all the values in it, create algorithm to identify object
	// First step, print the values to get a "pattern"

	for (compteur2=0; compteur2<DATA_POINTS; compteur2++) {
		//printf(" Value in the tab: %f\n", sonar_values[compteur2]);
	}
	return sonar_values;

	printf("END OF THE OBJECT_ACQUISITION \n");
}
/**
 * Objective of this function: if we are not front to the object, movement in order to be in front of it
 * @param edge_index -> Used in the beginning to move according to the edge, but not precise so we dropped it
 * @param motor1
 * @param motor2
 * @param speed
 * @param orientation_move_to_front
 * @param distance_to_object_second_measurement
 * These 2 last parameters were used initially to get in memory the new distance and orientation, in order then to go back to the initial position
 * But not precise anymore  movements so dropped this
 * Written by Gaël
 */
void move_to_front(int edge_index, int motor1, int motor2, int speed, float* orientation_move_to_front,float* distance_to_object_second_measurement) {
	float obstacle_orientation;
    float orientation_robot_value;
	printf("GET EDGE INDEX: %d\n",edge_index);
	ev3_tacho_init();
	int size_interval_values=floor(DATA_POINTS/5);
	printf("size_interval_values= %d\n",size_interval_values);
	get_compass_value(&orientation_robot_value);

	if (orientation_robot_value<=0) { // Values determined with the help of the Gyrp
		printf("Middle_Object_Left\n");
		printf("ORIENTATION: %f\n", orientation_robot_value);
		move_backward_cm(motor1, motor2, 3, speed); //3 = number of cm
		turn_right_precision(motor1, motor2,speed, 180); // 180 = Angle of movement
		move_backward_cm(motor1, motor2, 20, speed);
		get_compass_value(orientation_move_to_front);
		go_to_position_to_scan(motor1, motor2, speed, 120);
		scanMiddle(motor1, motor2, speed, 90, 300, &obstacle_orientation,distance_to_object_second_measurement);
	}

	else if (orientation_robot_value>0) { // Values determined with the help of the Gyrp
		printf("Middle_Object_Right\n");
		printf("ORIENTATION: %f\n", orientation_robot_value);
		move_backward_cm(motor1, motor2, 3, speed); //3 = number of cm
		turn_left_precision(motor1, motor2,speed, 150); // 150 = Angle of movement
		move_backward_cm(motor1, motor2, 20, speed);
		get_compass_value(orientation_move_to_front);
		scanMiddle(motor1, motor2, speed, 120, 300, &obstacle_orientation,distance_to_object_second_measurement);
	}

}

/**
 * Functions which allows the robot to return to an approximate initial position
 * Since the movement values were not precise at all, we dropped the idea of doing exactly in the opposite of the movement we did before
 * You can find this first idea in "commentary" in the below code
 * New idea, just go back to 0 angle (middle position of gyro)
 * And then move back to aproximately center of beginning point
 * Here I used arbitrary values defined on tests movements and can be changed if necessary
 * @param was_front_since beginning -> used initially when wanted to do reverse way
 * @param motor1
 * @param motor2
 * @param speed
 * Written by Gaël
 */
void move_back_to_initial_position(int was_front_since_the_beginning, int motor1, int motor2, int speed) {
	float current_orientation;
	printf("IN FUNCTION\n");

	get_compass_value(&current_orientation);
	while ( ev3_tacho_init() < 1 ) Sleep( 1000 ); // Detect nb of connected tachos (<1 => none connected)

	move_backward_cm(motor1, motor2, 3, speed);

	if (current_orientation<=0) { //This means that we are oriented left
		// We need to move till -90 (full left 90°) and then move back 5cm and then move till 0
		while (current_orientation > -90){
			turn_right_precision(motor1, motor2, MAX_SPEED/8, 3);
			get_compass_value(&current_orientation);
			printf("Current Value: %f\n",current_orientation);
		}
		move_backward_cm(motor1, motor2, 10, speed);
		while (current_orientation <0){
			turn_left_precision(motor1, motor2, MAX_SPEED/8, 3);
			get_compass_value(&current_orientation);
			printf("Current Value: %f\n",current_orientation);
		}
		move_backward_cm(motor1, motor2, 80, speed);
		move_forward_cm(motor1, motor2, 10, speed);
		printf("END OF MOVEMENT\n");
	}
	else if (current_orientation>=0) { //This means that we are oriented right (full right 90°)
		while (current_orientation < 90){
			turn_left_precision(motor1, motor2, MAX_SPEED/8, 3);
			get_compass_value(&current_orientation);
			printf("Current Value: %f\n",current_orientation);
		}
		move_backward_cm(motor1, motor2, 10, speed);
		while (current_orientation >0){
			turn_right_precision(motor1, motor2, MAX_SPEED/8, 3);
			get_compass_value(&current_orientation);
			printf("Current Value: %f\n",current_orientation);
		}
		move_backward_cm(motor1, motor2, 40, speed);
		move_forward_cm(motor1, motor2, 10, speed);
		printf("END OF MOVEMENT\n");
	}

	//Code used in initial idea: make exact reverse movements of the one we did before

	/*if (was_front_since_the_beginning==1) { // Then it's easy, not really much to do to go back to initial position
		printf("IN FUNCTION_1\n");
		move_backward_cm(motor1, motor2, distance_to_obstacle_first_scan, speed);
		if (*obstacle_orientation_first_scan<0) {
			while (*obstacle_orientation_first_scan < 0){
				turn_left_precision(motor1, motor2, MAX_SPEED/8, -3);
				get_compass_value(&obstacle_orientation_first_scan);
			}
			printf("Robot well positioned back to his first left state\n");
		}
		else if (obstacle_orientation_first_scan>=0) {
			while (obstacle_orientation_first_scan > 0){
				turn_right_precision(motor1, motor2, MAX_SPEED/8, -3);
				get_compass_value(&obstacle_orientation_first_scan);
			}
			printf("Robot well positioned back to his first right state\n");
		}
	}
	if (was_front_since_the_beginning==0) {// Then the robot made some movements
		printf("IN FUNCTION_2\n");
		printf("*orientation = %f", *obstacle_orientation_first_scan);
		printf("*orientation = %f", *obstacle_orientation_first_scan);
		if (*obstacle_orientation_first_scan<0) { // Then we are looking at the object from the left
			move_backward_cm(motor1, motor2, distance_to_obstacle_second_scan_if_needed, speed);
			while (*obstacle_orientation_first_scan > *last_compass_before_move_to_front){
				turn_right_precision(motor1, motor2, MAX_SPEED/8, -3);
				get_compass_value(&obstacle_orientation_first_scan);
			}
			move_forward_cm(motor1, motor2, 30, speed);
			turn_left_precision(motor1, motor2, speed, 150);
			move_forward_cm(motor1, motor2, 3, speed);
		}
		else {
			move_backward_cm(motor1, motor2, distance_to_obstacle_second_scan_if_needed, speed);
			while (*obstacle_orientation_first_scan < *last_compass_before_move_to_front){
				turn_left_precision(motor1, motor2, MAX_SPEED/8, -3);
				get_compass_value(&obstacle_orientation_first_scan);
			}
			move_forward_cm(motor1, motor2, 30, speed);
			turn_right_precision(motor1, motor2, speed, 150);
			move_forward_cm(motor1, motor2, 3, speed);
		}
		move_backward_cm(motor1, motor2, distance_to_obstacle_first_scan, speed);
		if (*obstacle_orientation_first_scan<0) {
			while (*obstacle_orientation_first_scan < 0){
				turn_left_precision(motor1, motor2, MAX_SPEED/8, -3);
				get_compass_value(&obstacle_orientation_first_scan);
			}
			printf("Robot well positioned back to his first left state\n");
		}
		else if (*obstacle_orientation_first_scan>=0) {
			while (*obstacle_orientation_first_scan > 0){
				turn_right_precision(motor1, motor2, MAX_SPEED/8, -3);
				get_compass_value(&obstacle_orientation_first_scan);
			}
			printf("Robot well positioned back to his first right state\n");
		}
	}*/
}

/** 
 * first big scan to get distance and orientation of the objects in radius of 60 cm
 * @param motor1 first motor
 * @param motor2 second motor
 * @param orientation_to_object angle to store the angle of the minimum distance found object
 * @param distance_to_object minimum distance to object found
 * written by Laila
 **/
void first_scan(uint8_t *sn_B, uint8_t *sn_D, float* orientations, float* distances){
	while ( ev3_tacho_init() < 1 ) Sleep( 1000 );

	int portB = OUTPUT_B;
	int portD = OUTPUT_D;
	if ( ev3_search_tacho_plugged_in(portB, 0, sn_B, 0 ) && ev3_search_tacho_plugged_in(portD, 0, sn_D, 0 )) {
		int radius = 600;
		int i;
		go_to_position_to_scan(*sn_B, *sn_D, MAX_SPEED/4, 130);
		scanSeveral(*sn_B, *sn_D, MAX_SPEED/4, 0, 180, radius, orientations, distances);
		for (i=0; i < 3; i++) {
			printf("Result orientation %d : %f\n", i, orientations[i]);
			printf("Result distance %d : %f\n", i, distances[i]);
		}
	}
	else {
		printf("MOTORS NOT FOUNDS FIRST SCAN\n");
	}
}


int init_sensor_motors(uint8_t *sn_B, uint8_t *sn_D, float* obstacle_orientation_first_scan, float* distance_to_obstacle_first_scan, int side){
	while ( ev3_tacho_init() < 1 ) Sleep( 1000 ); // Detect nb of connected tachos (<1 => none connected)

	int portB = OUTPUT_B;
	int portD = OUTPUT_D;
	if ( ev3_search_tacho_plugged_in(portB, 0, sn_B, 0 ) && ev3_search_tacho_plugged_in(portD, 0, sn_D, 0 )) {
		int radius_of_first_scan=400;
		//float obstacle_orientation_first_scan;
		allScan(*sn_B, *sn_D, MAX_SPEED/4, radius_of_first_scan, obstacle_orientation_first_scan, distance_to_obstacle_first_scan,side);
		printf("Allscan result to get orientation: %f\n",*obstacle_orientation_first_scan);
		printf("Allscan result to get distance to object: %f\n",*distance_to_obstacle_first_scan);
	}
	else {
		printf("MOTORS NOT FOUNDS FIRST SCAN\n");
	}
}

/** Function called in the main -> objective = from being positionned in front of an object, can determine which kind of object it is
 * Returns the object we think it is
 * @param sn_B -> Needed to make motor work
 * @param sn_D -> Needed to make motor work
 * @param obstacle_orientation_first_scan (used initially for go back to initial position)
 * @param distance_to_obstacle_first_scan (used initially for go back to initial position also)
 * Written by Gaël
 */

Object object_scan(uint8_t* sn_B, uint8_t *sn_D,float* obstacle_orientation_first_scan, float* distance_to_obstacle_first_scan ){

	//uint8_t sn_sonar; // For the sonar
	char s[256]; // To get some string values

	printf( "*** ( EV3 ) Hello TEST RECOGN! ***\n" );
	int portD = OUTPUT_D;
	int portB = OUTPUT_B;

	int iteration_done=0;
	int EDGE_FOUND = 0;
	
	Object all_objects[3];
	Object result1;
	Object result2;

	int object_on_the_left=0;
	int object_on_the_right=0;

	float last_compass_before_scan_move_to_front;

	float obstacle_orientation_second_scan;
	float distance_to_obstacle_second_scan;

	float* first_acquisition = object_acquisition(MAX_SPEED);

	printf("FIRST ACQUISITION SUCCESSFUL -> DETERMINE THE EDGES\n"); //  Pointeur instead of tab

	int position_edge1 = find_edge(first_acquisition);

	if(position_edge1 != -1){
		EDGE_FOUND = 1;
	} else {
		

		while (iteration_done<3) {

			// We move backward before each measurement, to modifiy a bit the values, but not the trend

			if ( ev3_search_tacho_plugged_in(portB,0, sn_B, 0 ) && ev3_search_tacho_plugged_in(portD,0, sn_D, 0 )) {
				if (iteration_done != 0) move_backward_cm(*sn_B, *sn_D, 1, MAX_SPEED/3);
			} else {
				printf("Some motor not detected\n");
			}

			float* first_acquisition = object_acquisition(MAX_SPEED);
			int position_edge1 = find_edge(first_acquisition);

			printf("TRY CHECK_CUBE\n");
			if ((is_cube(first_acquisition))==1) {
				printf("OBJECT CONSIDERED AS A CUBE\n");
				all_objects[iteration_done]=CUBE;
				iteration_done++;
				continue;
			}
			printf("TRY CHECK_PYRAMID_OR_CYLINDER\n");
			if ((check_pyramid_or_cylinder(first_acquisition))==1) {
				printf("OBJECT IS MORE LIKELY TO BE A PYRAMID\n");
				all_objects[iteration_done]=PYR4;
				iteration_done++;
				continue;
			}
			if ((check_pyramid_or_cylinder(first_acquisition))==2) {
				printf("OBJECT IS MORE LIKELY TO BE A CYLINDER\n");
				all_objects[iteration_done]=CYL;
				iteration_done++;
				continue;
			}
			printf("TRY CHECK_REVERSE_PYRAMID\n");
			if ((check_reverse_pyramid(first_acquisition))==1) {
				printf("OBJECT CONSIDERED AS A REVERSE_PYRAMID\n");
				all_objects[iteration_done]=PYR4_U;
				iteration_done++;
				continue;
			}
		}
		result1 = most_common(all_objects, 3);

		if (result1 == CUBE) {
			printf("IN THE END, OBJECT = CUBE\n");
		}
		if (result1 == PYR4) {
			printf("IN THE END, OBJECT = PYRAMID\n");
		}
		if (result1 == PYR4_U) {
			printf("IN THE END, OBJECT = REVERSE_PYRAMID\n");
		}
		if (result1 == CYL) {
			printf("IN THE END, OBJECT = CYLINDER\n");
		}

		// Goes back to Initial Position
		printf("GOING BACK TO INITIAL POSITION\n");
		move_back_to_initial_position(1, *sn_B, *sn_D, MAX_SPEED/8);

		return result1;
	}

	printf("GOING TO EXECUTE THE PROGRAM WHICH MOVES THE ROBOT SO THAT WE CAN BE IN FRONT IF WE ARE NOT\n");
	printf("OR CHECK IF THERE ARE OTHER EDGES TO CHECK IF IT IS A CYLINDER OR A PYRAMID\n");

	while ( ev3_tacho_init() < 1 ) Sleep( 1000 ); // Detect nb of connected tachos (<1 => none connected)

	if ( ev3_search_tacho_plugged_in(portB,0, sn_B, 0 ) && ev3_search_tacho_plugged_in(portD,0, sn_D, 0 )) {
		move_to_front(position_edge1, *sn_B, *sn_D, MAX_SPEED/8, &last_compass_before_scan_move_to_front,&distance_to_obstacle_second_scan);
	} else {
		printf("Some motor not detected\n");
	}

	printf("END OF MOVEMENT -> ROBOT SHOULD BE FRONT OR SOULD HAVE TAKEN INTO ACCOUNT NEW EDGES\n");
	printf("NOW THAT HE IS FRONT -> FRONT ACQUISITION\n");
	Sleep(1000); // To let the time to change the robot position for the tests (make it really front if not perfect)

	printf("NOW CHECKS WHAT KIND OF OBJECT IT IS\n");

	while(iteration_done < 3){
		if ( ev3_search_tacho_plugged_in(portB,0, sn_B, 0 ) && ev3_search_tacho_plugged_in(portD,0, sn_D, 0 )) {
			if (iteration_done != 0) move_backward_cm(*sn_B, *sn_D, 1, MAX_SPEED/3);
		} else {
			printf("Some motor not detected\n");
		}
		float* front_acquisition= object_acquisition(MAX_SPEED);

		printf("TRY CHECK_CUBE\n");
		if ((is_cube(front_acquisition))==1) {
			printf("OBJECT CONSIDERED AS A CUBE\n");
			all_objects[iteration_done]=CUBE;
			iteration_done++;
			continue;
		}
		printf("TRY CHECK_PYRAMID_OR_CYLINDER\n");
		if ((check_pyramid_or_cylinder(front_acquisition))==1) {
			printf("OBJECT IS MORE LIKELY TO BE A PYRAMID\n");
			all_objects[iteration_done]=PYR4;
			iteration_done++;
			continue;
		}
		// Will never be a cylinder because we found an edge in the first place
		// Did not remove the code but changed it to pyramid
		if ((check_pyramid_or_cylinder(first_acquisition))==2) {
			printf("OBJECT IS MORE LIKELY TO BE A CYLINDER BUT CONSIDER IT AS A PYRAMID BECAUSE WE FOUND AN EDGE BEFORE\n");
			all_objects[iteration_done]=PYR4;
			iteration_done++;
			continue;
		}
		printf("TRY CHECK_REVERSE_PYRAMID\n");
		if ((check_reverse_pyramid(front_acquisition))==1) {
			printf("OBJECT CONSIDERED AS A REVERSE_PYRAMID\n");
			all_objects[iteration_done]=PYR4_U;
			iteration_done++;
			continue;
		}
	}

	printf("NOW THAT WE MADE ALL OUR MEASUREMENTS, CAN SAY WHAT TYPE OF OBJECT IT IS\n");
	result2 = most_common(all_objects, 3);
	printf("SECOND FINAL RESULT = %d\n", result2);
	if (result2 == CUBE) {
		printf("IN THE END, OBJECT = CUBE\n");
	}
	if (result2 == PYR4) {
		printf("IN THE END, OBJECT = PYRAMID\n");
	}
	if (result2 == PYR4_U) {
		printf("IN THE END, OBJECT = REVERSE_PYRAMID\n");
	}
	if (result2 == CYL) {
		printf("IN THE END, OBJECT = CYLINDER\n");
	}

	printf("Moves back to initial position\n");
	move_back_to_initial_position(0, *sn_B, *sn_D, MAX_SPEED/8);

	return result2;
}

#endif


/**
 *
 * @param label
 * @param test
 */
void print_data(char label[], float* test){
	//printf(label);
	int i;
	for (i = 0; i < DATA_POINTS; i++){
		printf("%2.2f ", test[i]);
	}
	printf("\n");
}

#ifdef LOCAL_TESTING

int main (int argc, char* argv){
	float test[] = {400.0, 1.1, 500.5, 500.0, 350.00, 350.0, 20.0, 20.0, 20.0, 20.0, 20.0, 20.0, 20.0, 400.0, 400.0, 400.0, 20.0, 20.0, 20.0, 20.0, 20.0, 0.0, 30.0, 30.0, 500.0, 500.0};
	printf("%d\n", find_edge(test));

	float test2[DATA_POINTS];
	int i;
	for(i=0; i< DATA_POINTS; i++){

		test2[i] = 0.3 * i + 0.02 * (i * i);
	}
	test2[20] = 2000.0;

	print_data("Test array: ", test2);
	printf("Is cube: %d\n", is_cube(test2));
	return 0;
}
#endif



