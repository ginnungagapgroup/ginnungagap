# for SuperMUC-NG using SLURM

let coresPerNode=32
let memPerCore=80000/$coresPerNode
let OMPtasks=1

maxNodes=0

function pow2 {
    local x=1
    for (( y=$1 ; $y > 0; y=y-1 )) ; do
        let x=$x*2
    done
    echo $x
}
# log2 for computing levels
function log2l {
    local x=0
    local inval=$1
    for (( y=$inval-1 ; $y > 0; y >>= 1 )) ; do
        let x=$x+1
    done
    echo $x
}

coresFromMem1 ()
{
echo $1 | awk 'function ceil(valor)
   {
      return (valor == int(valor)) ? valor : int(valor)+1
   }
   {print ceil($1/'$memPerCore')}'
}

coresFromMem ()
{
nc=$(coresFromMem1 $1)
let nc=$nc/15
ncl=$(log2l nc)
(>&2 echo $nc $ncl $(pow2 $ncl))
nc=$(pow2 $ncl)
let nc=$nc*15
echo $nc
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
mpiexec -n $ncores ./refineGrid ref_x_$m.ini
mpiexec -n $ncores ./refineGrid ref_z_$m.ini
mpiexec -n $ncores ./refineGrid ref_y_$m.ini
"
if [ $doDelta == 'true' ]; then
    template="$template
mpiexec -n $ncores ./refineGrid ref_d_$m.ini
"
fi
batwriter $1
getMax
}

refCutWNBatCreator ()
{
ncores=$(coresFromMem $mem)
nodes=$(nodesFromCores $ncores)
let ncores=nodes*coresPerNode
template="
export OMP_NUM_THREADS=$OMPtasks
mpiexec -n $ncores ./refineGrid ref_wn_cut_$mprev.ini
"
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
mpiexec -n $ncores ./realSpaceConstraints $rscIni
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
mpiexec -n $ncores ./ginnungagap $ggpIni
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
mpiexec -n $ncores ./generateICs $genIni
"
batwriter $1
getMax
}

writeRecommendations ()
{
echo "Maximal number of nodes: $maxNodes" >> recommendations.txt
}
