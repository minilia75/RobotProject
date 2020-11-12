//
// Created by Clara on 09/01/2020.
//
#include "all_libraries.h"

#include "SharedData.c"
#include "messages.c"
#include "server_send_thread.c"
#include "server_receive_thread.c"
#include "sensor_thread.c"

int main( void ){

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

    Message* m1 = TEST_message();
    printf("Posting message: ");
    print_message(m1);
    send_message(m1);
    Message* m2 = OBJ_ID_message(6, 0, 0);
    printf("Posting message: ");
    print_message(m2);
    send_message(m2);
    Message* m3 = OBJ_ID_message(4, 60, 22);
    printf("Posting message: ");
    print_message(m3);
    send_message(m3);

    // wait for STOP
    while(readSharedI(&play) != 0){
        printf("waiting for stop\n");
        sleep(3);
    };

    pthread_join(rec_thread, NULL);

    printf("exiting!!!!\n");
}