#ifndef SHARED_H
#define SHARED_H
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include "all_libraries.h"
#include "messages.h"

#define ARRAY_SIZE 10

typedef struct threads{
    // pthread_t gyro;
    pthread_t rangeSensor;
    pthread_t server;
} Threads;

Threads init_threads();

void* ServerSendThread(void* param);
void* ServerRecThread(void* param);
void* SensorThread(void* param);
void* GyroThread(void* param);
	
int joinThreads(Threads threads);

typedef struct shared_i{
    uint8_t value;
    pthread_mutex_t lock;
} SharedInt;

int readSharedI(SharedInt* i);

// i = new
//Blocking if locked
void assignSharedI(SharedInt* i, uint8_t new);


// i = i + increase
//Blocking if locked
void incrementSharedI(SharedInt* i, uint8_t increase);


typedef struct shared_f{
    float value;
    pthread_mutex_t lock;
} SharedFloat;

float readSharedF(SharedFloat* i);

// i = new
//Blocking if locked
void assignSharedF(SharedFloat* i, float new);


// i = i + increase
//Blocking if locked
void incrementSharedF(SharedFloat* i, float increase);

//Thread safe FIFO
typedef struct queue{
    uint8_t queue[ARRAY_SIZE];
    pthread_mutex_t lock;
    uint8_t first;
    uint8_t last;
    bool full;
} SharedQueue;


// return Null on empty queue, else first value
// blocking if locked, returns 1 if empty
uint8_t popShared(SharedQueue* queue);

// returns 1 on full queue, else adds value last
// blocking if locked, returns 1 if full
uint8_t pushShared(SharedQueue* queue, uint8_t value);

uint8_t lengthShared(SharedQueue* queue);

void initQueue(SharedQueue* queue);


typedef struct message_queue{
    Message* queue[ARRAY_SIZE];
    pthread_mutex_t lock;
    uint8_t first;
    uint8_t last;
    bool full;
} SharedMsgQueue;


// return Null on empty queue, else first value
// blocking if locked
Message *popSharedMsg(SharedMsgQueue* queue);

// returns 1 on full queue, else adds value last
// blocking if locked
uint8_t pushSharedMsg(SharedMsgQueue* queue, Message* value);

uint8_t lengthSharedMsg(SharedMsgQueue* queue);

void initQueueMsg(SharedMsgQueue* queue);

//=============================================================================

// Put shared variables here to make them available to all threads:
// Initialize here and reference by pointer works, not sure if good practice..

SharedInt sonar_found;

SharedQueue testQueue;

SharedFloat sonar_shared_value;

SharedFloat gyro_shared_value;

SharedMsgQueue message_send_queue;

SharedInt play;


#endif //SHARED_H
