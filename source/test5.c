//
// Created by Clara on 09/01/2020.
//

//
// Created by Clara on 09/01/2020.
//


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




int main(void){
      uint8_t sn_C;
    int portC = OUTPUT_C;
     if (ev3_search_tacho_plugged_in(portC,0, &sn_C, 0 )){
   
        launch(sn_C,  MAX_SPEED/4);
         in_launch_position(sn_C,  MAX_SPEED/4);
    
         
     }}

 





