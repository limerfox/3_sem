#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <math.h>

#define NUM_THREADS 1
#define NUM_LOOPS 100000000
// #define NUM_LOOPS 100;
#define BILLION 1000000000L

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
unsigned long sum = 0;

typedef struct {
    float from; 
    float to; 
} periodFromTo;

void* part_intergral(void *args) {
	unsigned long i;
    unsigned long intergralPart = 0;
	periodFromTo fromTo = *(periodFromTo*) args;
	for (i = 0; i < NUM_LOOPS/NUM_THREADS; i++) 
    {
		float x = (float)rand()/(float)RAND_MAX * (fromTo.to - fromTo.from) + fromTo.from;
        float y = (float)rand()/(float)RAND_MAX;
        float ys = sinf(x);
        if (y < ys)
        {
            intergralPart += 1;
        }
	}
    pthread_mutex_lock(&mutex);
    sum +=intergralPart;
    pthread_mutex_unlock(&mutex);

	pthread_exit(NULL);
}

int main() {
	pthread_t idThrd[NUM_THREADS];
    periodFromTo idArgs[NUM_THREADS];
    struct timespec start_tm, stop_tm;
    double accum_time;
    int status;
    int status_addr;
	float start=0.0;
	float end=2.0;
    float diff = end/NUM_THREADS;
    srand((unsigned int)time(NULL));
    
    if(clock_gettime(CLOCK_MONOTONIC, &start_tm)== -1)
    {
        perror("clock_gettime");
        return EXIT_FAILURE;
    }
    
    for (size_t i = 0; i < NUM_THREADS; i++)
    {
        idArgs[i].from = (float)i * diff;
        idArgs[i].to = (float)(i + 1) * diff;
        pthread_create(&(idThrd[i]), NULL, part_intergral, (void*) &idArgs[i]);
    }
    
    for (size_t i = 0; i < NUM_THREADS; i++)
    {
	    pthread_join(idThrd[i], (void**) &status_addr);
    }

    if(clock_gettime(CLOCK_MONOTONIC, &stop_tm)== -1)
    {
        perror("clock_gettime");
        return EXIT_FAILURE;
    }

    accum_time = (stop_tm.tv_sec - start_tm.tv_sec) 
    + (double)(stop_tm.tv_nsec - start_tm.tv_nsec)/(double)BILLION;

    printf("%lf seconds\n", accum_time);
	printf("Result of integral: %.9f\n",(float)sum/(float)NUM_LOOPS );
	return 0;
}
