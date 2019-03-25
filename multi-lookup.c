#include "multi-lookup.h"
#include "queue.h"

int main(int argc, char *argv[])
{
	//requester = how many threads are availabel to read the files
		//used for reading the files
	//resolver = how many threads can read from the array 
		//write to the resolver_file
	nglobal ng;
	int numFiles = argc-5;
	ng.count = argc-5;
	int reqThreads = atoi(argv[1]);
	int resThreads = atoi(argv[2]);

	// int numFiles = 1;
	// ng.count = 1;
	// int reqThreads = 5;
	// int resThreads = 10;
	
	struct timeval start, end;
	int run = 1;
	gettimeofday(&start, NULL);
	
	
	pthread_t requester[reqThreads];
	pthread_t resolver[resThreads];
	
	pthread_mutex_init(&filesToService.muequeue, NULL);
	pthread_mutex_init(&ng.request, NULL);
	pthread_mutex_init(&ng.servicedt, NULL);
	pthread_cond_init(&filesToService.halfFull, NULL);
	pthread_cond_init(&filesToService.halfEmpty, NULL);
	

	init(&filesToService, -1);
	
	if(argc < MINARGS)
	{
		fprintf(stderr, "Not enough arguments\n");
		fprintf(stderr, "Usage is: %s %s\n", argv[0], USAGE);
		run = false;
	}	
	else if(numFiles > MAX_INPUT_FILES)
	{
		fprintf(stderr, "Too many arguments\n");
		fprintf(stderr, "Usage is: %s %s\n", argv[0], USAGE);
		run = false;
	}
	//change to else if bellow
	else if(reqThreads > MAX_REQUESTER_THREADS)
	{
		printf("1\n");
		fprintf(stderr, "Too many requester threads!");
		run = 0;
	}
	else if(resThreads > MAX_RESOLVER_THREADS)
	{
		printf("2\n");
		fprintf(stderr, "Too many resolver threads!");
		run = 0;
	}
	if(run == 0)
	{
		cleanup(&filesToService);
		pthread_mutex_destroy(&filesToService.muequeue);
		pthread_mutex_destroy(&ng.request);	
		return 0;
	}
	for(int i=0; i < reqThreads; i++)
	{
		int reqtemp = pthread_create(&(requester[i]), NULL, req_read_write, (void*)argv[i+5]);
		//int reqtemp = pthread_create(&(requester[i]), NULL, req_read_write, "./input/names1.txt");
		if(reqtemp != 0)
		{
			printf("Error creating requester thread\n");
			return 0;
		}
	}
	for(int i=0; i < resThreads; i++)
	{
		int restemp = pthread_create(&(resolver[i]), NULL, res_read_write, (void *)argv[4]);
		//int restemp = pthread_create(&(resolver[i]), NULL, res_read_write, "serviced.txt");
		if(restemp !=0)
		{
			printf("Error creating resolver thread\n");
			return 0;
		}
	}
	for(int i = 0; i < numFiles; i++)
	{
		pthread_join(requester[i], NULL);
	}
	
	for(int i=0; i < resThreads; i++)
	{
		pthread_join(resolver[i], NULL);
	}

	cleanup(&filesToService);
	pthread_mutex_destroy(&filesToService.muequeue);
	pthread_mutex_destroy(&ng.request);	

	gettimeofday(&end, NULL);
	printf("Main: program completed.\n");
	printf("Total run time: %ld\n", ((end.tv_sec - end.tv_sec)/1000000L +end.tv_usec) - start.tv_usec);
	return 0;
}

void *req_read_write(void *file)
{
	
	nglobal ng;
	int i = 0;
	char ch;
	FILE *fo=fopen("serviced.txt", "a");
	pid_t tid = pthread_self();
	FILE* infile = NULL;
	char* fileName = (char*)file;
	//put fileName bellow
	infile = fopen(fileName, "r");
	if(!infile)
	{
		printf("Error opening file\n");
		return NULL;
	}
	else if(!fo)
	{
		perror("Error opening file\n");
		return NULL;
	}
	char* host = (char*)malloc(CHARBUFF);
	pthread_mutex_lock(&ng.servicedt);
    fprintf(fo, "Thread %d serviced %s\n", tid, "./input/names1.txt");
    pthread_mutex_unlock(&ng.servicedt);

	while(fscanf(infile, INPUTS, host) >0)
	{
		//READ file
		pthread_mutex_lock(&(filesToService.muequeue));
		if(is_full(&filesToService))
		{
			pthread_cond_wait(&filesToService.halfFull, &filesToService.muequeue);
		}
		if(push(&(filesToService), (void*)host) == QUEUE_FAILURE)
		{
			fprintf(stderr, "Queue push fail: %s\n", host);
		}
		pthread_mutex_unlock(&filesToService.muequeue);
		host = (char*) malloc(CHARBUFF);
		
	}
	
	ng.count--;
	fclose(fo);
	fclose(infile);
	free(host);
	host=NULL;
	pthread_exit(NULL);
	return NULL;
}

void *res_read_write(void *file)
{
	
	nglobal ng;
	char firststring[MAX_NAME_LENGTH];
	char* host=(char*) malloc(CHARBUFF);	
	FILE* outfile = NULL;
	char* fileName = (char*)file;
	outfile = fopen(fileName, "w");
	if(!outfile)
	{
		perror("Error opening file\n");
	}	
	
	while(ng.count >0 || !is_empty(&filesToService))
	{
		pthread_mutex_lock(&filesToService.muequeue);
		host = (char*)pop(&filesToService);
		if(host == NULL)
		{
				pthread_mutex_unlock(&filesToService.muequeue);
				pthread_cond_signal(&filesToService.halfFull);
		}
		else
		{
			pthread_mutex_unlock(&filesToService.muequeue);
			if(dnslookup(host, firststring, sizeof(firststring)) == UTIL_FAILURE)
			{
				fprintf(stderr, "Error, dnslookup: %s\n", host);
				strncpy(firststring, "", sizeof(firststring));
			}
			pthread_mutex_lock(&ng.request);
			fprintf(outfile, "%s, %s\n", host, firststring);
			pthread_mutex_unlock(&ng.request);
			free(host);
			host=NULL;
		}
	}
	pthread_mutex_lock(&filesToService.muequeue);
	if(is_empty(&filesToService))
	{
		fclose(outfile);
		pthread_mutex_unlock(&filesToService.muequeue);
		pthread_exit(NULL);
		return NULL;
	}
	else
	{
		fprintf(stderr, "Error resolvinng files");
		fclose(outfile);
		pthread_mutex_unlock(&filesToService.muequeue);
		pthread_exit(NULL);
		return NULL;
	}
}
