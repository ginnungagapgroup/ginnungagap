#!/bin/bash

shopt -s extglob

version=06.02.2016

###########################################

# Templates for various .ini files

### .ini creator for realSpaceConstraints
rscCreator ()
{
# find out WN type
wnType=hdf5
if [ $wnprev == $wnStartFile ]; then
   wnType=$wnStartType
fi
template="
[Setup]
boxsizeInMpch = $Box
inputDim1D = $mprev
outputDim1D = $m
useFileForInput = true
readerSecName = inputReader
writerSecName = outputWriter
seedIn = 0
seedOut = ${seedArr[$i]}

[inputReader]
type = $wnType
prefix = $wnprev

[outputWriter]
type = hdf5
prefix = $wnPure
overwriteFileIfExists = true
writerSection = writeHDF

[writeHDF]
doChunking = $doChunking
chunkSize = $chunk $chunk $chunk
"
iniwriter $1
}

### .ini creator for refineGrid - only for downgrading velocity fields!
refineCreatorDowngrade ()
{
template="
[Setup]
boxsizeInMpch = $Box
inputDim1D = $mnext
outputDim1D = $m
readerSecName = inputReader
writerSecName = outputWriter
varName = velx
addFields = false

[inputReader]
type = hdf5
prefix = $velnext

[outputWriter]
type = hdf5
prefix = $velPure
overwriteFileIfExists = true
writerSection = writeHDF

[writeHDF]
doChunking = $doChunking
chunkSize = $chunk $chunk $chunk
"
iniwriter_multi $1
}

### .ini creator for refineGrid - add large scale and small scale fields
refineCreatorAdd ()
{
template="
[Setup]
boxsizeInMpch = $Box
inputDim1D = $startMesh
outputDim1D = $m
readerSecName = inputReader
readerAddSecName = reader2
writerSecName = outputWriter
varName = velx
addFields = true

[inputReader]
type = hdf5
prefix = $velLarge
readerSection = readLarge

[reader2]
type = hdf5
prefix = $velSmall
readerSection = readSmall

[outputWriter]
type = hdf5
prefix = $velPure
overwriteFileIfExists = true
writerSection = write

[readLarge]
doPatch = $doPatchLarge
patchSection = PatchLarge

[readSmall]
doPatch = $doPatchThis
patchSection = PatchSmall

[write]
doChunking = $doChunking
chunkSize = $chunk $chunk $chunk
doPatch = $doPatchThis
patchSection = PatchSmall

[PatchLarge]
unit = cells
patchLo = $patchLoLarge
patchDims = $patchDimsLarge

[PatchSmall]
unit = cells
patchLo = $patchLoThis
patchDims = $patchDimsThis
"
iniwriter_multi $1
}

### .ini creator for ginnungagap
ggpCreator ()
{
# find out WN type
wnType=hdf5
if [ $wn == $wnStartFile ]; then
   wnType=$wnStartType
fi

# do we use power spectrum from file?
if [ -z "$powerSpectrumFileName" ]; then
   SF='#'
   TF=''
else
   TF='#'
   SF=''
fi

template="
[Ginnungagap]
# this is different for each level:
dim1D = $m

boxsizeInMpch = $Box
zInit = $zInit
gridName = testGrid
normalisationMode = sigma8

doLargeScale = $doLargeScale
doSmallScale = $doSmallScale
cutoffScale = $cutoffScale

[Output]
type = hdf5
path = ./
# this is different for each level:
prefix = $velPrefix$m

overwriteFileIfExists = true
writerSection = OutputHDF5

[OutputHDF5]
suffix = .h5
doChunking = $doChunking
chunkSize = $chunk $chunk $chunk
doPatch = $doPatchThis
patchSection = Patch

[Patch]
unit = cells
patchLo = $patchLoThis
patchDims = $patchDimsThis

[WhiteNoise]
useFile = $useFileForWn
dumpWhiteNoise = $dumpWhiteNoise
rngSectionName = rng
readerSection = WhiteNoiseReader
writerSection = WhiteNoiseWriter

[WhiteNoiseReader]
type = $wnType
# this is different for each level:
prefix = $wn

[WhiteNoiseWriter]
type = hdf5
path = ./
prefix = $wnPure
doChunking = $doChunking
chunkSize = $chunk $chunk $chunk

[rng]
generator = 4
numStreamsTotal = 16
randomSeed = ${seedArr[$i]}

[MPI]
nProcs = 1  0  0

[Cosmology]
modelOmegaRad0 = $modelOmegaRad0
modelOmegaLambda0 = $modelOmegaLambda0
modelOmegaMatter0 = $modelOmegaMatter0
modelOmegaBaryon0 = $modelOmegaBaryon0
modelHubble = $modelHubble
modelSigma8 = $modelSigma8
modelNs = $modelNs
# You can use built-in transfer function:
${TF}powerSpectrumKmin = $powerSpectrumKmin
${TF}powerSpectrumKmax = $powerSpectrumKmax
${TF}powerSpectrumNumPoints = $powerSpectrumNumPoints
${TF}transferFunctionType = $transferFunctionType
# or a file with the power spectrum:
${SF}powerSpectrumFileName = $powerSpectrumFileName

"
iniwriter $1
}


