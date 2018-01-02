#include <stdio.h>
#include <mpi.h>
#include <math.h> 
#include <stdlib.h> 

MPI_Status mpi_result;
void d_barrier(int num_processes, int rank){
	int buff = 1;
	int tag = 1;
	int sendto, recv;

	for(int i=1;i < num_processes; i=i<<1){
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




int main(int argc, char **argv){
	//printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n")
	int rank, num_processes;
	double t1,t2,dif;
	double t1f,t2f,diff;
	int iteration=1000;
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

//printf("Before Dessimination Barrier from thread %d of %d\n",rank, num_processes);

	t1=MPI_Wtime();
		
	for(int i=0;i<iteration;i++){
	d_barrier(num_processes, rank);
	for(int j=0; j<10000; j++);

	}
	//MPI_Barrier(MPI_COMM_WORLD);

	t2=MPI_Wtime();
	dif=(t2-t1)/iteration;

	//MPI_Reduce(&t1,&t1f,1,MPI_DOUBLE,MPI_MIN,0,MPI_COMM_WORLD);
	//MPI_Reduce(&t2,&t2f,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
	MPI_Reduce(&dif,&diff,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
    //printf("After Dessimination Barrier from thread %d of %d, time taken: %0.9f us\n",rank, num_processes, dif*pow(10,6));

    MPI_Barrier(MPI_COMM_WORLD);
    if(rank==0)
    	printf("with NOP Time taken to cross Dessimination barrier: %0.9f us \n", diff*pow(10,6)/num_processes);
	

	MPI_Finalize();
	//printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n")

	return 0;
}
