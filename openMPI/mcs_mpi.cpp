#include <stdio.h>
#include <mpi.h>
#include <math.h> 
#include <stdlib.h> 
#include <time.h>
using namespace std;



typedef struct tree{
	int haveChild[4];
	int childNotReady[4];
	int upParent;
	int downParent;
	int childPointer[2];
}tree_node;

void mcs_init(int rank,int num_processes);
void mcs_barrier(int rank,int num_processes);
void mcs_finalise(void);

tree_node *node;

void mcs_init(int rank,int num_processes){
	node=new tree_node;

	for(int i = 0;i < 4; i++){
		if(rank*4+i+1 < num_processes){
			node->haveChild[i] = 1;
			node->childNotReady[i] = rank*4+i+1;
		}
		else{
			node->haveChild[i] = 0;
			node->childNotReady[i] = -1;
		}
	}

	for(int j = 0;j < 2; j++){
		if(rank*2+j+1 < num_processes){
			node->childPointer[j] = rank*2+j+1;
		} 
 		else{
			node->childPointer[j] = -1;
		}
	}

	if(rank != 0){
		node->upParent = floor((rank-1)/4);
		node->downParent = floor((rank-1)/2);
	}
	else
	{
		node->upParent = -1;
		node->downParent = -1;
	}

}

MPI_Status mpi_result;
void mcs_barrier(int rank, int num_processes){
	int buff = 1;
	int tag = 1;

	for(int i = 0; i < 4; i++){
		if(node->haveChild[i] != 0)
			MPI_Recv(&buff,1,MPI_INT,node->childNotReady[i],tag, MPI_COMM_WORLD,&mpi_result);
	}

	if(rank>0){
		MPI_Send(&buff,1,MPI_INT,node->upParent,tag,MPI_COMM_WORLD);
		MPI_Recv(&buff,1,MPI_INT,node->downParent,tag,MPI_COMM_WORLD,&mpi_result);
	}

	for(int i = 0;i < 2; i++){
		if(node->childPointer[i] != -1){
			MPI_Send(&buff,1,MPI_INT,node->childPointer[i],tag,MPI_COMM_WORLD);
		}
	}
}


void mcs_finalise(void){
	free(node);
}

int main(int argc, char ** argv){
//printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
	int rank, num_processes;
	double t1,t2,dif;
	double t1f,t2f,diff;
	int iteration=1000;

	MPI_Init(&argc,&argv);

	MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	mcs_init(rank,num_processes);

	//printf("Before MCS Barrier from thread %d of %d\n",rank, num_processes);

	t1=MPI_Wtime();
	
  
	for(int i=0;i<iteration;i++){
	mcs_barrier(rank,num_processes);
	for(int j=0; j<10000; j++);

   // MPI_Barrier(MPI_COMM_WORLD);
	}
	

    t2=MPI_Wtime();
    dif=(t2-t1)/iteration;
    //MPI_Reduce(&t1,&t1f,1,MPI_DOUBLE,MPI_MIN,0,MPI_COMM_WORLD);
	//MPI_Reduce(&t2,&t2f,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
	MPI_Reduce(&dif,&diff,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
    //printf("After MCS Barrier from thread %d of %d, time taken: %0.9f us\n",rank, num_processes, dif*pow(10,6));

	mcs_finalise();

	MPI_Barrier(MPI_COMM_WORLD);
	if(rank==0)
    	printf("with NOP Avg time taken to cross MCS barrier: %0.9f us\n", diff*pow(10,6)/num_processes);

	MPI_Finalize();
	//printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
	return 0;
}
