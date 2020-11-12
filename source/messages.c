
#include "all_libraries.h"
#include <stdlib.h>
#include <stdio.h>

/**
* Creata am message with specified content
* @param ID Message ID
* @param src Sender ID 
* @param dst Receiver ID 
* @param type Message type
* @param body An array of 3 chars holding the body of the message
* @return A message struct with data 
*/
Message *new_message(uint16_t ID, uint8_t src, uint8_t dst,  uint8_t type, int8_t body[3]) {
	Message *m = malloc(sizeof(*m) + sizeof(uint8_t) * 3);
	m->ID_l = (uint8_t)ID;
    m->ID_h = (uint8_t)(ID >> 8);
	m->src = src;
	m->dst = dst;
	m->type = type;
    m->body[0] = body[0];
    m->body[1] = body[1];
    m->body[2] = body[2];
	return m;
}

/**
 * Allows easy conversion of a uint16 to little endian
 * @param message The massage to assign ID to
 * @param ID th 16 bit ID
 */
void setID(Message* message, uint16_t ID){
    message->ID_l = (uint8_t)ID;
    message->ID_h = (uint8_t)(ID >> 8);
}

uint16_t getID(Message* message){
    return message->ID_l | message->ID_h << 8;
}

/**
 * Message creator for messages of type OBJ_ID
 * @param type type of the object numerated in the Object enum
 * @param x estimated x position
 * @param y estimated y position
 * @return Message struct ready to be sent to sender thread
 */
Message *OBJ_ID_message(Object type, int8_t x, int8_t y){
    int8_t body[] =  {(int8_t)type, x, y};
    return new_message(0, ME, SERVER_DST, OBJ_ID, body);
}

Message *KICK_message(int8_t x, int8_t y){
    uint8_t body[] = {x, y, 0};
    return new_message(0, ME, SERVER_DST, KICK, body);
}

Message *TEST_message(){
    uint8_t body[] = {1,2,3};
    return new_message(0, ME, SERVER_DST, TEST, body);
}

Message *EMPTY_message(){
    int8_t body[] = {0,0,0};
    return new_message(0, 0, 0, 0, body);
}

void print_message(Message *m) {
	printf("Message: %d %d %d  %d  %d %d %d %d \n", m->ID_l, m->ID_h, m->src, m->dst, m->type, m->body[0], m->body[1], m->body[2]);
}

int send_message(Message* m){
    return pushSharedMsg(&message_send_queue, m);
}
