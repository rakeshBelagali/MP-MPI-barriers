#include <stdio.h>
#include <mpi.h>
#include <math.h> 
#include <stdlib.h>
#include <omp.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h> 

#define TRUE 1
#define FALSE 0
#define BILLION 1000000000L
#define NUM_THREADS_SIZE 12

unsigned int NUM_THREADS = 0;
uint64_t NUM_ITERATIONS = 1000;
//uint64_t NUM_ITERATIONS = 10;


MPI_Status mpi_result;

void d_barrier(int num_processes, int rank){
	int buff = 1;
	int tag = 1;
	int sendto, recv;
	int i;
	for(i=1;i < num_processes; i=i<<1){
		//printf("hiiii\n");
		sendto=(rank+i) % num_processes;
		recv=(rank-i) % num_processes;
		if(recv < 0) {
      		recv += num_processes;
			}

		MPI_Send(&buff,1,MPI_INT,sendto,tag,MPI_COMM_WORLD);
		MPI_Recv(&buff,1,MPI_INT,recv,tag,MPI_COMM_WORLD,&mpi_result);
		
	}
}

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

int main(int argc, char **argv){
	//printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
	int rank, num_processes,thread_num, num_threads;
	double t1,t2,dif;
	double t1f,t2f,diff;
	int iteration=1000;
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    omp_set_num_threads(NUM_THREADS);

	printf("Before Dessimination Barrier from thread %d of %d\n",rank, num_processes);

	t1=MPI_Wtime();
		
	uint64_t diffMPListSum[NUM_THREADS_SIZE+1];
	memset(diffMPListSum, 0, sizeof(diffMPListSum));
	unsigned int threadCount = 0;
	int i = 0;
	for(i=0;i<iteration;i++){

		//RAKESH:: start of openMP code
		//printf("---------------BEGIN OF openMP_MCS-------------\n\n");
		uint64_t diffMP[NUM_THREADS_SIZE+1];
		uint64_t diffMPList[NUM_THREADS_SIZE+1];
		
		memset(diffMP, 0, sizeof(diffMP));
		memset(diffMPList, 0, sizeof(diffMPList));
		
		//unsigned int threadCount = 0;
		int loop = 0;
		for(threadCount=2;threadCount<=12;threadCount+=2) { // take readings for thread count 2,4,6,8,12
			NUM_THREADS = threadCount;
			//printf("=============================================\n");
			//printf("openMP_MCS started with threads %d\n",NUM_THREADS);
			//printf("=============================================\n");
			uint64_t sumClock = 0;
			//printf("This is the serial section\n");
			omp_set_num_threads(NUM_THREADS);
			initBarrier();
			#pragma omp parallel
			{
				// Now we're in the parallel section
				struct timespec start, end;
				int k = 0;
				int ii = 0;
				int num_threads = omp_get_num_threads();
				int thread_num = omp_get_thread_num();
				//for(ii=0;ii<thread_num*1000;ii++) {
		
				//}
				//printf("Before Barrier from thread %d of %d\n",thread_num, num_threads);
				clock_gettime(CLOCK_MONOTONIC, &start);
				for(k=0;k<NUM_ITERATIONS;k++) {
					mcsBarrier(thread_num);
					//clock_nanosleep(CLOCK_MONOTONIC,&temp);
					//nanosleep(&temp,NULL);
				}
				clock_gettime(CLOCK_MONOTONIC, &end);
				//diff[thread_num] = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec - (NUM_ITERATIONS * 100);
				diffMP[thread_num] = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
				//printf("After Barrier from thread %d of %d\n",thread_num, num_threads);
			} // implied barrier
			//printf("Back in the serial section again\n");
			// Resume serial code
			for(loop=0;loop<NUM_THREADS;loop++) {
				sumClock += diffMP[loop];
				//printf("diffMPs of %d = %llu\n",loop,diffMP[loop]);
			}
			sumClock = sumClock/NUM_ITERATIONS;
			//printf("mcsBarrier:: main:: average diffMP taken = %llu\n",sumClock/NUM_THREADS);
			//printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
			diffMPList[threadCount] = sumClock/NUM_THREADS;
			diffMPListSum[threadCount] += diffMPList[threadCount];
		}
		//printf("ProcessID=%d\tThreadCount\tdiffMP\n",rank);
		//for(threadCount=2;threadCount<=12;threadCount+=2) {
		//	printf("ProcessID=%d\t%d\t%llu\n",rank,threadCount,diffMPList[threadCount]);
		//}
		//printf("\n---------------END OF openMP_MCS-------------\n");
		//RAKESH:: end of openMP code

	    d_barrier(num_processes, rank);
	}
	//MPI_Barrier(MPI_COMM_WORLD);

	printf("ProcessID=%d\tThreadCount\tdiffMP\n",rank);
	for(i=2;i<=12;i+=2) {  //change this
		printf("ProcessID=%d\t%d\t%llu\n",rank,i,diffMPListSum[i]/iteration);
	}

	t2=MPI_Wtime();
	dif=(t2-t1)/iteration;

	//MPI_Reduce(&t1,&t1f,1,MPI_DOUBLE,MPI_MIN,0,MPI_COMM_WORLD);
	//MPI_Reduce(&t2,&t2f,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
	MPI_Reduce(&dif,&diff,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
    printf("After Dessimination Barrier from process %d of %d, time taken: %0.9f secs\n",rank, num_processes, dif);
	MPI_Barrier(MPI_COMM_WORLD);
    if(rank==0)
    	printf("Time taken to cross Dessimination barrier: %0.9f \n", diff/num_processes);
	

	MPI_Finalize();
	//printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");

	return 0;
}
