#include <omp.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>

#define TRUE 1
#define FALSE 0
#define BILLION 1000000000L
#define NUM_ITERATIONS 10000000
#define NUM_THREADS_SIZE  8

unsigned int NUM_THREADS = 0;
unsigned int count = 0;
bool sense = TRUE;
//bool localSense[NUM_THREADS];

void senseBarrier (int id, bool *local) {
    *local = !*local;
    if (__sync_fetch_and_sub (&count, 1) == 1) {
        count = NUM_THREADS;
        sense = *local;
    } else {
        while (sense != *local);
    }
}

int main(int argc, char **argv)
{   
    printf("---------------BEGIN OF openMP_sense-------------\n");
    uint64_t diff[NUM_THREADS_SIZE];
    uint64_t diffSum[NUM_THREADS_SIZE];
    memset(diffSum,0,sizeof(diffSum));
    unsigned int threadCount = 0;
    int loop = 0;
    for(threadCount=2;threadCount<=8;threadCount++) {
        NUM_THREADS = threadCount;
        count = NUM_THREADS;
        printf("=============================================\n");
        printf("openMP_sense started with threads %d\n",NUM_THREADS);
        printf("=============================================\n");
        uint64_t sumClock = 0;
        printf("This is the serial section\n");
        omp_set_num_threads(NUM_THREADS);
        //init
        #pragma omp parallel
        {
            // Now we're in the parallel section
            bool localSense = TRUE;
            struct timespec start, end;
            int i = 0;
            //int ii = 0;
            int nop = 0;
            int num_threads = omp_get_num_threads();
            int thread_num = omp_get_thread_num();
            for(i=0;i<thread_num*1000;i++) {
    
            }
            printf("Before Barrier from thread %d of %d\n",thread_num, num_threads);
            clock_gettime(CLOCK_MONOTONIC, &start);
            for(i=0;i<NUM_ITERATIONS;i++) {
                senseBarrier(thread_num,&localSense);
                
            }
            clock_gettime(CLOCK_MONOTONIC, &end);
            diff[thread_num] = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
            printf("After Barrier from thread %d of %d\n",thread_num, num_threads);
        } // implied barrier
        printf("Back in the serial section again\n");
        // Resume serial code
        for(loop=0;loop<NUM_THREADS;loop++) {
            sumClock += diff[loop];
            printf("diffs of %d = %llu\n",loop,diff[loop]);
        }
        sumClock = sumClock/NUM_ITERATIONS;
        printf("senseBarrier:: main:: average diff taken = %llu\n",sumClock/NUM_THREADS);
        diffSum[threadCount] = sumClock/NUM_THREADS;
        printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
    }
    printf("NUM_THREADS\tdiff\n");
    for(loop = 0;loop<=8;loop++) {
        printf("%d\t\t%llu\n",loop,diffSum[loop]);
    }
    printf("---------------END OF openMP_sense-------------\n");
    return 0;
}
