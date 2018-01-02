#include <omp.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0
#define BILLION 1000000000L
#define NUM_THREADS_SIZE 8

unsigned int NUM_THREADS = 0;
uint64_t NUM_ITERATIONS = 10000000;
//uint64_t NUM_ITERATIONS = 10;

typedef struct {
    uint8_t  parentSense;
    uint8_t *parentPointer;
    uint8_t *childPointers[2];
    uint8_t haveChild[4];
    uint8_t childNotReady[4];
    uint8_t dummy;
    uint8_t sense;
    uint8_t id;
} thread_t;

thread_t sharedNodes[NUM_THREADS_SIZE];

void initBarrier() {
    int i,j;
    thread_t *t;
    for(i=0;i<NUM_THREADS;i++) {
        t = &sharedNodes[i];
        //t->parentSense = FALSE;
        t->sense = TRUE;
        t->dummy = 0x0;
        t->id = i;
        for(j=0;j<4;j++) {
            t->haveChild[j] = (4*i+j < (NUM_THREADS-1)) ? TRUE : FALSE;
            t->childNotReady[j] = t->haveChild[j];
        }
        t->parentPointer = (i!=0) ? &sharedNodes[(uint8_t)(floor((i-1)/4))].childNotReady[(i-1)%4] : &t->dummy;
        t->childPointers[0] = (2*i+1 < NUM_THREADS) ? &sharedNodes[2*i+1].parentSense : &t->dummy;
        t->childPointers[1] = (2*i+2 < NUM_THREADS) ? &sharedNodes[2*i+2].parentSense : &t->dummy;
        t->parentSense = FALSE;
    }
}

void mcsBarrier (int id) {
    thread_t *t;
    int j;
    uint8_t numFalses = 0;
    t = &sharedNodes[id];
    while(1){
        numFalses = 0;
        for(j=0;j<4;j++) {
            if(t->childNotReady[j] == FALSE) {
                numFalses++;
            }
        }
        if(numFalses == 4) {
            break;
        }
    }
    for(j=0;j<4;j++) {
        t->childNotReady[j] = t->haveChild[j];
    }
    *(t->parentPointer) = FALSE;
    if(t->id != 0) {
        while (t->parentSense != t->sense);
    }
    *(t->childPointers[0]) = t->sense;
    *(t->childPointers[1]) = t->sense;
    t->sense = !t->sense;
}

int main(int argc, char **argv)
{   
    printf("---------------BEGIN OF openMP_MCS-------------\n\n");
    uint64_t diff[NUM_THREADS_SIZE];
    uint64_t diffSum[NUM_THREADS_SIZE];
    memset(diffSum,0,sizeof(diffSum));
    unsigned int threadCount = 0;
    int loop = 0;
    for(threadCount=2;threadCount<=8;threadCount++) { // take readings for thread count 2,4,6,8
        NUM_THREADS = threadCount;
        printf("=============================================\n");
        printf("openMP_MCS started with threads %d\n",NUM_THREADS);
        printf("=============================================\n");
        uint64_t sumClock = 0;
        printf("This is the serial section\n");
        omp_set_num_threads(NUM_THREADS);
        initBarrier();
        #pragma omp parallel
        {
            // Now we're in the parallel section
            struct timespec start, end, temp;
            temp.tv_sec = 0;
            temp.tv_nsec = 100;
            int k = 0;
            int ii = 0;
            int nop = 0;
            int num_threads = omp_get_num_threads();
            int thread_num = omp_get_thread_num();
            for(ii=0;ii<thread_num*1000;ii++) {
    
            }
            printf("Before Barrier from thread %d of %d\n",thread_num, num_threads);
            clock_gettime(CLOCK_MONOTONIC, &start);
            for(k=0;k<NUM_ITERATIONS;k++) {
                mcsBarrier(thread_num);
                //clock_nanosleep(CLOCK_MONOTONIC,&temp);
                //nanosleep(&temp,NULL);
                
            }
            clock_gettime(CLOCK_MONOTONIC, &end);
            //diff[thread_num] = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec - (NUM_ITERATIONS * 100);
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
        printf("mcsBarrier:: main:: average diff taken = %llu\n",sumClock/NUM_THREADS);
        diffSum[threadCount] = sumClock/NUM_THREADS;
        printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
    }
    printf("NUM_THREADS\tdiff\n");
    for(loop = 0;loop<=8;loop++) {
        printf("%d\t\t%llu\n",loop,diffSum[loop]);
    }
    printf("\n---------------END OF openMP_MCS-------------\n");
    return 0;
}




/*int main(int argc, char **argv)
{
    printf("This is the serial section\n");
    omp_set_num_threads(NUM_THREADS);
    initBarrier();
    #pragma omp parallel
    {
        // Now we're in the parallel section
        int num_threads = omp_get_num_threads();
        int thread_num = omp_get_thread_num();
        printf("Before Barrier from thread %d of %d\n",thread_num, num_threads);
        mcsBarrier(thread_num);
        printf("After Barrier from thread %d of %d\n",thread_num, num_threads);

    } // implied barrier

    // Resume serial code
    printf("Back in the serial section again\n");
    return 0;
}*/
