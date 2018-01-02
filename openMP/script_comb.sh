#PBS -q cs6210
#PBS -l nodes=1:sixcore
#PBS -l walltime=00:15:00
#PBS -N cs6210_rakeshB_openMP
#PBS -m abe
#PBs -M rbelagali3@gatech.edu
pushd /nethome/rbelagali3/cs6210/project-2/openMP/openMP_temp
./openMP_MCS
./openMP_sense
./openMP_builtin
popd