### .ini creator for generateICs
genicsCreator ()
{
# do we use power spectrum from file?
if [ -z "$powerSpectrumFileName" ]; then
   SF='#'
   TF=''
else
   TF='#'
   SF=''
fi

template="
[Ginnungagap]
# this is different for each level:
dim1D = $m

boxsizeInMpch = $Box
zInit = $zInit
gridName = testGrid
normalisationMode = sigma8

[Patch]
unit = cells
patchLo = $patchLoThis
patchDims = $patchDimsThis

[Cosmology]
modelOmegaRad0 = $modelOmegaRad0
modelOmegaLambda0 = $modelOmegaLambda0
modelOmegaMatter0 = $modelOmegaMatter0
modelOmegaBaryon0 = $modelOmegaBaryon0
modelHubble = $modelHubble
modelSigma8 = $modelSigma8
modelNs = $modelNs
# You can use built-in transfer function:
${TF}powerSpectrumKmin = $powerSpectrumKmin
${TF}powerSpectrumKmax = $powerSpectrumKmax
${TF}powerSpectrumNumPoints = $powerSpectrumNumPoints
${TF}transferFunctionType = $transferFunctionType
# or a file with the power spectrum:
${SF}powerSpectrumFileName = $powerSpectrumFileName

[GenerateICs]
ginnungagapSection = Ginnungagap
doGas = $doGas
doLongIDs = $doLongIDs
autoCenter = $autoCenter
useKpc = $useKpc
shift = $boxShift
inputSection = GenicsInput
outputSection = GenicsOutput
cosmologySection = Cosmology
maskSection = Mask
hierarchySection = Hierarchy
# this is different for each level:
zoomLevel = $zoomLevel

$typesForLevels

[Mask]
maskLevel = $maskLevel
minLevel = $minLevel
maxLevel = $maxLevel
tileLevel = $tileLevel
readerType = legacy
readerSection = Lare

[Hierarchy]
#min. level dim = 256 = 2^8
#max. level dim = 8192 = 2^13
#hence the max. level number is 13, if they are numbered from 2^0 = 1.
numLevels = 21
minDim1D = $minDim1D
factor = 2

[Lare]
hasHeader = false
fileName = $maskFile
ngrid = $maskMesh $maskMesh $maskMesh

[GenicsInput]
velxSection = GenicsInput_velx
velySection = GenicsInput_vely
velzSection = GenicsInput_velz
doPatch = $doPatchThis
patchSection = Patch

[GenicsInput_velx]
type = hdf5
# this is different for each level:
prefix = $velPrefix$m

qualifier = _velx
suffix = .h5

[GenicsInput_vely]
type = hdf5
# this is different for each level:
prefix = $velPrefix$m
qualifier = _vely
suffix = .h5

[GenicsInput_velz]
type = hdf5
# this is different for each level:
prefix = $velPrefix$m
qualifier = _velz
suffix = .h5

[GenicsOutput]
#use > 1 if file size is > 4Gb.
$numFiles
prefix = $gadgetPrefix
"
iniwriter $1
}


###########################################

# Utility functions

