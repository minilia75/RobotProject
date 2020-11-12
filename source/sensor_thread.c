
#include "all_libraries.h"

#define SAMPLE_INTERVAL 10

uint8_t sn_sonar_recogn; // For the sonar

void* SensorThread(void* param){

    printf("Sensor_thread started\n");
    ev3_sensor_init();

    while (readSharedI(&sonar_found) == 0) {
    	if (!ev3_search_sensor(LEGO_EV3_US, &sn_sonar_recogn,0)){ // Put this line => No need for input port (in1)
    		//printf("Sonar not found\n");
    		assignSharedI(&sonar_found, 0);
    	} else {
    		printf("Sonar found\n");
    		assignSharedI(&sonar_found, 1);
    	}
    }
    /*
    if (!ev3_search_sensor(LEGO_EV3_US, &sn_sonar_recogn,0)){ // Put this line => No need for input port (in1)
        printf("Sonar not found\n");
        return 0;
    } else {
        printf("Sonar found\n");
    }*/

    while (1){

        float sonar_local_value;

        if ( !get_sensor_value0(sn_sonar_recogn, &sonar_local_value )) {
            sonar_local_value = 0;
        }

        assignSharedF(&sonar_shared_value, sonar_local_value);

        //printf( "Sonar value captured: %f \n", sonar_local_value);
    }
}
