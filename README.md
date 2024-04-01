Table of contents
=================

1. [Qiuckstart](#quickstart)
1. [Ginnungagap basics](#ginnungagap-basics)
1. [Installing Ginnungagap](#installing-ginnungagap)
1. [Example](#example)
1. [Changing resolution for a non-zoom run](#changing-resolution-for-a-non-zoom-run)
1. [Highest resolution zoom](#highest-resolution-zoom)
1. [Usage of Ginnungagap tools](#usage-of-ginnungagap-tools)
     1. [Input, output and patches](#input,-output-and-patches)
     1. [Ginnungagap](#ginnungagap)
     1. [RealSpaceConstraints](#realspaceconstraints)
     1. [RefineGrid](#refinegrid)
     1. [generateICs](#generateics)
     1. [LareWrite](#larewrite)
     1. [FileTools](#filetools)
1. [Known bugs](#known-bugs)
     1. [Zeros in statistics](#zeros-in-statistics)
     1. [Zeros in GAGDET files](#zeros-in-gagdet-files)
     1. [Segmentation fault](#segmentation-fault)
1. [Running on different clusters](#running-on-different-clusters)
     1. [SuperMUC](#supermuc)
     1. [Jureca](#jureca)
     1. [MareNostrum](#marenostrum)


Quickstart
==========

Ginnungagap consists of a set of separate tools to prepare cosmological initial conditions:

* `ginnungagap` to prepare velocity fields starting from a white noise field (or just a random seed), and a set of cosmological parameters
* `generateICs` to convert velocity fields into GADGET-files

For a simple (non-zoom) simulations these two programs are sufficient, but for a multi-level zoom-in simulation other tools are needed:

* `realSpaceConstraints` to rescale the white noise fields,
* `refineGrid` to increase or decrease the resolution of the velocity fields,
* `prepare_ini.sh` script to make preparation of complex multi-scale initial conditions easy.

Ginnungagap basics
==================

Initial conditions are started from a white noise field that is a 3D array of random numbers. This array is convolved with the Transfer function to make velocity fields, which are then transformed to particle offsets using Zel'dovich approximation.

Ginnungagap can be used for the following purposes:

* Making full box initial conditions. Note that Ginnungagap is fully MPI-parallel.
* Rescaling a full box initial conditions (increasing or decreasing resolution)
* Rescaling a white noise field.
* Gas support (by simply duplicating DM particle parameters).
* Making multi-scale (zoom) initial conditions. The zoom can be done on the basis of any full box simulation by adding more scales with increased/decreased resolution. The zoom region may be placed anywhere, including the boundaries of the full box. Afterwards it can be shifted to the box center.
* Output can be written in GADGET-2 (full support) or GRAFIC (only full grid currently) formats. In GADGET-2 format, particles of different mass can be assigned to different GADGET-2 particle types to optimize memory usage.
* Updating the initial conditions already made with Ginnungagap, without re-making everything from scratch. Only the parts that change will be updated. E.g. making gas/no-gas versions requires only running the conversion tool (`generateICs`). 

For multi-scale initial conditions, a set of velocity fields with different resolutions are needed. If the zoom region is small, the high resolution velocity fields do not need to cover the whole volume, so these velocity fields differ not only by resolution, but also by their size.

The velocity fields of different resolution can be prepared in different ways:

1. by rescaling a white noise with `realSpaceConstraints` and making new velocity fields with `ginnungagap` just from it,
2. by using an additional information from the lower resolution velocity fields to improve the accuracy of the previous method with `refineGrid`,
3. by downgrading a high-resolution velocity field using nearest-grid-point interpolation with `refineGrid`.

Method 2 is recommended over method 1 when possible. Method 2 also allows to make high resolution white noise and velocity fields only in a small cubic box surrounding the zoom region.

Installing Ginnungagap
======================

The main requirements of Ginnungagap are:

* FFTW-3
* Gnu Scientific Library
* SPRNG-2
* MPI and HDF5 are highly recommended.

The simplest way to download and build them is to use the following installer script:

[scripts/g9p_installer.sh](https://github.com/ginnungagapgroup/ginnungagap/blob/master/scripts/g9p_installer.sh)

In the beginning of the script you may turn off compilation of some of the libraries.

Otherwise, you can use it as an example of what build flags are required.

It is usually *NOT* recommended to use a version of HDF5 provided by your system. After the installation you need to update your LD_LIBRARY_PATH with the path to the newely installed libraries.

Example
=======

For a three-level zoom-in simulation with scales of 256^3, 512^3 and 1024^3, the example workflow looks like this:

1. Make a 256^3 white noise (`ginnungagap`)
2. Make 256^3 full grid velocity fields (`ginnungagap`)
3. Make GADGET-2 files (`generateICs`)
4. Run the low resolution full grid simulation
5. Identify the zoom region and make the mask using e.g. `tools/zoomTools/LareWriter`
6. Rescale the white noise from step 1 to 512^3 (`realSpaceConstraints`)
7. Make 512^3 velocity fields with only the small scale information (`ginnungagap`)
8. Interpolate 256^3 velocity fields from step 2 and add to them small scale information from step 7  to produce the 'final' 512^3 velocity fields (`refineGrid`)
10. Rescale the white noise from step 6 to 1024^3 (`realSpaceConstraints`)
11. Make 1024^3 velocity fields with only the small scale information (`ginnungagap`)
12. Interpolate 1024^3 velocity fields from step 8 and add to them small scale information from step 11 (`refineGrid`)
13. Make GADGET-2 files for the zoom simulation (`generateICs`)

This workflow becomes more complicated if you need more levels of zoom. Each tool is controlled by a separate `.ini` file with parameters, such as the input/output filenames, grid sizes, etc.

In order to avoid writing the `.ini` files for every operation manually it is suggested to ue a script called `prepare_ini.sh`. It takes only one 'master' `.ini` file as an input and produces all the required 'child' ini files from it, as well as prepares tasks and a Makefile that allows you to submit all the required tasks to your computer within one single command `make gadget`.

In the `doc/examples` you can find two example ini files: `example_64.ini` for a single level simulation and `example_zoom.ini` for a multi scale one. To start a project for a zoom simulation, do the following:

1. Create a directory for non-zoom simulation, e.g. 'my_project_nozoom'.

2. Copy the following files to that directory: `prepare_ini.sh`, `batTemplate_local.sh`, `example_64.ini`, `ginnungagap` and `generateICs`

3. Create a directory for the zoom simulation, 'my_project_zoom', and copy there the same files, but `example_zoom.ini` instead of `example_64.ini`, and also `realSpaceConstraints`, `refineGrid` and `lare.dat`.

4. In the folder 'my_project_nozoom' type the following commands:
    
        ./prepare_ini.sh example_64.ini
        make gadget
    
    after a while the file named 'GADGET' will appear in that folder and this is your ICs. Some other files are also created.

5. Now cd to 'my_project_zoom'.

6. Make a soft link to the white noise field from the no-zoom simulation:
    
        ln -s ../my_project_nozoom/wn_64.h5 .
    
7. Again, run:

        ./prepare_ini.sh example_zoom.ini
        make gadget
   
    The files GADGET.0 ... GADGET.3 will be created which contain the zoom ICs.

Now let's look what the `example_zoom.ini` contains:

```
[ICs]
Box = 64.0
; box size in Mpc/h

meshes = 32 64 128 256
; resolution for each mesh

startMesh = 64
; for meshes with lower resolution than startMesh the velocity fields will be
; produced using nearest grid point interpolation from the startMesh velocity
; filed. For meshes with higher resolution the complex rescaling algorithm
; described in the ginnungagap paper will be used.

seeds = 1001 1002 1003 1004
; the seeds will be used corresponding to the meshes above when they are needed

zInit = 49
; initial redshift

[options]
doGas = false
doLongIDs = false
autoCenter = false
; place zoom region at the box center - note that it is done for the center in Lagrangian coordinates
; alternatively, you can provide a vector to shift the center of the box:
; boxShift = 11.2 15.6 -2.4
useKpc = false
; which gadget output units to use

[WN]
wnStartFile = wn_64.h5
; initial white noise field (set to 'none' to generate new):
; don't include path in the filename!
; use none if you are not using an input file.

wnStartType = hdf5
; type of the input white noise field
; can be hdf5 or grfic
; note that output is hdf5

wnPrefix = wn_
; prefix for all the rest WN files that are generated from the initial one
; don't include path in prefix!

[files]
velPrefix = g9p
; don't include path in prefix!
gadgetPrefix = GADGET
; don't include path in prefix!

[HDF5]
chunk = 128
; start do chunking for mesh > chunk and use chunk as a chunkSize

[gadget]
gadgetTypes = 4 2 2 1
; gadget particle types in the same order like meshes.
gadgetNFiles = 1 1 1 1
; number of files for each zoom level in the same order like meshes.

[mask]
; mask will be applied if you spiecify more than one mesh in the [ICs] section
maskFile = lare.dat
; file with the lagrangian region mask
maskMesh = 64
; the mesh at which the mask was constructed

[Cosmology]
modelOmegaRad0 = 0.0
modelOmegaLambda0 = 0.692885
modelOmegaMatter0 = 0.307115
modelOmegaBaryon0 = 0.048206
modelHubble = 0.6777
modelSigma8 = 0.8288
modelNs = 0.9611
# You can use built-in transfer function:
powerSpectrumKmin = 1e-6
powerSpectrumKmax = 1e3
powerSpectrumNumPoints = 501
transferFunctionType = EisensteinHu1998
# or a file with the power spectrum:
#powerSpectrumFileName = mySpectrum.txt
; here if you want to use the input file with power spectrum, 
; just uncomment that line

[submit]
batTemplate = batTemplate_local.sh
; template file with submitting commands for your system
```
The file `example_64.ini` has almost the same contents, the difference is only in `meshes`, `seeds`, `gadgetTypes`, `gadgetNFiles`.

The comments in the example explain all the options.

The script `prepare_ini.sh` creates a bunch of .ini files for all the tools of Ginnungagap, also it creates bat_\* files which contain scripts to run all these tools. A Makefile is generated which is used to run all these files in a right sequence and track dependencies: if you change some parameters, `prepare_ini.sh` will update only the \*.ini and bat_\* files which are affected by the change, and `make` will take care to run only those of them which are needed.

**Important caveats**:

* The number of seeds must be not less than the number of meshes, even if some of the meshes will not be created by the random numbers (e.g. by downgrading).
* The chunk size must not be much larger than the size of the part of the grid belonging to a single MPI task. But too small chunks will result in slow operation.

Changing resolution for a non-zoom run
======================================

If you have a white noise field for some non-zoom simulation and would like to make ICs for another non-zoom simulation with different resolution, you should specify the resolution of your existing ICs as `startMesh` and the target resolution as `meshes`. In this case the script will decide what is need to be done: downgrading or upgrading the resolution, and it will produce the relevant files for this purpose. The non-zoom ICs can be created with `make gadget` command.

Note that the upgrading/degrading of resolution should be done in either steps of 2 in 1D resolution, or in steps of 1.5. The `prepare_ini.sh` takes this into account and automatically produces necessarily steps.

Highest resolution zoom
=======================

With the example of a zoom .ini file given above you can create initial conditions up to the mesh resolution which fits into computer's memory. If higher resolution is needed, one can utilize the ability of Ginnungagap to use smaller subgrids to generate small scale velocity fields. E.g. supose you need the maximal resolution equivalent to the 16384^3 full box in a box of 100 Mpc/h, while the maimal full grid that fits into computer's memory is 4096^3. In that case one can generate full box velocity fields for steps up to 4096^3 while for higher resolution the velocity fields can be generated in the subboxes of 25 Mpc/h. To do this, the part of the `.ini` file will look like this:

```
meshes =   128 256 512 1024 2048 4096 8192 16384
subboxes = 100 100 100 100  100  100  25    25
```

The code then will automatically select the position of the subbox basing on the `maskFile` provided.
When doing so, one must be sure that the subbox is at least 1.5 times larger than the size of the zoom region at the starting redshift.

Usage of Ginnungagap tools
==========================

Input, output and patches
-------------------------

The parameters of input and output are specified in special sections of `.ini` files. The output section specifies the type of file to produce, its name and path. Also it must have a reference to a type-specific writer section. The latter may contain a reference to a `Patch` section in case only a part of the grid needs to be written. Here is the HDF5 output example.

```
[Output]
type = hdf5
path = ./
prefix = g9p_velx
overwriteFileIfExists = true
writerSection = OutputHDF5

[OutputHDF5]
suffix = .h5
doChunking = true
chunkSize = 256 256 256 
doPatch = true ; if we write only a part of the grid
patchSection = Patch

[Patch]
unit = cells
patchLo = 15 16 16
patchDims = 32 32 32
```

The same holds for the reading grids:

```
[reader2]
type = hdf5
prefix = g9p256_small_velx.h5
readerSection = readSmall

[readSmall]
doPatch = true
patchSection = PatchSmall

[PatchSmall]
unit = cells
patchLo =  64 64 64
patchDims =  128 128 128
dim1D = 256 ; Full grid size is needed to properly read the data if the patch intersects the grid boundary
```

Ginnungagap
-----------

```
[Ginnungagap]
dim1D = 2048 ; grid size in 1D

boxsizeInMpch = 64.0  ; in Mpc/h
zInit = 49 ; starting redshift
gridName = testGrid ; is not used
normalisationMode = sigma8 ; available modes: none, sigma8, sigma8Box

doLargeScale = false ; do only large scale modes above the cutoff scale
doSmallScale = false ; do only small scale modes above the cutoff scale
cutoffScale = 2 ; cutoff scale in Mpc/h

[Output]
type = hdf5 ; type of grid files: hdf5 or grafic
path = ./
prefix = g9p2048 ; prefix of velocity grids

overwriteFileIfExists = true
writerSection = OutputHDF5

[OutputHDF5]
suffix = .h5
doChunking = true ; chunking is useful for MPI parallel write
chunkSize = 256 256 256 
doPatch = false ; if we write only a part of the grid
patchSection = Patch

[Patch]
unit = cells ; either cells, or Mpch
patchLo = 0 0 0
patchDims = 0 0 0

[WhiteNoise]
useFile = true ; if false - generate new
dumpWhiteNoise = false ; if true - write to file
rngSectionName = rng ; used o generate new WN
readerSection = WhiteNoiseReader
writerSection = WhiteNoiseWriter

[WhiteNoiseReader]
type = hdf5
prefix = wn_2048.h5 ; white noise file to use

[WhiteNoiseWriter]
type = hdf5
path = ./
prefix = wn_20148
doChunking = false
chunkSize = 256 256 256

[rng]
generator = 4
numStreamsTotal = 16
randomSeed = 1002

[MPI]
nProcs = 1  0  0 ; how grid is divided between CPUs

[Cosmology]
modelOmegaRad0 = 0.0
modelOmegaLambda0 = 0.692885
modelOmegaMatter0 = 0.307115
modelOmegaBaryon0 = 0.048206
modelHubble = 0.6777
modelSigma8 = 0.8288
modelNs = 0.9611
# You can use built-in transfer function:
powerSpectrumKmin = 1e-6
powerSpectrumKmax = 1e3
powerSpectrumNumPoints = 501
transferFunctionType = EisensteinHu1998
# or a file with the power spectrum:
#powerSpectrumFileName = 

```

RealSpaceConstraints
--------------------

```
[Setup]
boxsizeInMpch = 64.0
inputDim1D = 1024
outputDim1D = 2048
useFileForInput = true
readerSecName = inputReader
writerSecName = outputWriter
seedIn = 0 ; not used
seedOut = 1004 ; random seed for the new grid

[inputReader]
type = hdf5
prefix = wn_1024.h5

[outputWriter]
type = hdf5
prefix = wn_2048
overwriteFileIfExists = true
writerSection = writeHDF

[writeHDF]
doChunking = false
chunkSize = 256 256 256
```

RefineGrid
----------

This tool can do three things:

* Degrade velocity field:

```
[Setup]
boxsizeInMpch = 500.0
inputDim1D = 3072
outputDim1D = 1024
readerSecName = inputReader
writerSecName = outputWriter
varName = velx  ; name of the variable in HDF5 file. Can be velx, vely, velz, wn
addFields = false
doPk = false

[inputReader]
type = hdf5
prefix = g9p3072_velx.h5

[outputWriter]
type = hdf5
prefix = g9p_dg1024_velx
overwriteFileIfExists = true
writerSection = writeHDF

[writeHDF]
doChunking = true
chunkSize = 128 128 128
```

* Cut a patch from a field:

```
[Setup]
boxsizeInMpch = 64.0
inputDim1D = 64
outputDim1D = 64
readerSecName = inputReader
writerSecName = outputWriter
varName = velx
addFields = false

[inputReader]
type = hdf5
prefix = g9p64_velx.h5

[outputWriter]
type = hdf5
prefix = g9p64_cut_velx
overwriteFileIfExists = true
writerSection = writeHDF

[writeHDF]
doChunking = false
chunkSize = 256 256 256
doPatch = true
patchSection = Patch

[Patch]
unit = cells
patchLo = 15 16 16
patchDims = 32 32 32
```


* Add two fields with different resolution. The low resolution one is smoothed and interpolated. The corresponding small scale field must be prepared by `ginnungagap` tool with `doSmallScale = true` and the same value of the cutoff scale.

```
[Setup]
boxsizeInMpch = 64.0
inputDim1D = 64
outputDim1D = 256
readerSecName = inputReader ; section for large scale field
readerAddSecName = reader2 ; section for small scale field
writerSecName = outputWriter
varName = velx
addFields = true ; we add two fields

[inputReader]
type = hdf5
prefix = g9p64_large_velx.h5
readerSection = readLarge

[reader2]
type = hdf5
prefix = g9p256_small_velx.h5
readerSection = readSmall

[outputWriter]
type = hdf5
prefix = g9p256_velx
overwriteFileIfExists = true
writerSection = write

[readLarge]
doPatch = false
patchSection = PatchLarge

[readSmall]
doPatch = true
patchSection = PatchSmall

[write]
doChunking = false
chunkSize = 256 256 256
doPatch = true
patchSection = PatchSmall

[PatchLarge]
unit = cells
patchLo =  16 16 16
patchDims =  32 32 32

[PatchSmall]
unit = cells
patchLo =  64 64 64
patchDims =  128 128 128
dim1D = 256
```


generateICs
-----------


```
[Ginnungagap]
# this is different for each level:
dim1D = 256

boxsizeInMpch = 64.0
zInit = 49
gridName = testGrid
normalisationMode = sigma8

[Patch]
unit = cells
patchLo = 60 64 64
patchDims = 128 128 128
dim1D = 256

[Cosmology]
modelOmegaRad0 = 0.0
modelOmegaLambda0 = 0.692885
modelOmegaMatter0 = 0.307115
modelOmegaBaryon0 = 0.048206
modelHubble = 0.6777
modelSigma8 = 0.8288
modelNs = 0.9611
# You can use built-in transfer function:
powerSpectrumKmin = 1e-6
powerSpectrumKmax = 1e3
powerSpectrumNumPoints = 501
transferFunctionType = EisensteinHu1998
# or a file with the power spectrum:
#powerSpectrumFileName = 

[GenerateICs]
ginnungagapSection = Ginnungagap
doGas = false
doLongIDs = false ; if we do 8-byte IDs this should be true
sequentialIDs = true ; true means IDs are computed for particles one by one, 
                     ; false means from their Lagrangian positions.
                     ; The latter requires LongIDs for grids >1024^3.
autoCenter = false ; auto center the ICs in Lagrangian coordinates
useKpc = false
shift = 0.0 0.0 0.0  ; shift box center by a given vector
inputSection = GenicsInput
outputSection = GenicsOutput
cosmologySection = Cosmology
maskSection = Mask
hierarchySection = Hierarchy

zoomLevel = 8 ; current file is responsible for only this zoom level

typeForLevel5 = 4 ; GADGET particle types to use for each zoom level
typeForLevel6 = 2
typeForLevel7 = 2
typeForLevel8 = 1

[Mask]
maskLevel = 6 ; level at which the mask is provided
minLevel = 5 ; minimum zoom level
maxLevel = 8 ; maximal zoom level
             ; the ICs are non-zoom if maxLevel == minLevel (mask is not read in that case)
             
tileLevel = 3 ; level at which the grid is divided into tiles, 
              ; only one tile is located in memory at a time
readerType = legacy ; mask reader type
readerSection = Lare

[Hierarchy]
; defines how the levels are numbered
; e.g.
; min. level dim = 256 = 2^8
; max. level dim = 8192 = 2^13
; hence the max. level number is 13, if they are numbered from 2^0 = 1.
numLevels = 13
minDim1D = 1 ; dimension of level 0
factor = 2 ; factor between levels, should be 2

[Lare]
hasHeader = false
fileName = lare.dat ; zoom mask file
ngrid = 64 64 64 ; mask grid size

[GenicsInput]
velxSection = GenicsInput_velx
velySection = GenicsInput_vely
velzSection = GenicsInput_velz

[GenicsInput_velx]
type = hdf5
# this is different for each level:
prefix = g9p256
doPatch = false
patchSection = Patch

qualifier = _velx
suffix = .h5

[GenicsInput_vely]
type = hdf5
# this is different for each level:
prefix = g9p256
qualifier = _vely
suffix = .h5
doPatch = false
patchSection = Patch

[GenicsInput_velz]
type = hdf5
# this is different for each level:
prefix = g9p256
qualifier = _velz
suffix = .h5
doPatch = false
patchSection = Patch

[GenicsOutput]
; number of files for each zoom level
; use > 1 if file size is > 4Gb.
numFilesForLevel5 = 1
numFilesForLevel6 = 1
numFilesForLevel7 = 1
numFilesForLevel8 = 1

prefix = GADGET
```

LareWrite
---------

The tools reads file `LareWrite.tbl` for the input.

```
sphere.txt ; input of zoom region coordinates in AHF _halo file format
snapshot_100 ; snapshot to use for mask construction, particle IDs must correspond to Lagrangian positions
lare.dat ; output file name
1 ; enlarge mask by 1 cell to avoid holes in it
1.0 ; enlarge zoom region spheres by this factor
```
(comments should be removed!!!)

Example `sphere.txt`:

```
# first line is skipped
1 -1 0 0 0 1e3 32e3 63e3 0 0 0 10e3
```
This is for a sphere of radius 10 Mpc/h at X,Y,Z = 1., 32., 63 Mpc/h.
Several lines may be provided here.

Note that the units of `sphere.txt` must be the same as you use for the initial conditions, i.e. Mpc/h for Mpc/h ICs. And AHF always gives output in kpc/h!!!

FileTools
---------

`/tools/fileTools/gadget_dumpSelection` - dump part of a GADGET file

`/tools/fileTools/gadget_peekHeader` - dump header of a GADGET file

Known bugs
==========

You should always monitor the output of `make gadget` or tasks submitted by it to look for errors.

Zeros in statistics
-------------------

The programs `ginnungagap`, `realSpaceConstraints` and `refineGrid` calculate statistics on grids they are working with. If you see somewhere output like this:

```
 Calculating statistics on second input grid... took 0.13692s
                  mean  :     0.0000000000
    standard deviation  :     0.0000000000
```
it means that something is wrong. If this happens during `refineGrid`, most probably it is due to too large `chunkSize` - when `doPatch = true` the patch size could get smaller than the chunkSize if the latter is too big.

Zeros in GAGDET files
---------------------

It is recommended to dump first few particles from every GADGET file produced using `gadget_dumpSelection` tool. In some cases the whole file has particle velocities set to zero. Most probably this comes from setting a wrong `Patch`, i.e. reading a part of the mesh which was not written to the velocity field file.

Segmentation fault
------------------

Usual causes are 1) insufficient memory, 2) number of MPI tasks for `refineGrid` is not a power of 2 for a 2^N grid.

Running on different clusters
=============================

The scripts are provided for a number of clusters. Each script contains some routines to compute the number of nodes and cores needed to execute each task based on the memory requirements. 

After the execution of `prepare_ini.sh` some useful information is written on the screen, like this:

```
Recommendations:
Maximal memory needed: 68719 MB
Maximal number of nodes: 1
```

from which you know how many nodes do you need. This information is also saved to `recommendations.txt`. Sometimes you will need to select the queue and allocate resources according to this recommendations yourself.

SuperMUC
--------

Use the template file `batTemplate_supermuc.sh`. In the beginning of the script set `variant = hw` for haswell nodes or `variant = thin` for thin nodes. After running `prepare_ini.sh`, run `make gadget` in your shell and a number of files with the names ending by `.seq` will be generated. They are copies of `bat*` files with one additional command added to each of them which will submit the next script. In order to submit the whole sequence, type
        
        llsubmit 1.seq
        
If after making some changes you will need to remake the ICs, type again `make gadget` and the contents of `*.seq` files will be updated automatically. During this procedure all previously created files ending with `.seq` will be deleted.

Jureca
------

Use the template file `batTemplate_jureca.sh`. 

Each `bat_\*` file contains `srun` command. In order to submit all the scripts there are two options:

1. Do it interactively by invoking:

        salloc --partition=devel --nodes=1 --time=01:00:00
        export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/libs
        make gadget | tee LOG
        exit

2. Or in a script like this:

        #!/bin/bash -x
        #SBATCH --nodes=1
        #SBATCH --ntasks-per-node=24
        #SBATCH --time=01:00:00
        #SBATCH --partition=devel
        export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/libs
        make gadget


MareNostrum
-----------



End note
========

Copyright (C) 2010, 2011, 2012, Steffen Knollmann, 
                2013, 2014, 2015, 2016, 2017 Sergey Pilipenko

  Copying and distribution of this file, with or without modification,
  are permitted in any medium without royalty provided the copyright
  notice and this notice are preserved.
