#!/bin/bash

if [ "$#" -eq 0 ]
then
  echo Compute max. memory per CPU for Gadget-2 simulations with PLACEHIGHRESREGION enabled.
  echo 
  echo Usage:
  echo
  echo "   gadgetMaxMem.sh  PM  box  extent  Ncpu"
  echo
  exit
fi

# PM=$1, box=$2, extent=$3, Ncpu=$4 in awk

echo $@
echo $@ |awk '{print "Total mem: ", ( $1^3*12 + (2*$1)^3*16 ) / 1024^3, "GiB"}'
echo $@ |awk '{fact=$2/$3/$4/2; fact=(fact>1)?1:fact;  print "1 CPU mem: ", ( $1^3*12/$4 + (2*$1)^3*16*fact ) / 1024^3, "GiB"}'


