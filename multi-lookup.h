#ifndef MULTI_LOOKUP_H
#define MULTI_LOOKUP_H

#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include "util.h"
#include "queue.h"
#include <sys/time.h>
#include <sys/types.h>
#include <sys/syscall.h>

#define MAX_INPUT_FILES 10
#define MAX_REQUESTER_THREADS 10
#define MAX_RESOLVER_THREADS 10
#define MAX_NAME_LENGTH 1025
#define MAX_IP_LENGiTH INET6 ADDRSTRLEN
#define USAGE "<# requester> <#resolver> <requester log> <resolver log> [<data file>...(limit 10 files)]"
#define MINARGS 6
#define CHARBUFF 1025
#define INPUTS "%1024s"


squeue filesToService;

typedef struct notglobal
{
	pthread_mutex_t request;
	pthread_mutex_t servicedt;
	double count;
	
} nglobal;

void* req_read_write(void *file);
void* res_read_write(void *file);


#endif
