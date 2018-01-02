#
#OMPFLAGS = -fopenmp 
#OMPLIBS = -lgomp -lm 

#CC = gcc 
#CPPFLAGS = -g -Wall
#LDFLAGS = -g -Wall
#LDLIBS = $(OMPLIBS)

#MPICC = mpicc
#MPICC = mpiCC
#MPICH = /usr/lib64/openmpi/1.4-gcc
#CFLAGS = -I$(MPICH)/include

#all: mpi_mp_try1

#mpi_mp_try1: mpi_mp_try1.o
#	$(MPICC) -o $@ $(LDFLAGS) $^ $(LDLIBS) $(OMPLIBS)

#mpi_mp_try1.o: mpi_mp_try1.c
#	$(CC) -c -o $@ $(CPPFLAGS) $(CFLAGS) $(OMPFLAGS) $^	

#clean:
#	rm -f *.o mpi_mp_try1

CC=mpiCC

CC1=/opt/openmpi-1.4.3-gcc44/bin/mpicc

all: mpi_mp_try1

mpi_mp_try1: mpi_mp_try1.c
	$(CC1) -o mpi_mp_try1 -g -fopenmp mpi_mp_try1.c -lm

clean:
	rm -f *.o mpi_mp_try1


