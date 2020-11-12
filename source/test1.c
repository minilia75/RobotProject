//
// Created by Clara on 08/01/2020.
//
#include "all_libraries.h"
#include "config.h"
#include "SharedData.c"
#include "sensor_thread.c"
#include "gyro_thread.c"
#include "messages.c"
#include "search.c"
#include "movement.c"
int main( void ) {
    int i;
    uint8_t found_cube = 0;
    uint8_t found_4_pyramid = 0;
    uint8_t found_4_reversed_pyramid = 0;
    uint8_t found_cylinder = 0;
    uint8_t found_3_pyramid = 0;

    //sn = sequence number
    uint8_t sn_D; // For the motor D
    uint8_t sn_B; // For the motor B
    char s[256]; // To get some string values

    // Get Motors work a little: Horizontal Motor -> Port A // Vertical Motor -> Port B
    // Different types of tachos : M, L and None

    while ( ev3_tacho_init() < 1 ) Sleep( 1000 ); // Detect nb of connected tachos (<1 => none connected)

    printf( "*** ( EV3 ) Hello! ***\n" );
    int portD;
    portD = OUTPUT_D;
    int max_speed;
    int portB;
    portB = OUTPUT_B;

	if ( ev3_search_tacho_plugged_in(portB, 0, &sn_B, 0 ) && ev3_search_tacho_plugged_in(portD, 0, &sn_D, 0 )) {
		initGyro();
        int radius = 250;
        float obstacle_orientation[3];
        float obstacle_distance[3];
        allScan(sn_B, sn_D, MAX_SPEED/4, radius, obstacle_orientation, obstacle_distance);
        //float obstacle_orientation;
        //float obstacle_distance;
        //allScan(sn_B, sn_D, MAX_SPEED/4, radius, &obstacle_orientation, &obstacle_distance);
    }
}
