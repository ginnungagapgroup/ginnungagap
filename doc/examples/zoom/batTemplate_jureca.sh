# for JURECA using SLURM

let coresPerNode=16
let memPerCore=108000/$coresPerNode
let OMPtasks=2

maxNodes=0

coresFromMem ()
{
echo $1 | awk 'function ceil(valor)
   {
      return (valor == int(valor)) ? valor : int(valor)+1
   }
   {print ceil($1/'$memPerCore')}'
}

nodesFromCores ()
{
echo $1 | awk 'function ceil(valor)
   {
      return (valor == int(valor)) ? valor : int(valor)+1
   }
   {print ceil($1/'$coresPerNode')}'
}

getMax ()
{
if [ $nodes -gt $maxNodes ]; then
  maxNodes=$nodes
fi
}

refBatCreator ()
{
ncores=$(coresFromMem $mem)
nodes=$(nodesFromCores $ncores)
let ncores=nodes*coresPerNode
template="
export OMP_NUM_THREADS=$OMPtasks
srun --ntasks=$ncores --nodes=$nodes --cpus-per-task=$OMPtasks ./refineGrid ref_x_$m.ini
srun --ntasks=$ncores --nodes=$nodes --cpus-per-task=$OMPtasks ./refineGrid ref_z_$m.ini
srun --ntasks=$ncores --nodes=$nodes --cpus-per-task=$OMPtasks ./refineGrid ref_y_$m.ini
"
if [ $doDelta == 'true' ]; then
    template="$template
srun --ntasks=$ncores --nodes=$nodes --cpus-per-task=$OMPtasks ./refineGrid ref_d_$m.ini
"
fi
batwriter $1
getMax
}

rscBatCreator ()
{
ncores=$(coresFromMem $mem)
nodes=$(nodesFromCores $ncores)
let ncores=nodes*coresPerNode
template="
export OMP_NUM_THREADS=$OMPtasks
srun --ntasks=$ncores --nodes=$nodes --cpus-per-task=$OMPtasks ./realSpaceConstraints $rscIni
"
batwriter $1
getMax
}

ggpBatCreator ()
{
ncores=$(coresFromMem $mem)
nodes=$(nodesFromCores $ncores)
let ncores=nodes*coresPerNode
template="
export OMP_NUM_THREADS=$OMPtasks
srun --ntasks=$ncores --nodes=$nodes --cpus-per-task=$OMPtasks ./ginnungagap $ggpIni
"
batwriter $1
getMax
}

genicsBatCreator ()
{
ncores=1
nodes=1
template="
export OMP_NUM_THREADS=$OMPtasks
srun --ntasks=$ncores --nodes=$nodes --cpus-per-task=$OMPtasks ./generateICs $genIni
"
batwriter $1
getMax
}

writeRecommendations ()
{
echo "Maximal number of nodes: $maxNodes" >> recommendations.txt
}
