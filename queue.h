#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdbool.h>

#define QUEUEMAXSIZE 50
#define QUEUE_FAILURE -1
#define QUEUE_SUCCESS 0

typedef struct queue_node_s
{
    void* payload;
    int used;
} queue_node;

typedef struct sQueue
{
    queue_node* array;
    int front;
    int rear;
    int maxSize;
    pthread_mutex_t muequeue;
    pthread_cond_t halfFull;
	pthread_cond_t halfEmpty;
} squeue;

int init(squeue* q, int size);
int push(squeue* q, void* payload);
bool is_full(squeue* q);
bool is_empty(squeue* q);
void* pop(squeue* q);
void cleanup(squeue* q);

#endif