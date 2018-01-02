#include <stdio.h>
#include <mpi.h>
#include <math.h> 
#include <stdlib.h> 







int main(int argc, char **argv){
	//printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n")
	int rank, num_processes;
	double t1,t2,dif;
	double t1f,t2f,diff;
	int iteration=1000;
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

printf("Before Inbuilt Barrier from thread %d of %d\n",rank, num_processes);

	t1=MPI_Wtime();
		
	for(int i=0;i<iteration;i++){
		MPI_Barrier(MPI_COMM_WORLD);

		for(int j=0; j<10000; j++);
		}
	//MPI_Barrier(MPI_COMM_WORLD);

	t2=MPI_Wtime();
	dif=(t2-t1)/iteration;

	//MPI_Reduce(&t1,&t1f,1,MPI_DOUBLE,MPI_MIN,0,MPI_COMM_WORLD);
	//MPI_Reduce(&t2,&t2f,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
	MPI_Reduce(&dif,&diff,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
   // printf("After Inbuilt Barrier from thread %d of %d, time taken: %0.9f us\n",rank, num_processes, dif*pow(10,6));

    if(rank==0)
    	printf("with NOP Time taken to cross Inbuilt barrier: %0.9f us\n", diff*pow(10,6)/num_processes);
	

	MPI_Finalize();
	//printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n")

	return 0;
}
