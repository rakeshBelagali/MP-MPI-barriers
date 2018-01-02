#CC=mpiCC
CC=/opt/openmpi-1.4.3-gcc44/bin/mpiCC

all: mcs_mpi d_barrier mpi_inbuilt 

mcs_mpi:mcs_mpi.cpp
	$(CC) mcs_mpi.cpp -o mcs_mpi

d_barrier: d_barrier.cpp
	$(CC) d_barrier.cpp -o d_barrier

mpi_inbuilt: mpi_inbuilt.cpp
	$(CC) mpi_inbuilt.cpp -o mpi_inbuilt


clean:
	rm -f *.o mcs_mpi d_barrier mpi_inbuilt 
