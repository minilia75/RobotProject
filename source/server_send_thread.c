#include "all_libraries.h"

#include <stdarg.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h> 
#include <string.h> 

#define SERV_ADDR "192.168.43.201"     
#define SERV_PORT 1024			

#define RETRIES 10

#define SA struct sockaddr 
#define Sleep( msec ) usleep(( msec ) * 1000 )


// #define testing


void* ServerSendThread(void* param){
	int sockfd, connfd; 
	struct sockaddr_in servaddr, cli; 
	Message *unack_message = NULL;
	int retry_counter;

    uint16_t ID = 0;
 
    printf("ServerSendThread\n");
    initQueueMsg(&message_send_queue);


	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("[ERR] Socket creation failed...\n"); 
		exit(0); 
	} 
	else printf("[DEBUG] Socket successfully created..\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

     
	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = inet_addr(SERV_ADDR); 
	servaddr.sin_port = htons(SERV_PORT); 


	#ifndef testing
	// connect the client socket to server socket 
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
		printf("[ERR] Connection with the server failed...\n"); 
		exit(0); 
	} 
	else printf("[DEBUG] Connected to the server..\n"); 
	#endif

	int counter = 0;
    
    while(1){


		// Send Messages

		if (unack_message == NULL){
			Message* out_message = popSharedMsg(&message_send_queue);
			if (out_message != NULL) {
				setID(out_message, ID++);
				printf("Sending Message: ");
				print_message(out_message);
				unack_message = out_message;
				write(sockfd, out_message, 8);
			} 
		} //else {
			// if(retry_counter == RETRIES){
			// 	printf("Giving up on message: ");
			// 	print_message(unack_message);
			// 	unack_message = NULL;
			// 	retry_counter = 0;

			// } else {

			// 	printf("Resending Message: ");
			// 	print_message(unack_message);
			// 	write(sockfd, unack_message, 8);
			// 	retry_counter++;
			//}
		//}
	Sleep(1000);
	}
}
