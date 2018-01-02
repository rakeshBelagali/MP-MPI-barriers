#PBS -q cs6210
#PBS -l nodes=4:sixcore 
#PBS -l walltime=00:20:00 
#PBS -N rakeshBcomb 

pushd /nethome/rbelagali3/cs6210/project-2/MPI
rm log4
for number1 in {4..4}; do
    echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
	echo "NUM_Processes = $number1" 
    echo "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	OMPI_MCA_mpi_yield_when_idle=0 /opt/openmpi-1.4.3-gcc44/bin/mpirun --hostfile $PBS_NODEFILE -np $number1 /nethome/rbelagali3/cs6210/project-2/MPI/mpi_mp_try1 >>log4
	echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
done

popd