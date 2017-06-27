# for MareNostrum using LSF

let coresPerNode=16
let memPerCore=1600
let OMPtasks=1

maxNodes=0

submitCommand="bsub <"

echo "seq = 1" >> Makefile
rule () 
{
    #echo -e "\t@echo $1 $2 \$(seq).seq" >> Makefile
    echo -e "\t@cp $2 \$(seq).seq" >> Makefile
    echo -e "\t@echo \"$1 \$\$(( \$(seq) + 1 )).seq\" >> \$(seq).seq" >> Makefile
    echo -e "\t@\$(eval seq = \$(shell expr \$(seq) + 1 ))\n" >> Makefile
}

firstRule="$firstRule
	@rm -rf *.seq
	@echo \"To submit all the tasks, run bsub < 1.seq\""

function pow2 {
    local x=1
    for (( y=$1 ; $y > 0; y=y-1 )) ; do
        let x=$x*2
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
ncl=$(log2 nc)
echo $(pow2 $ncl)
}

nodesFromCores ()
{
echo $1 | awk 'function ceil(valor)
   {
      return (valor == int(valor)) ? valor : int(valor)+1
   }
   {print ceil($1/'$coresPerNode')}'
}

getQueue ()
{
if [ $ncores -le 64 ]; then
   queue=class_a
else
   queue=class_a
fi
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
getQueue
template="
#!/bin/bash
#BSUB -n $ncores
#BSUB -J ginnungagap
#BSUB -W 01:00
#BSUB -q $queue
#BSUB -x
#BSUB -eo output_%J.err
#BSUB -oo output_%J.out

export OMP_NUM_THREADS=$OMPtasks
mpirun ./refineGrid ref_x_$m.ini
mpirun ./refineGrid ref_z_$m.ini
mpirun ./refineGrid ref_y_$m.ini
"
batwriter $1
getMax
}

refCutWNBatCreator ()
{
ncores=$(coresFromMem $mem)
nodes=$(nodesFromCores $ncores)
let ncores=nodes*coresPerNode
getQueue
template="
#!/bin/bash
#BSUB -n $ncores
#BSUB -J ginnungagap
#BSUB -W 01:00
#BSUB -q $queue
#BSUB -x
#BSUB -eo output_%J.err
#BSUB -oo output_%J.out

export OMP_NUM_THREADS=$OMPtasks
mpirun ./refineGrid ref_wn_cut_$mprev.ini
"
batwriter $1
getMax
}


rscBatCreator ()
{
ncores=$(coresFromMem $mem)
nodes=$(nodesFromCores $ncores)
let ncores=nodes*coresPerNode
getQueue
template="
#!/bin/bash
#BSUB -n $ncores
#BSUB -J ginnungagap
#BSUB -W 01:00
#BSUB -q $queue
#BSUB -eo output_%J.err
#BSUB -oo output_%J.out
#BSUB -x

export OMP_NUM_THREADS=$OMPtasks
mpirun ./realSpaceConstraints $rscIni
"
batwriter $1
getMax
}

ggpBatCreator ()
{
ncores=$(coresFromMem $mem)
nodes=$(nodesFromCores $ncores)
let ncores=nodes*coresPerNode
getQueue
template="
#!/bin/bash
#BSUB -n $ncores
#BSUB -J ginnungagap
#BSUB -W 01:00
#BSUB -q $queue
#BSUB -eo output_%J.err
#BSUB -oo output_%J.out
#BSUB -x

export OMP_NUM_THREADS=$OMPtasks
mpirun ./ginnungagap $ggpIni
"
batwriter $1
getMax
}

genicsBatCreator ()
{
ncores=$coresPerNode
nodes=1
template="
#!/bin/bash
#BSUB -n $ncores
#BSUB -J ginnungagap
#BSUB -W 01:00
#BSUB -q $queue
#BSUB -x

export OMP_NUM_THREADS=$ncores
mpirun -n 1 ./generateICs $genIni
"
batwriter $1
getMax
}

writeRecommendations ()
{
echo "Maximal number of nodes: $maxNodes" >> recommendations.txt
}
