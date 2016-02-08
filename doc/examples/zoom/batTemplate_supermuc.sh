# for SUPERMUC using LOADLEVELER

variant=hw

if [ $variant == 'hw' ]; then
  let coresPerNode=28
  let memPerCore=58000/$coresPerNode
  let OMPtasks=2
  LIBS=/gpfs/work/pr87yi/di29xaz/lib_hw
else
  let coresPerNode=16
  let memPerCore=26000/$coresPerNode
  let OMPtasks=2
  LIBS=/gpfs/work/pr87yi/di29xaz/lib
fi

maxNodes=0

submitCommand=llsubmit
echo "seq = 1" >> Makefile
rule () 
{
    #echo -e "\t@echo $1 $2 \$(seq).seq" >> Makefile
    echo -e "\t@cp $2 \$(seq).seq" >> Makefile
    echo -e "\t@echo $1 \$\$(( \$(seq) + 1 )).seq >> \$(seq).seq" >> Makefile
    echo -e "\t@\$(eval seq = \$(shell expr \$(seq) + 1 ))\n" >> Makefile
}

firstRule="$firstRule
	@rm -rf *.seq
	@echo To submit all the tasks, run llsubmit 1.seq"

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

class_setup ()
{
class='test'
limit='00:30:00'
if [ $nodes -gt 32 ]; then
  class='general'
  limit='3:00:00'
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
class_setup
template="
#!/bin/bash
#@ job_name = ginnungagap
#@ job_type = parallel
#@ class = $class
#@ node = $nodes
#@ total_tasks = $ncores
#@ wall_clock_limit = $limit
#@ network.MPI = sn_all,not_shared,us
#@ initialdir = $PWD
#@ notification = never
#@ output = run\$(jobid).out
#@ error = run\$(jobid).err
#@ queue

. /etc/profile
. /etc/profile.d/modules.sh
module add gsl
export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:$LIBS

export MP_DEBUG_TIMEOUT_SECONDS=1200 

export OMP_NUM_THREADS=$OMPtasks
poe ./refineGrid ref_x_$m.ini
poe ./refineGrid ref_z_$m.ini
poe ./refineGrid ref_y_$m.ini
"
batwriter $1
getMax
}

rscBatCreator ()
{
ncores=$(coresFromMem $mem)
nodes=$(nodesFromCores $ncores)
let ncores=nodes*coresPerNode
class_setup
template="
#!/bin/bash
#@ job_name = ginnungagap
#@ job_type = parallel
#@ class = $class
#@ node = $nodes
#@ total_tasks = $ncores
#@ wall_clock_limit = $limit
##                    0 h 20 min 30 secs

#@ network.MPI = sn_all,not_shared,us
#@ initialdir = $PWD
#@ notification = never
#@ output = run\$(jobid).out
#@ error = run\$(jobid).err
#@ queue

. /etc/profile
. /etc/profile.d/modules.sh
module add gsl
export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:$LIBS

export MP_DEBUG_TIMEOUT_SECONDS=1200 

export OMP_NUM_THREADS=$OMPtasks
poe ./realSpaceConstraints $rscIni
"
batwriter $1
getMax
}

ggpBatCreator ()
{
ncores=$(coresFromMem $mem)
nodes=$(nodesFromCores $ncores)
let ncores=nodes*coresPerNode
class_setup
template="
#!/bin/bash
#@ job_name = ginnungagap
#@ job_type = parallel
#@ class = $class
#@ node = $nodes
#@ total_tasks = $ncores
#@ wall_clock_limit = $limit
##                    0 h 20 min 30 secs

#@ network.MPI = sn_all,not_shared,us
#@ initialdir = $PWD
#@ notification = never
#@ output = run\$(jobid).out
#@ error = run\$(jobid).err
#@ queue

. /etc/profile
. /etc/profile.d/modules.sh
module add gsl
export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:$LIBS

export MP_DEBUG_TIMEOUT_SECONDS=1200 

export OMP_NUM_THREADS=$OMPtasks
poe ./ginnungagap $ggpIni
"
batwriter $1
getMax
}

genicsBatCreator ()
{
ncores=1
nodes=1
let OMP=coresPerNode
class_setup
template="
#!/bin/bash
#@ job_name = ginnungagap
#@ job_type = parallel
#@ class = $class
#@ node = $nodes
#@ total_tasks = $ncores
#@ wall_clock_limit = $limit
##                    0 h 20 min 30 secs

#@ network.MPI = sn_all,not_shared,us
#@ initialdir = $PWD
#@ notification = never
#@ output = run\$(jobid).out
#@ error = run\$(jobid).err
#@ queue

. /etc/profile
. /etc/profile.d/modules.sh
module add gsl
export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:$LIBS

export MP_DEBUG_TIMEOUT_SECONDS=1200 

export OMP_NUM_THREADS=$OMP

poe ./generateICs $genIni
"
batwriter $1
getMax
}

writeRecommendations ()
{
echo "Maximal number of nodes: $maxNodes" >> recommendations.txt
}
