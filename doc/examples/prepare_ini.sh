#!/bin/bash

shopt -s extglob

version=01.04.2024

# this is needed to avoid , as decimal separator
export LC_NUMERIC="en_US.UTF-8"

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
boxsizeInMpch = $subbox
inputDim1D = $actualMeshPrevRefine
outputDim1D = $actualMesh
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
doPatch = false

"
iniwriter $1
}

### .ini creator for refineGrid - only for downgrading velocity fields!
refineCreatorDowngrade ()
{
template="
[Setup]
boxsizeInMpch = $subbox
inputDim1D = $mnext
outputDim1D = $m
readerSecName = inputReader
writerSecName = outputWriter
varName = velx
addFields = false
doPk = false

[inputReader]
type = $outFieldFormat
prefix = $velnext

[outputWriter]
type = $outFieldFormat
prefix = $velPure
overwriteFileIfExists = true
writerSection = writeHDF

[writeHDF]
doChunking = $doChunking
chunkSize = $chunk $chunk $chunk
$graficStuff
"
iniwriter_multi $1
}

### .ini creator for refineGrid - cutting a patch from a field!
refineCreatorCut ()
{
template="
[Setup]
boxsizeInMpch = $subbox
inputDim1D = $actualMeshPrev
outputDim1D = $actualMeshPrev
readerSecName = inputReader
writerSecName = outputWriter
varName = velx
addFields = false
doPk = false

[inputReader]
type = $outFieldFormat
prefix = $velprev

[outputWriter]
type = $outFieldFormat
prefix = "$velPrefix$mprev''_cut_velx"
overwriteFileIfExists = true
writerSection = write

[write]
doChunking = $doChunking
chunkSize = $chunk $chunk $chunk
doPatch = true
patchSection = Patch
$graficStuff

[Patch]
unit = cells
patchLo = $patchLoCut
patchDims = $patchDimsCut
"
iniwriter_multi $1
}

### .ini creator for refineGrid - cutting a patch from a WN field!
refineCreatorCutWN ()
{
template="
[Setup]
boxsizeInMpch = $Box
inputDim1D = $actualMeshPrev
outputDim1D = $actualMeshPrev
readerSecName = inputReader
writerSecName = outputWriter
varName = wn
addFields = false
doPk = false

[inputReader]
type = hdf5
prefix = $wnprev

[outputWriter]
type = hdf5
prefix = "$wnPrefix$mprev''_cut"
overwriteFileIfExists = true
writerSection = writeHDF

[writeHDF]
doChunking = $doChunking
chunkSize = $chunk $chunk $chunk
doPatch = true
patchSection = Patch

[Patch]
unit = cells
patchLo = $patchLoCut
patchDims = $patchDimsCut
"
iniwriter $1
}

