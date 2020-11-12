
#ifndef MESSAGES
#define MESSAGES

#include <stdint.h>

#define ME 2
#define SERVER_DST 255

typedef enum message_type {
	ACK, START, STOP, KICK, TEST, OBJ_ID
}Type;

typedef struct message {
	uint8_t ID_l;
	uint8_t ID_h;
	uint8_t src;
	uint8_t dst;
	uint8_t type;
	int8_t body[3];
}Message;

Message *new_message(uint16_t ID, uint8_t src, uint8_t dst,  uint8_t type, int8_t body[3]);

Message *OBJ_ID_message(Object type, int8_t x, int8_t y);

Message *KICK_message(int8_t x, int8_t y);

Message *TEST_message();

Message *EMPTY_message();

void setID(Message* message, uint16_t ID);

uint16_t getID(Message* message);

int send_message(Message *m);

void print_message(Message *m);

#endif //MESSAGES.H
