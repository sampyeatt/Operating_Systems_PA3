#include <stdlib.h>
#include "queue.h"

int init(squeue* q, int size){  
    if(size > 0) 
    {
        q->maxSize = size;
    }
    else 
    {
        q->maxSize = QUEUEMAXSIZE;
    }
    
    q->array = malloc(sizeof(queue_node) * (q->maxSize));
    
    if(!(q->array))
    {
        return QUEUE_FAILURE;
    }
    for(int i=0; i < q->maxSize; ++i)
    {
        q->array[i].payload = malloc(sizeof(void*));
        q->array[i].used = 0;
    }
    q->front = 0;
    q->rear = 0;
    return q->maxSize;
}

void cleanup(squeue* q)
{
    while(!is_empty(q))
    {
        pop(q);
    }
    free(q->array);
}

void* pop(squeue* q)
{
    void* ret_payload;
    if(is_empty(q))
    {
        return NULL;
    }
    ret_payload = q->array[q->front].payload;
    q->array[q->front].used = 0;
    q->front = ((q->front + 1) % q->maxSize);
    return ret_payload;
}

int push(squeue* q, void* new_payload)
{
    if(is_full(q))
    {
        return QUEUE_FAILURE;
    }
    q->array[q->rear].payload = new_payload;
    q->array[q->rear].used = 1;
    q->rear = ((q->rear+1) % q->maxSize);
    return QUEUE_SUCCESS;
}
bool is_empty(squeue* q)
{
    if((q->front == q->rear) && (q->array[q->front].used == 0))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool is_full(squeue* q)
{
    if((q->front == q->rear) && (q->array[q->front].used == 1))
    {
        return true;
    }
    else
    {
        return false;
    }
}

// int main(int argc, char *argv[])
// {
//     squeue testqueue;
//     init(&testqueue, 2);

//     push(&testqueue, "first");
//     push(&testqueue, "second");
//     push(&testqueue, "thirsd");

//     char* pop1 = pop(&testqueue);
//     char* pop2 = pop(&testqueue);
//     char* pop3 = pop(&testqueue);
//     char* pop4 = pop(&testqueue);
//     printf("pop1 = %s, pop2 = %s\n", pop1, pop2);
//     printf("pop3 = %s\n", pop3);
//     printf("pop4 =%s\n", pop4 );
//     return 0;

// }