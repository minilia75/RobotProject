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



int s;

/**
 * Reads from the socket into a provided Message
 * @param sock socket to read from
 * @param buffer An empty, initalized message struct to save the data in
 * @param maxSize Maximum number of bytes to read
 * @return Number of bytes read
 */
int read_from_server (int sock, Message* buffer, size_t maxSize) {

  // Problem point, blocks indefinitly second time called in multi-thread scenario
  int bytes_read = read (sock, buffer, maxSize);

  if (bytes_read <= 0) {
    fprintf (stderr, "[ERR] Server unexpectedly closed connection...\n");
    close (s);
    exit (EXIT_FAILURE);
  }

  printf ("[DEBUG] Received %d bytes\n", bytes_read);
  return bytes_read;
}

/**
 * Reads from the socket into a buffer
 * @param sock socket to read from
 * @param buffer An empty, initalized array to save the data in
 * @param maxSize Maximum number of bytes to read
 * @return Number of bytes read
 */
int read_from_server2 (int sock, uint8_t* buffer, size_t maxSize) {

  // Problem point, blocks indefinitly second time called in multi-thread scenario
  int bytes_read = read (sock, buffer, maxSize);

  if (bytes_read <= 0) {
    fprintf (stderr, "[ERR] Server unexpectedly closed connection...\n");
    close (s);
    exit (EXIT_FAILURE);
  }

  printf ("[DEBUG] Received %d bytes\n", bytes_read);
  return bytes_read;
}


/**
 * The worker function for receiving messages, currently get stuck on second message read
 * @param param unused, defined by thread interface
 */
void* ServerRecThread(void* param){
	int sockfd, connfd; 
	uint8_t* buffer;
	struct sockaddr_in servaddr, cli; 
	Message *unack_message = NULL;
	int retry_counter;

    uint16_t ID = 0;
 
	//Initiation and connection

    printf("ServerThread\n");
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

		// Read Messages
		printf("restarting\n");
		Message* in_message = EMPTY_message();
        uint8_t* test_buffer = malloc(8 * sizeof(uint8_t));
		#ifndef testing
		// Blocking function which is the reason it stops, we cant figure out why
       	read_from_server(sockfd, in_message, 8);
		#endif

		#ifdef testing
		if (counter == 0){
			printf("[SIM]Start message\n");
			uint8_t body[] = {1,2,3};
			in_message = new_message(10, SERVER_DST, ME, START, body);
		} else if (counter == 5){
			printf("[SIM]ACK message\n");
			uint8_t body[] = {0, (uint8_t)0, (uint8_t)(0 << 8)};
			in_message = new_message(10, SERVER_DST, ME, ACK, body);
		} else if (counter == 10){
			printf("[SIM]STOP message\n");
			uint8_t body[] = {1,2,3};
			in_message = new_message(10, SERVER_DST, ME, STOP, body);
		}
		counter++;
		#endif	

		// Message interpretation

		printf("Message received:");
        print_message(in_message);

        if(in_message->dst != ME){
            printf("Message for %d\n", in_message->dst);
        } else {
            switch (in_message->type)
            {
            case ACK:
                printf("ACK received: ");
				print_message(in_message);
				if(unack_message == NULL){
					printf("ACK error, no unack_message\n");
				} else {
					if (in_message->body[1] | in_message->body[2] << 8 != getID(unack_message)){
						printf("ACK error, wrong ID\n");
					} else {
						if (in_message->body[0] != 0) {
							printf("ACK error, error\n");
						} else {
							printf("ACK ok\n");
							unack_message = NULL;
							retry_counter = 0;
						}
					}
				}
                break;

            case START:
                printf("START received\n");
				assignSharedI(&play, 1);
                break;

            case STOP:
                printf("STOP received\n");
				assignSharedI(&play, 0);
				return 0;

            default:
                break;
            }
        }
	}
}
