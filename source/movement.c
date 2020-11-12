#include "all_libraries.h"

/** 
 * Function to stop a motor when done
 * @param motor motor to stop
 * written by Laila
 **/
void wait_motor_stop(uint8_t motor) {
	FLAGS_T state;
	do {
		get_tacho_state_flags(motor, &state );
	} while ( state );
}

/** 
 * Function to move forward
 * @param motor1 first motor
 * @param motor2 second motor
 * @param cm length in centimenter to advance
 * @param speed speed to turn
 * written by Laila
 **/
void move_forward_cm(int motor1, int motor2, int cm, int speed) {
	float deg = (360.0*cm*10)/(M_PI * WHEEL_DIAM);
	set_tacho_stop_action_inx( motor1, STOP_ACTION );
	set_tacho_stop_action_inx( motor2, STOP_ACTION );
	set_tacho_speed_sp( motor1, speed * COMP_SX);
	set_tacho_speed_sp( motor2, speed * COMP_DX);
	set_tacho_position_sp( motor1, - MOT_DIR * deg );
	set_tacho_position_sp( motor2, - MOT_DIR * deg );
	set_tacho_ramp_up_sp( motor1, MOV_RAMP_UP );
	set_tacho_ramp_up_sp( motor2, MOV_RAMP_UP );
	set_tacho_ramp_down_sp( motor1, MOV_RAMP_DOWN );
	set_tacho_ramp_down_sp( motor2, MOV_RAMP_DOWN );
	set_tacho_command_inx( motor1, TACHO_RUN_TO_REL_POS );
	set_tacho_command_inx( motor2, TACHO_RUN_TO_REL_POS );
	wait_motor_stop(motor1);
	wait_motor_stop(motor2);
}

/** 
 * Function to move backward
 * @param motor1 first motor
 * @param motor2 second motor
 * @param cm length in centimenter to advance
 * @param speed speed to turn
 * written by Laila
 **/
void move_backward_cm(int motor1, int motor2, int cm, int speed) {
	float deg = (360.0*cm*10)/(M_PI * WHEEL_DIAM);
	set_tacho_stop_action_inx( motor1, STOP_ACTION );
	set_tacho_stop_action_inx( motor2, STOP_ACTION );
	set_tacho_speed_sp( motor1, speed * COMP_SX);
	set_tacho_speed_sp( motor2, speed * COMP_DX);
	set_tacho_position_sp( motor1, MOT_DIR * deg );
	set_tacho_position_sp( motor2, MOT_DIR * deg );
	set_tacho_ramp_up_sp( motor1, MOV_RAMP_UP );
	set_tacho_ramp_up_sp( motor2, MOV_RAMP_UP );
	set_tacho_ramp_down_sp( motor1, MOV_RAMP_DOWN );
	set_tacho_ramp_down_sp( motor2, MOV_RAMP_DOWN );
	set_tacho_command_inx( motor1, TACHO_RUN_TO_REL_POS );
	set_tacho_command_inx( motor2, TACHO_RUN_TO_REL_POS );
	wait_motor_stop(motor1);
	wait_motor_stop(motor2);
}

/** 
 * Function to init gyro thread
 * written by Laila
 **/
void initGyro(){
	pthread_t thread;
    pthread_create(&thread, NULL, GyroThread, NULL);
}

/** 
 * Function to convert gyro value to angle
 * @param value to store angle value of gyro
 * written by Laila
 **/
void get_compass_value(float *value){
	*value = (float)readSharedF(&gyro_shared_value);
	//printf("gyro value: %f", *value);
	if(*value>=0){
		*value=(int)*value%360;
	}
	else{
		*value=(int)*value%-360;
	}
}

/** 
 * Function to get a mean of samples gyro value
 * @param value to store angle value of gyro
 * @param samples number of samples wanted
 * written by Laila
 **/
float get_value_samples(float *value, int samples) {
	int sum = 0;
	int i;
	int compass_value;

	for (i = 0; i < samples; i++) {
		get_compass_value(value);
		sum  += *value;
	}
	return sum/(float)samples;
}

/** 
 * Function to turn left
 * @param motor1 first motor
 * @param motor2 second motor
 * @param speed speed to turn
 * @param deg angle to turn to
 * written by Laila
 **/