### .ini creator for refineGrid - add large scale and small scale fields
refineCreatorAdd ()
{
template="
[Setup]
boxsizeInMpch = $subbox
inputDim1D = $actualMeshPrevRefine
outputDim1D = $actualMesh
readerSecName = inputReader
readerAddSecName = reader2
writerSecName = outputWriter
varName = velx
addFields = true
doPk = false

[inputReader]
type = $outFieldFormat
prefix = $velLarge
readerSection = readLarge

[reader2]
type = $outFieldFormat
prefix = $velSmall
readerSection = readSmall

[outputWriter]
type = $outFieldFormat
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
$graficStuff

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
dim1D = $actualMesh

boxsizeInMpch = $subbox
zInit = $zInit
gridName = testGrid
normalisationMode = sigma8

doLargeScale = $doLargeScale
doSmallScale = $doSmallScale
cutoffScale = $cutoffScale
writeDensityField = $doDelta

[Output]
type = $outFieldFormat
path = ./
# this is different for each level:
prefix = $2

overwriteFileIfExists = true
writerSection = OutputHDF5

[OutputHDF5]
doChunking = $doChunking
chunkSize = $chunk $chunk $chunk
doPatch = $doPatchThis
patchSection = Patch
$graficStuff

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
numStreamsTotal = 256
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
patchLo = $patchLoGen
patchDims = $patchDimsGen
dim1D = $m

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
sequentialIDs = $sequentialIDs
doMassBlock = $doMassBlock
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
numLevels = 20
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
doPatch = $doPatchGen
patchSection = Patch

[GenicsInput_velx]
type = hdf5
# this is different for each level:
prefix = $velPrefix$m
qualifier = _velx
suffix = .h5
doPatch = $doPatchGen
patchSection = Patch

[GenicsInput_vely]
type = hdf5
# this is different for each level:
prefix = $velPrefix$m
qualifier = _vely
suffix = .h5
doPatch = $doPatchGen
patchSection = Patch

[GenicsInput_velz]
type = hdf5
# this is different for each level:
prefix = $velPrefix$m
qualifier = _velz
suffix = .h5
doPatch = $doPatchGen
patchSection = Patch

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

# just log2...
function log2p {
    local x=0
    local inval=$1
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
   if [ $doDelta == 'true' ]; then
     sed 's|velx|delta|' $1 > ${1/x/d}.tmp
     update ${1/x/d}
   fi
}

compute_mem ()
{
   echo $1 | awk '{print int($1^3*4*2/1e6*2)}'
}

compute_patch ()
{
   awk 'BEGIN {x1='$maskMesh'; y1='$maskMesh'; z1='$maskMesh'; x2=0; y2=0; z2=0
        db='$maskMesh'/2
        b='$maskMesh'
        xb1='$maskMesh'; yb1='$maskMesh'; zb1='$maskMesh'; xb2=0; yb2=0; zb2=0}
   {x=$8-db; y=$9-db; z=$10-db; if(x<0) x=x+b; if(y<0) y=y+b; if(z<0) z=z+b
   if($8<x1) x1=$8; if($8>x2) x2=$8
   if($9<y1) y1=$9; if($9>y2) y2=$9
   if($10<z1) z1=$10; if($10>z2) z2=$10
   if(x<xb1) xb1=x; if(x>xb2) xb2=x
   if(y<yb1) yb1=y; if(y>yb2) yb2=y
   if(z<zb1) zb1=z; if(z>zb2) zb2=z
   }
   END {dx=x2-x1+1; dy=y2-y1+1; dz=z2-z1+1
   if(dx=='$maskMesh') {dx=xb2-xb1+1; x1=xb1+db}
   if(dy=='$maskMesh') {dy=yb2-yb1+1; y1=yb1+db}
   if(dz=='$maskMesh') {dz=zb2-zb1+1; z1=zb1+db}
   if(dx>dy) d=dx; else d=dy
   if(d<dz) d=dz
   print "maskNCells=\"",NR,"\""
   print "maskCenter1=\"",int(x1+d/2),"\""
   print "maskCenter2=\"",int(y1+d/2),"\""
   print "maskCenter3=\"",int(z1+d/2),"\""
   print "maskSize=\"",d,"\""}' $maskFile
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
grafic=false
doDelta=false
sequentialIDs=true
doMassBlock=false

# read .ini into variables

cfg_parser $1
IFS=$' '

# make arrays just in case
meshArr=( ${meshes} )
seedArr=( ${seeds} )
subArr=( ${subboxes} )
actualMeshes=''

# some standard choice
velSuffix=.h5
wnSuffix=.h5

outFieldFormat=hdf5
graficStuff=''

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

if [ $grafic == 'true' ]; then
   doDelta='true'
   velSuffix=''
fi
xoff="0.0 0.0 0.0"

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

i=0
subfactorPrev=1
mprev=$minMesh
subbox=$Box
doZoom=true
if [ $minLevel -eq $maxLevel -a $minMesh -ne $startMesh ]; then
   doZoom=false
   if [ $startMesh -lt $minMesh ]; then
      mesh=$startMesh
      meshes=$mesh
      while [ $mesh -lt $minMesh ]; do
         if [ $(( mesh*3 )) -eq $minMesh ]; then
            let mesh=$mesh*3
         else
            let mesh=$mesh*2
         fi
         meshes="$meshes $mesh"
      done
      i=-1
   else
      meshes="$meshes $startMesh"
   fi
   #seeds="$seeds $seeds"
   seedArr=( ${seeds} )
   meshArr=( ${meshes} )
fi

if [ $doZoom == 'true' ]; then
   eval "$(compute_patch)"
   let patchCenter1=maskCenter1
   let patchCenter2=maskCenter2
   let patchCenter3=maskCenter3
fi


# tests of user input

if [ $maskMesh -ne $minMesh ]; then
   echo "WARNING: maskMesh = $maskMesh != minimal mesh $minMesh. Please, double check if this is what you want."
fi

nmeshes=${#meshArr[@]}
if [ ${#seedArr[@]} -lt $nmeshes ]; then
   echo "ERROR: not enough seeds."
   exit
fi

if [ $doZoom == True -a ${#gadgetTypes[@]} -lt $nmeshes ]; then
   echo "ERROR: not enough gadgetTypes."
   exit
fi

if [ $doZoom == True -a  ${#gadgetNFiles[@]} -lt $nmeshes ]; then
   echo "ERROR: not enough gadgetNFiles."
   exit
fi

if [ -n "$subboxes" ]; then
   if [ ${#subArr[@]} -lt $nmeshes ]; then
      echo "ERROR: not enough subboxes."
      exit
   fi
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
   actualMesh=$m
   actualMeshPrevRefine=$mprev
   doPatchGen=false
   patchLoGen="0 0 0"
   patchDimsGen="0 0 0"
   doPatchWN=false
   patchLoWN="0 0 0"
   patchDimsWN="0 0 0"
   if [ -n "$subboxes" ]; then
    subbox=${subArr[$i]}
    subfactor=`echo $subbox $Box|awk '{print int($2/$1)}'`
    sub2power=$(log2p subfactor)
    subcheck=`echo $subfactor $sub2power|awk '{if($1==2^$2) print "true"; else print "false"}'`
    if [ $subcheck != 'true' ]; then
        echo "ERROR: box/subbox must be a power of 2."
	echo subfactor: $subfactor
	echo sub2power: $sub2power
        exit
    fi
    let actualMesh=m/subfactor
    let actualMeshPrevRefine=mprev/subfactor
    let actualMaskSize=maskSize*m/maskMesh
    if [ $actualMaskSize -gt $actualMesh ]; then
        echo "ERROR: mask size > subbox for mesh $m"
	echo $actualMaskSize ">" $actualMesh
        exit
    fi
    if [ $m -gt $maskMesh ]; then
        let patchCenter1=patchCenter1*m/mprev
        let patchCenter2=patchCenter2*m/mprev
        let patchCenter3=patchCenter3*m/mprev
    fi
    if [ $subfactor -gt $subfactorPrev ]; then
        if [ $m -lt $maskMesh ]; then
            echo "ERROR: cutting subbox at mesh $m < maskMesh is not allowed."
            exit
        fi
        doPatchWN='true'
        patchDimsWN="$actualMesh $actualMesh $actualMesh"
        let patchLo1=patchCenter1-actualMesh/2
        let patchLo2=patchCenter2-actualMesh/2
        let patchLo3=patchCenter3-actualMesh/2
        patchLoWN="$patchLo1 $patchLo2 $patchLo3"
        doPatchLarge='true'
        patchDimsCut="$actualMeshPrevRefine $actualMeshPrevRefine $actualMeshPrevRefine"
        let patchLo1=patchCenter1*mprev/m-actualMeshPrevRefine/2
        let patchLo2=patchCenter2*mprev/m-actualMeshPrevRefine/2
        let patchLo3=patchCenter3*mprev/m-actualMeshPrevRefine/2
        patchLoCut="$patchLo1 $patchLo2 $patchLo3"
        velCut=$velPrefix$mprev''_cut_velx$velSuffix
        refCutBat=bat_ref_cut_$mprev
        refCutIni=ref_x_cut_$mprev.ini
        echo "$velCut : $velPrefix$mprev""_velx$velSuffix $refCutIni $refCutBat" >> Makefile
        rule "$submitCommand" ./$refCutBat
        if [ $grafic == 'true' ]; then
			outFieldFormat=grafic
			dx=`echo $Box $mprev $modelHubble | awk '{print 1.*$1/$2/$3}'`
			hubble=`echo $modelHubble | awk '{print $1*100.}'`
			xoff=`echo $dx $xoff $patchLoCut | awk '{print $2+$5*$1, $3+$6*$1, $4+$7*$1}'`
			ainit=`echo $zInit | awk '{print 1./($1+1.)}'`
			graficStuff="isWhiteNoise = false
size = $patchDimsCut
xoff = $xoff
dx = $dx
astart = $ainit
omegam = $modelOmegaMatter0
omegav = $modelOmegaLambda0
h0 = $hubble
"
		fi
        refineCreatorCut $refCutIni
        refineCreatorCutWN ref_wn_cut_$mprev.ini
        wnCut=$wnPrefix$mprev''_cut$wnSuffix
        refCutWNBat=bat_ref_WN_cut_$mprev
        refCutWNBatCreator $refCutWNBat
        echo "$wnCut : $wnPrefix$mprev$wnSuffix ref_wn_cut_$mprev.ini $refCutWNBat" >> Makefile
        rule "$submitCommand" ./$refCutWNBat
        wnprev=$wnCut
        tmpm=$m
        m=cut_$mprev
        refBatCreator $refCutBat
        m=$tmpm
        velLarge=$velCut
        let patchCenter1=patchCenter1-patchLo1
        let patchCenter2=patchCenter2-patchLo2
        let patchCenter3=patchCenter3-patchLo3
    else
        doPatchWN='false'
        patchDimsWN="0 0 0"
        patchLoWN="0 0 0"
    fi
    if [ $subfactor -gt 1 ]; then
        doPatchGen='true'
        patchDimsGen="$actualMesh $actualMesh $actualMesh"
        let patchLo1=maskCenter1*m/maskMesh-actualMesh/2
        let patchLo2=maskCenter2*m/maskMesh-actualMesh/2
        let patchLo3=maskCenter3*m/maskMesh-actualMesh/2
        patchLoGen="$patchLo1 $patchLo2 $patchLo3"
    else
        doPatchGen='false'
        patchDimsGen="0 0 0"
        patchLoGen="0 0 0"
    fi
    subfactorPrev=$subfactor
   fi
   
   mem=$(compute_mem $actualMesh)
   if [ $m -lt $startMesh ]; then
        mnext=${meshArr[$i+1]}
        mem=$(compute_mem $mnext)
   fi
   if [ $mem -gt $maxMem ]; then
   	maxMem=$mem
   fi
   
   actualMeshes="$actualMeshes $actualMesh"
   

       patchLoThis="0 0 0"
       patchDimsThis="0 0 0"
       patchLoLarge="0 0 0"
       patchDimsLarge="0 0 0"
       doPatchLarge=false
       doPatchThis=false

   
   

   # decide wether we need to do realSpaceConstraints
   
   if [ $m == $startMesh -a $wnStartFile != none ]; then
      wn=$wnStartFile
      refIni="" 
      rscIni=""
   elif [ $m -gt $startMesh ]; then
      if [ $(( m/mprev )) -eq 3 ]; then
         let mint=$mprev*2
         wnint=$wnPrefix$mint$wnSuffix
         let actualMeshWN=actualMeshPrev*mint/mprev
         echo "$wnint : scale_wn$mint.ini $wnprev" >> Makefile
         rule "$submitCommand" ./bat_scale_wn$mint
         tmp1=$wnPure
         wnPure=$wnPrefix$mint
         rscCreator scale_wn$mint.ini
         wnPure=$tmp1
         tmp1=$rscIni
         rscIni=scale_wn$mint.ini
         rscBatCreator bat_scale_wn$mint
         rscIni=$tmp1
         
         let actualMeshWN=actualMeshPrev*m/mprev
         tmp1=$actualMeshPrev
         let actualMeshPrev=$actualMeshPrev*2
         tmp2=$wnprev
         wnprev=$wnint
         echo "$wn : $rscIni $wnprev" >> Makefile
         rule "$submitCommand" ./$rscBat      
         rscCreator $rscIni
         rscBatCreator $rscBat
         actualMeshPrev=$tmp1
         wnprev=$tmp2
      else
         let actualMeshWN=actualMeshPrev*m/mprev
         echo "$wn : $rscIni $wnprev" >> Makefile
         rule "$submitCommand" ./$rscBat      
         rscCreator $rscIni
         rscBatCreator $rscBat
      fi
   fi
   
   # choose how we make the velocity fields
   
   cutoffScale=`echo $startMesh | awk '{print '$Box'/$1*1.1/3.14}'`

     if [ $grafic == 'true' ]; then
        outFieldFormat=grafic
        dx=`echo $Box $m $modelHubble | awk '{print 1.*$1/$2/$3}'`
        hubble=`echo $modelHubble | awk '{print $1*100.}'`
        ainit=`echo $zInit | awk '{print 1./($1+1.)}'`
        graficStuff="isWhiteNoise = false
size = $actualMesh $actualMesh $actualMesh
xoff = $xoff
dx = $dx
astart = $ainit
omegam = $modelOmegaMatter0
omegav = $modelOmegaLambda0
h0 = $hubble
"
     fi

   
   if [ $m -eq $startMesh ]; then
     
     if [ $wnStartFile == none -o -z "$wnStartFile" ]; then
        echo "$vel $velLarge $wn : $ggpIni" >> Makefile
        dumpWhiteNoise='true'
        useFileForWn='false'
     else
        echo "$vel : $wn $ggpIni" >> Makefile
        dumpWhiteNoise='false'
        useFileForWn='true'
     fi
     
     rule "$submitCommand" ./$ggpBat
     
     doLargeScale='false'
     doSmallScale='false'
     
     ggpCreator $ggpIni $velPrefix$m
     ggpBatCreator $ggpBat
     
   elif [ $m -gt $startMesh ]; then
   
     echo "$velSmall : $wn $ggpIni" >> Makefile
     rule "$submitCommand" ./$ggpBat
     
     echo "$vel : $velSmall $velLarge $refIni" >> Makefile
     rule "$submitCommand" ./$refBat

     
     refineCreatorAdd $refIni
     refBatCreator $refBat
     
     doLargeScale='false'
     doSmallScale='false'
     dumpWhiteNoise='false'
     useFileForWn='true'
     
     ggpCreator $ggpIni $velPrefix$m''_small
     ggpBatCreator $ggpBat
     
   else
     # we are downgrading velocity field
     mnext=${meshArr[$i+1]}
     velnext=$velPrefix$mnext''_velx$velSuffix
     
     echo "$vel : $velnext $refIni" >> Makefile
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
   
   if [ -z "$boxShift" ]; then
      boxShift="0.0 0.0 0.0"
   fi
   #if [ $m -lt $startMesh ]; then
   #   bshift=`echo $m | awk '{print (0.5-'$startMesh'/$1/2)*'$Box'/'$startMesh'}'`
   #   boxShift="$bshift $bshift $bshift"
   #fi
   
   if [ $doZoom == true -o $m -eq $minMesh ]; then
     if [ $grafic == false ]; then
       allFiles="$allFiles $gadgetFile"
    
       echo "$gadgetFile : $vel $genIni" >> Makefile
       rule "$submitCommand" ./$genBat
    
       genicsCreator $genIni
       genicsBatCreator $genBat
     else
       allFiles="$allFiles $vel"
     fi
   fi
   
   wnprev=$wn
   velprev=$vel
   velLarge=$vel
   mprev=$m
   actualMeshPrev=$actualMesh
   
   i=$((i + 1))
   
done
echo "gadget : first-rule $allFiles" >> Makefile
echo "grafic : first-rule $allFiles" >> Makefile

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
echo "actual meshes = $actualMeshes"
echo "seeds = $seeds"
echo "zoom simulation = $doZoom"
if [ $doZoom == 'true' ]; then
   echo "  mask at mesh $maskMesh"
   echo "  mask max extension 1D: $maskSize cells"
   echo "  mask center: $maskCenter1 $maskCenter2 $maskCenter3"
   let nhigh=maskNCells*(maxMesh/maskMesh)*(maxMesh/maskMesh)*(maxMesh/maskMesh)
   echo "  high resolution particles: $nhigh"
fi
echo "gadget file types = ${gadgetTypes[@]}"
echo "number of output files = ${gadgetNFiles[@]}"


echo

cat recommendations.txt
