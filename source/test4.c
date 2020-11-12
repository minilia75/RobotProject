#include "all_libraries.h"
#include "config.h"

//Test for finding ball at a predistined position
int main( void ) {

    //sn = sequence number
	uint8_t sn_sonar; // For the sonar
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
	uint8_t sn_gyro;
	int init_gyro;

    if ( ev3_search_tacho_plugged_in(portB, 0, &sn_B, 0 ) && ev3_search_tacho_plugged_in(portD, 0, &sn_D, 0 )) {
		move_backward_cm(sn_B, sn_D, MAX_SPEED/4, 30);
		turn_right_precision(sn_B, sn_D, MAX_SPEED/8, 90);
        move_backward_cm(sn_B, sn_D, MAX_SPEED/4, 30);
    }
}