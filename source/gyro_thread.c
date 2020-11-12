
#include "all_libraries.h"

#define SAMPLE_INTERVAL 10

uint8_t sn_gyro;
int init_gyro;

void* GyroThread(void* param){

    printf("Gyro_thread started\n");


    // Gyro_init   
	printf( "GYRO sensor is found\n" );
	set_sensor_mode(sn_gyro, "GYRO-ANG");
	set_sensor_mode( sn_gyro, "GYRO-G&A" );
	Sleep(10);
	get_sensor_value(0, sn_gyro, &init_gyro);
	Sleep(10);
	printf("init_gyro %d \n", init_gyro);
	printf("sn_gyro %d \n", sn_gyro);
    

    /*
    if (!ev3_search_sensor(LEGO_EV3_US, &sn_sonar_recogn,0)){ // Put this line => No need for input port (in1)
        printf("Sonar not found\n");
        return 0;
    } else {
        printf("Sonar found\n");
    }*/

    while (1){

        float gyro_local_value;

        if ( !get_sensor_value0(sn_gyro, &gyro_local_value )) {
            gyro_local_value = 0;
        }

        assignSharedF(&gyro_shared_value, gyro_local_value);

        //printf( "Sonar value captured: %f \n", sonar_local_value);
    }
}