# .ini parser function 
cfg_parser ()
{
    ini="$(<$1)"                # read the file
    IFS=$'\n' && ini=( ${ini} ) # convert to line-array
    ini=( ${ini[*]//;*/} )      # remove comments with ;
    ini=( ${ini[*]//\[*/} )     # remove comments with [
    ini=( ${ini[*]//#*/} )      # remove comments with #
    ini=( ${ini[*]/\    =/=} )  # remove tabs before =
    ini=( ${ini[*]/=\   /=} )   # remove tabs be =
    ini=( ${ini[*]/%+(\ )/} )   # remove blanks at the end of line (requires extglob)
    ini=( ${ini[*]/\ =\ /=} )   # remove anything with a space around =
    ini=( ${ini[*]//=/=\"} )    # add " at value string beginning
    ini=( ${ini[*]/%/\"} )      # add " at the end of value string
    #echo "${ini[*]}"           # uncomment this for debug
    eval "$(echo "${ini[*]}")" # eval the result
}

# Makefile rules syntax
rule ()
{
    echo -e "\t@$1 $2\n" >> Makefile
}

# log2 for computing levels
function log2 {
    local x=0
    local inval=1
    let inval=$1/$minDim1D
    for (( y=$inval-1 ; $y > 0; y >>= 1 )) ; do
        let x=$x+1
    done
    echo $x
}

# compute minimal dimension for Hierarchy
function getMinDim {
    for (( y=$1 ; $y%2 == 0; y >>= 1 )) ; do
        :
    done
    echo $y
}

# .ini updater
update ()
{
   if [ -e $1 ] ; then
   if ! diff $1.tmp $1 >/dev/null ; then
      cp $1.tmp $1
      echo "Updated file $1"
   fi
   else
      cp $1.tmp $1
   fi
   rm $1.tmp
}

iniwriter ()
{
   echo "$template" > $1.tmp
   update $1
}

batwriter ()
{
   echo "echo === Executing $1 ===" >$1.tmp
   echo "$template" >> $1.tmp
   update $1
   chmod a+x $1
}


iniwriter_multi ()
{
   echo "$template" > $1.tmp
   update $1
   sed 's|velx|vely|' $1 > ${1/x/y}.tmp
   sed 's|velx|velz|' $1 > ${1/x/z}.tmp
   update ${1/x/y}
   update ${1/x/z}
}

compute_mem ()
{
   echo $1 | awk '{print int($1^3*4*2/1e6)}'
}

compute_patch ()
{
   awk 'BEGIN {x1='$maskMesh'; y1='$maskMesh'; z1='$maskMesh'; x2=0; y2=0; z2=0}
   {if($8<x1) x1=$8; if($8>x2) x2=$8
   if($9<y1) y1=$9; if($9>y2) y2=$9
   if($10<z1) z1=$10; if($10>z2) z2=$10}
   END {dx=x2-x1+1; dy=y2-y1+1; dz=z2-z1+1
   if(dx>dy) d=dx; else d=dy
   if(d<dz) d=dz
   ds=('$maxLevel'-'$maskLevel')
   print "patchLo=\"",x1-ds,y1-ds,z1-ds,"\""
   print "patchDims=\"",d+2*ds,d+2*ds,d+2*ds,"\""}' $maskFile
}

writeRecommendations ()
{
  :
}

#################################################

# Main part of code starts here

echo "Ginnungagap prepare_ini.sh script version $version"
echo

# set defaults

# read .ini into variables

cfg_parser $1
IFS=$' '

# make arrays just in case
meshArr=( ${meshes} )
seedArr=( ${seeds} )

# some standard choice
velSuffix=.h5
wnSuffix=.h5

echo "# Makefile for running GINNUNGAGAP
# prepare_ini version $version" > Makefile

firstRule="first-rule :	
	@echo prepare_ini version $version"

source $batTemplate

# Main loop:
#   fill Makefile with rules
#   create .ini files
#   create batch files


# making some variables for all iterations

velStart=$velPath$velPrefix$startMesh''_velx$velSuffix
velLarge=$velPrefix$startMesh''_large_velx$velSuffix

minMesh=${meshArr[0]}
maxMesh=${meshArr[${#meshArr[@]}-1]}
maxMem=0
minDim1D=$(getMinDim minMesh)
minLevel=$(log2 minMesh)
maxLevel=$(log2 maxMesh)
if [ $minLevel -eq $maxLevel ]; then
   maskLevel=$minLevel
else
   maskLevel=$(log2 maskMesh)
fi
typesForLevels=''
numFiles=''
gadgetTypes=( ${gadgetTypes} )
gadgetNFiles=( ${gadgetNFiles} )
gadgetNum=()
for (( j=$minLevel ; $j <= $maxLevel ; j += 1 )) ; do
   typesForLevels="$typesForLevels
typeForLevel$j = ${gadgetTypes[$j-$minLevel]}"
   numFiles="$numFiles
numFilesForLevel$j = ${gadgetNFiles[$j-$minLevel]}"
   gadgetNum+=(${gadgetNFiles[$j-$minLevel]})
   let gadgetNum[0]=0
   if [ $j -gt $minLevel ]; then
      let gadgetNum[$j-$minLevel]+=${gadgetNum[$j-$minLevel-1]}
   fi
done
allFiles=""

if [ $doPatch == 'true' ]; then
   eval "$(compute_patch)"
fi

i=0
doZoom=true
if [ $minLevel -eq $maxLevel -a $minMesh -ne $startMesh ]; then
   doZoom=false
   if [ $startMesh -lt $minMesh ]; then
      meshes="$startMesh $meshes"
      i=-1
   else
      meshes="$meshes $startMesh"
   fi
fi

# tests of user input
if [ $doZoom == 'false' -a $doPatch != 'false' ]; then
   echo "ERROR: please, set doPatch = false for a non-zoom simulation"
   exit
fi

if [ $maskMesh -ne $minMesh ]; then
   echo "WARNING: maskMesh = $maskMesh != minimal mesh $minMesh. Please, double check if this is what you want."
fi

# main loop

for m in ${meshes}; do
   
   echo -e "\n\n#### $m ####\n\n" >> Makefile
   
   # create names for WN, velocity, .ini and batch files
   wnPure=$wnPrefix$m
   wn=$wnPure$wnSuffix
   velPure=$velPrefix$m''_velx
   vel=$velPure$velSuffix
   velSmall=$velPrefix$m''_small_velx$velSuffix
   
   rscIni=scale_wn$m.ini
   rscBat=bat_scale_wn$m
   
   ggpIni=ggp_$m.ini
   ggpBat=bat_ggp_$m
   
   genIni=genics_$m.ini
   genBat=bat_genics_$m
   
   refIni=ref_x_$m.ini
   refBat=bat_ref_$m
   
   # decide if we do HDF5 chunking
   doChunking='false'
   if [ $m -gt $chunk ]; then
      doChunking='true'
   fi
   
   # subbox
   actualMesh=$m # in future will be calculated...
   
   mem=$(compute_mem $actualMesh)
   if [ $mem -gt $maxMem ]; then
   	maxMem=$mem
   fi
   
   # do we write only a patch?
   if [ $doPatch == 'true' ]; then
      if [ $m -ge $patchStartMesh ]; then
         doPatchThis='true'
      else
         doPatchThis='false'
      fi
      patchLoThis=''
      patchDimsThis=''
      
      if [ $m -ge $patchStartMesh ]; then
      for p in $patchLo; do
         pThis=`echo $p | awk '{print int($1*'$m'/'$maskMesh')}'`
         patchLoThis="$patchLoThis $pThis"
      done
      for p in $patchDims; do
         pThis=`echo $p | awk '{print int($1*'$m'/'$maskMesh')}'`
         patchDimsThis="$patchDimsThis $pThis"
      done
      # starting mesh will be used for the large scale part of the velocity fields
      if [ $m -eq $startMesh ]; then
         doPatchLarge=$doPatchThis
         patchLoLarge=$patchLoThis
         patchDimsLarge=$patchDimsThis
      fi
      else
      patchLoThis="0 0 0"
      patchDimsThis="0 0 0"
      patchLoLarge="0 0 0"
      patchDimsLarge="0 0 0"
      doPatchLarge=false
      fi
   else
      patchLoThis="0 0 0"
      patchDimsThis="0 0 0"
      patchLoLarge="0 0 0"
      patchDimsLarge="0 0 0"
      doPatchLarge=false
      doPatchThis=false
   fi
   
   

   # decide wether we need to do realSpaceConstraints
   
   if [ $m == $startMesh -a $wnStartFile != none ]; then
      wn=$wnStartFile
      refIni="" 
      rscIni=""
   elif [ $m -gt $startMesh ]; then
      echo "$wn : $rscIni $rscBat $wnprev" >> Makefile
      rule "$submitCommand" ./$rscBat      
      rscCreator $rscIni
      rscBatCreator $rscBat
   fi
   
   # choose how we make the velocity fields
   
   cutoffScale=`echo $startMesh | awk '{print '$Box'/$1*2}'`
   
   if [ $m -eq $startMesh ]; then
     
     if [ $wnStartFile == none -o -z "$wnStartFile" ]; then
        echo "$vel $velLarge $wn : $ggpIni $ggpBat" >> Makefile
        dumpWhiteNoise='true'
        useFileForWn='false'
     else
        echo "$vel $velLarge : $wn $ggpIni $ggpBat" >> Makefile
        dumpWhiteNoise='false'
        useFileForWn='true'
     fi
     
     rule "$submitCommand" ./$ggpBat
     
     doLargeScale='true'
     doSmallScale='false'
     
     ggpCreator $ggpIni
     ggpBatCreator $ggpBat
     
   elif [ $m -gt $startMesh ]; then
   
     echo "$velSmall : $wn $ggpIni $ggpBat" >> Makefile
     rule "$submitCommand" ./$ggpBat
     
     echo "$vel : $velSmall $velLarge $refIni $refBat" >> Makefile
     rule "$submitCommand" ./$refBat
     
     refineCreatorAdd $refIni
     refBatCreator $refBat
     
     doLargeScale='false'
     doSmallScale='true'
     dumpWhiteNoise='false'
     useFileForWn='true'
     ggpCreator $ggpIni
     ggpBatCreator $ggpBat
     
   else
     # we are downgrading velocity field
     mnext=${meshArr[$i+1]}
     velnext=$velPrefix$mnext''_velx$velSuffix
     
     echo "$vel : $velStart $refIni" >> Makefile
     rule "$submitCommand" ./$refBat
     
     refineCreatorDowngrade $refIni
     refBatCreator $refBat
     
   fi
   
   zoomLevel=$(log2 m)
   
   # get  tileLevel
   if [ $m -le 1024 ]; then
      tileOpt=8
   else
      tileOpt=16
   fi
   tileLevel=1
   for (( tileMesh=$minDim1D ; tileMesh < $tileOpt/2 ; tileMesh=$tileMesh*2 )) ; do
      let tileLevel=$tileLevel+1
   done
   
   if [ $minLevel == $maxLevel ]; then
      gadgetFile=$gadgetPrefix
   else
      gadgetFile=$gadgetPrefix.${gadgetNum[$i]}
   fi
   
   boxShift="0.0 0.0 0.0"
   #if [ $m -lt $startMesh ]; then
   #   bshift=`echo $m | awk '{print (0.5-'$startMesh'/$1/2)*'$Box'/'$startMesh'}'`
   #   boxShift="$bshift $bshift $bshift"
   #fi
   
   if [ $doZoom == true -o $m -eq $minMesh ]; then
    allFiles="$allFiles $gadgetFile"
    
    echo "$gadgetFile : $vel $genIni $genBat" >> Makefile
    rule "$submitCommand" ./$genBat
    
    genicsCreator $genIni
    genicsBatCreator $genBat
   fi
   
   wnprev=$wn
   velprev=$vel
   mprev=$m
   
   i=$((i + 1))
   
done
echo "gadget : first-rule $allFiles" >> Makefile

echo $firstRule >> Makefile

echo "
clean-vel : 
	rm -rf *_vel*$velSuffix *_delta$velSuffix

clean-wn : 
	if [ -e $wnStartFile ]; then mv $wnStartFile tmp_wn; fi
	rm -rf $wnPrefix*$wnSuffix
	if [ -e tmp_wn ]; then mv tmp_wn $wnStartFile; fi

clean-gadget : 
	rm -rf $gadgetPrefix*

clean : clean-vel clean-wn clean-gadget
" >> Makefile

# write recommendations
echo "Recommendations:
Maximal memory needed: $maxMem MB" > recommendations.txt
writeRecommendations

echo "You can now generate initial conditions by typing 'make gadget'."
echo "Before doing this, please check again the following parameters: "
echo
echo "meshes = $meshes"
echo "seeds = $seeds"
echo "zoom simulation = $doZoom"
if [ $doZoom == 'true' ]; then
   echo "  mask at resolution = $maskMesh"
fi
echo "gadget file types = ${gadgetTypes[@]}"
echo "number of output files = ${gadgetNFiles[@]}"


echo

cat recommendations.txt