void turn_left(int motor1, int motor2, int speed, int deg) {
	int deg_to_distance = MOT_DIR*(M_PI*WHEEL_DIAM*10)*deg/360;
	set_tacho_stop_action_inx( motor1, STOP_ACTION );
	set_tacho_stop_action_inx( motor2, STOP_ACTION );
	set_tacho_speed_sp( motor1, speed * COMP_SX);
	set_tacho_speed_sp( motor2, speed * COMP_DX);
	set_tacho_ramp_up_sp( motor1, 0 );
	set_tacho_ramp_up_sp( motor2, 0 );
	set_tacho_ramp_down_sp( motor1, 0 );
	set_tacho_ramp_down_sp( motor2, 0 );
	set_tacho_position_sp( motor1, deg_to_distance/2);
	set_tacho_position_sp( motor2,  -deg_to_distance/2); //-MOT_DIR*(M_PI*WHEEL_DIAM*10)*deg/360
	set_tacho_command_inx( motor1, TACHO_RUN_TO_REL_POS );
	set_tacho_command_inx( motor2, TACHO_RUN_TO_REL_POS );
	wait_motor_stop(motor1);
	wait_motor_stop(motor2);
}

/** 
 * Function to turn right
 * @param motor1 first motor
 * @param motor2 second motor
 * @param speed speed to turn
 * @param deg angle to turn to
 * written by Laila
 **/
void turn_right(int motor1, int motor2, int speed, int deg) {
	int deg_to_distance = MOT_DIR*(M_PI*WHEEL_DIAM*10)*deg/360;
	set_tacho_stop_action_inx( motor1, STOP_ACTION );
	set_tacho_stop_action_inx( motor2, STOP_ACTION );
	set_tacho_speed_sp( motor1, speed * COMP_SX);
	set_tacho_speed_sp( motor2, speed * COMP_DX);
	set_tacho_ramp_up_sp( motor1, 0 );
	set_tacho_ramp_up_sp( motor2, 0 );
	set_tacho_ramp_down_sp( motor1, 0 );
	set_tacho_ramp_down_sp( motor2, 0 );
	set_tacho_position_sp( motor1, -deg_to_distance/2 );
	set_tacho_position_sp( motor2, deg_to_distance/2 );
	set_tacho_command_inx( motor1, TACHO_RUN_TO_REL_POS );
	set_tacho_command_inx( motor2, TACHO_RUN_TO_REL_POS );
	wait_motor_stop(motor1);
	wait_motor_stop(motor2);
}

/** 
 * Function to turn right with precision using gyro because of decalibration problem
 * @param motor1 first motor
 * @param motor2 second motor
 * @param speed speed to turn
 * @param deg angle to turn to
 * written by Laila
 **/
void turn_right_precision(int motor1, int motor2, int speed, int deg) {
	float value_start;
	float value_end;
	int angle_gyro_start;
	int angle_gyro_end;
	float difference;

	angle_gyro_end = get_value_samples(&value_start, 5);

	turn_right(motor1, motor2, speed, deg);

	angle_gyro_end = get_value_samples(&value_end, 5);

	difference = deg - (angle_gyro_end - angle_gyro_start);
	while (difference >= 4 && difference < deg){
		turn_right(motor1, motor2, speed, difference);

		angle_gyro_end = get_value_samples(&value_end, 5);
		difference = deg - (angle_gyro_end - angle_gyro_start);
		if (difference < 0){
			difference = - difference;
		}
	}
}

/** 
 * Function to turn left with precision using gyro because of decalibration problem
 * @param motor1 first motor
 * @param motor2 second motor
 * @param speed speed to turn
 * @param deg angle to turn to
 * written by Laila
 **/
void turn_left_precision(int motor1, int motor2, int speed, int deg) {
	float value_start;
	float value_end;
	float angle_gyro_start;
	float angle_gyro_end;
	float difference;

	angle_gyro_end = (float)get_value_samples(&value_start, 5);

	turn_left(motor1, motor2, speed, deg);

	angle_gyro_end = (float)get_value_samples(&value_end, 5);

	difference = (float)deg + (angle_gyro_end - angle_gyro_start);
	printf("difference : %f\n", difference);

	while (difference >= 4 && difference < deg){
		turn_left(motor1, motor2, speed, difference);

		angle_gyro_end = get_value_samples(&value_end, 5);
		difference = (float)deg + (angle_gyro_end - angle_gyro_start);
		if (difference < 0){
			difference = - difference;
		}
		printf("difference : %f\n", difference);
	}
}
