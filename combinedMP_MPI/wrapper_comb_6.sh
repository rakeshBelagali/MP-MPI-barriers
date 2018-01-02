#PBS -q cs6210
#PBS -l nodes=6:sixcore 
#PBS -l walltime=00:20:00 
#PBS -N rakeshBcomb 

pushd /nethome/rbelagali3/cs6210/project-2/MPI
rm log6
for number1 in {6..6}; do
    echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
	echo "NUM_Processes = $number1" 
    echo "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
	OMPI_MCA_mpi_yield_when_idle=0 /opt/openmpi-1.4.3-gcc44/bin/mpirun --hostfile $PBS_NODEFILE -np $number1 /nethome/rbelagali3/cs6210/project-2/MPI/mpi_mp_try1 >>log6
	echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
done

popd