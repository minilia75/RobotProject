
#include <pthread.h>
#include <stdio.h>
#include "SharedData.h"

// Integer functions

int readSharedI(SharedInt* i){
    return i->value;
}

void assignSharedI(SharedInt* i, uint8_t new){
    pthread_mutex_lock(&i->lock);
    i->value = new;
    pthread_mutex_unlock(&i->lock);
}
/**
 * Safe incremantation of shared integer
 * @param i Pointer to shared interger
 * @param increase Amount to increase by
 */
void incrementSharedI(SharedInt* i, uint8_t increase){
    pthread_mutex_lock(&i->lock);
    i->value = i->value + increase;
    pthread_mutex_unlock(&i->lock);
}

//Float functions

float readSharedF(SharedFloat* i){
    return i->value;
}

void assignSharedF(SharedFloat* i, float new){
    pthread_mutex_lock(&i->lock);
    i->value = new;
    pthread_mutex_unlock(&i->lock);
}

void incrementSharedF(SharedFloat* i, float increase){
    pthread_mutex_lock(&i->lock);
    i->value = i->value + increase;
    pthread_mutex_unlock(&i->lock);
}
//====== Queue ============================

uint8_t popShared(SharedQueue* queue){
    pthread_mutex_lock(&queue->lock);
    if (queue->first == queue->last && !queue->full){   //Empty queue
        printf("empty queue\n");
	pthread_mutex_unlock(&queue->lock);
        return -1;
    }
    uint8_t x = queue->queue[queue->first];
    queue->first = (queue->first + 1) % ARRAY_SIZE; //Wrap around
    queue->full = false;
    pthread_mutex_unlock(&queue->lock);

    return x;
}

uint8_t pushShared(SharedQueue* queue, uint8_t value){
    pthread_mutex_lock(&queue->lock);
    if (queue->full){
        printf("Queue full\n");
        pthread_mutex_unlock(&queue->lock);
        return 1;
    }
    queue->queue[queue->last] = value;
    queue->last = (queue->last + 1) % ARRAY_SIZE; //Wrap around
    if (queue->last == queue->first) {
        queue->full = true;
    }
    pthread_mutex_unlock(&queue->lock);
    return 0;
}

uint8_t lengthShared(SharedQueue* queue){
    return queue->last - queue->first;
}

void initQueue(SharedQueue* queue){
    queue->first = 0;
    queue->last = 0;
    queue->full = false;
}


/**
 * Removes and returns the first value of a shared queue, blocks if locked
 * @param queue Pointer to shared queue
 * @return First message in queue or NULL if empty
 */
Message *popSharedMsg(SharedMsgQueue* queue){
    pthread_mutex_lock(&queue->lock);
    if (queue->first == queue->last && !queue->full){   //Empty queue
        printf("empty queue\n");
	    pthread_mutex_unlock(&queue->lock);
        return NULL;
    }
    Message *x = queue->queue[queue->first];
    queue->first = (queue->first + 1) % ARRAY_SIZE; //Wrap around
    queue->full = false;
    pthread_mutex_unlock(&queue->lock);

    return x;
}

/**
 * Puts a Message in the shared queue, blocks if locked
 * @param queue Pointer to the shared queue
 * @param value The message to be queued
 * @return 0 if OK, 1 if queue is full
 */

uint8_t pushSharedMsg(SharedMsgQueue* queue, Message *value){
    pthread_mutex_lock(&queue->lock);
    if (queue->full){
        printf("Queue full\n");
        pthread_mutex_unlock(&queue->lock);
        return 1;
    }
    queue->queue[queue->last] = value;
    queue->last = (queue->last + 1) % ARRAY_SIZE; //Wrap around
    if (queue->last == queue->first) {
        queue->full = true;
    }
    pthread_mutex_unlock(&queue->lock);
    return 0;
}

uint8_t lengthSharedMsg(SharedMsgQueue* queue){
    return queue->last - queue->first;
}

void initQueueMsg(SharedMsgQueue* queue){
    queue->first = 0;
    queue->last = 0;
    queue->full = false;
}

//=========Threads=========

// Threads init_threads(){
//    Threads threads;

//     pthread_create(&threads.rangeSensor, NULL, SensorThread, NULL);
// //    pthread_create(&threads.gyro, NULL, GyroThread, NULL);
//    pthread_create(&threads.server, NULL, ServerThread, NULL);

//    return threads;
// }

int joinThreads(Threads threads){
    
    // int retValG = pthread_join(threads.gyro, NULL);
    // int retValRS = pthread_join(threads.rangeSensor, NULL);
    int retValS = pthread_join(threads.server, NULL);

    return retValS; //+ retValG + retValRS;
}
