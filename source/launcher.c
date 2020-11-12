//
// Created by Clara on 09/01/2020.
//

#include "all_libraries.h"

#define Sleep( msec ) usleep(( msec ) * 1000 )

/**
 * launch the ball
 * @param motor3 
 * @param speed 
 */
void launch(int motor3, int speed) {
    printf("LAUNCH\n");
    get_tacho_max_speed( motor3, &speed ); // Get the maxspeed value of the tacho -> going to print it
    printf("  max speed = %d\n", speed );
    set_tacho_stop_action_inx( motor3, TACHO_COAST );  //Don't know what this is about
    set_tacho_speed_sp( motor3, speed );
    set_tacho_time_sp( motor3, 300 );
    set_tacho_ramp_up_sp( motor3, 100 ); // 3seconds to climb in power
    set_tacho_ramp_down_sp( motor3, 100 ); // 2seconds to go down in power
    set_tacho_command_inx( motor3, TACHO_RUN_TIMED ); // Launches the time duration motorization
    printf("First Movement successful for motor C\n");
    set_tacho_command_inx( motor3, TACHO_RUN_TIMED ); // launches a second time a time duration motorization
    printf("Second Movement successful for motor C\n");


}
/**
 * return in position to launch the ball
 * @param motor3 
 * @param speed 
 */
void in_launch_position(int motor3, int speed){
        printf("GO BACK IN POSITION AFTER LAUNCH\n");
        get_tacho_max_speed( motor3, &speed ); // Get the maxspeed value of the tacho -> going to print it
        printf("  max speed = %d\n", speed );
        set_tacho_stop_action_inx( motor3, TACHO_COAST );  //Don't know what this is about
        set_tacho_speed_sp( motor3, - speed);
        set_tacho_time_sp( motor3, 300 );
        set_tacho_ramp_up_sp( motor3, 100 ); // 3seconds to climb in power
        set_tacho_ramp_down_sp( motor3, 100 ); // 2seconds to go down in power
        set_tacho_command_inx( motor3, TACHO_RUN_TIMED ); // Launches the time duration motorization
        printf("First Movement successful for motor C\n");
        set_tacho_command_inx( motor3, TACHO_RUN_TIMED ); // launches a second time a time duration motorization
        printf("Second Movement successful for motor C\n");
}
/**
 * go to the ball on the right
 * @param motor1 
 * @param motor2 
 * @param speed 
 */
void go_to_ball_right(int motor1, int motor2, int speed){
    turn_left_precision(motor1, motor2, speed, 90);
    move_forward_cm(motor1, motor2, 35, speed);
    turn_right_precision(motor1, motor2, speed, 270);
    move_backward_cm(motor1, motor2, 20, speed);
    turn_right_precision(motor1, motor2, speed, 90);
    
}
/**
 * come back from the ball on the right
 * @param motor1 
 * @param motor2 
 * @param speed 
 */
void comeback_to_ball_right(int motor1, int motor2, int speed){
    turn_left_precision(motor1, motor2, speed, 60);
    move_forward_cm(motor1, motor2, 23, speed);
    turn_left_precision(motor1, motor2, speed, 200);
    move_backward_cm(motor1, motor2, 34, speed);
    turn_right_precision(motor1, motor2, speed, 40);

}
/**
 * go to the ball on the left
 * @param motor1 
 * @param motor2 
 * @param speed 
 */
void go_to_ball_left(int motor1, int motor2, int speed){
    turn_right_precision(motor1, motor2, speed, 90);
    move_forward_cm(motor1, motor2, 30, speed);
    turn_left_precision(motor1, motor2, speed, 240);
    move_backward_cm(motor1, motor2, 23, speed);
    turn_left_precision(motor1, motor2, speed, 60);
    
}
/**
 * come back from the ball on the left
 * @param motor1 
 * @param motor2 
 * @param speed 
 */
void comeback_to_ball_left(int motor1, int motor2, int speed){
    turn_right_precision(motor1, motor2, speed, 60);
    move_forward_cm(motor1, motor2, 23, speed);
    turn_right_precision(motor1, motor2, speed, 200);
    move_backward_cm(motor1, motor2, 34, speed);
    turn_left_precision(motor1, motor2, speed, 40);
}
/**
 * go and come back to the ball on the right
 * @param motor1 
 * @param motor2 
 * @param motor3 
 * @param speed 
 */
void  go_and_comeback_right(int motor1, int motor2, int speed){
    uint8_t sn_C;
    int portC = OUTPUT_C;
     if (ev3_search_tacho_plugged_in(portC,0, &sn_C, 0 )){ 
        go_to_ball_right(motor1, motor2, speed);
        launch(sn_C, speed);
        Sleep(500);     
        in_launch_position(sn_C, speed);
        comeback_to_ball_right(motor1, motor2,speed);
  }}
/**
* go and come back to the ball on the left
* @param motor1 
* @param motor2 
* @param motor3 
* @param speed 
*/
void  go_and_comeback_left(int motor1, int motor2, int speed){
    uint8_t sn_C;
    int portC = OUTPUT_C;
     if (ev3_search_tacho_plugged_in(portC,0, &sn_C, 0 )){
        go_to_ball_left(motor1, motor2, speed);
        launch(sn_C, speed);
        Sleep(500);     
        in_launch_position(sn_C, speed);
        comeback_to_ball_left(motor1, motor2,speed);
         
     }}
  


