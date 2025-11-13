Table of contents
=================

1. [Introduction](#introduction)
1. [Installing Ginnungagap](#installing-ginnungagap)
1. [Basic usage](#basic-usage)
1. [Advanced usage](#advanced-usage)

Introduciton
============

Ginnungagap is a code to prepare cosmological initial conditions (ICs). Its main features are the following:

* MPI+OpenMP efficient parallelization.
* Homogeneous resolution and variable resolution (zoom-in) ICs.
* Resolution extension: increasing/reducing the resolution of existing simulations.
* Reuse of previous computations: all intermediate results are saved as files and can be used multiple times.
* Output in GADGET-2 and GRAFIC formats (currently zoom-in ICs are only in GADGET-2 format).
* Modular structure: code is a collection of separate tools.

The more detailed explanation of concepts behind Ginnungagap and some test results can be found in the paper [g9p paper](https://axive.org/).

Installing Ginnungagap
======================

The main requirements of Ginnungagap are:

* FFTW-3
* Gnu Scientific Library
* SPRNG-2
* MPI and HDF5 are highly recommended.
* Fortran is currently required, but will be removed in future.

The simplest way to download and build them is to use the following installer script:

[scripts/g9p_installer.sh](https://github.com/ginnungagapgroup/ginnungagap/blob/master/scripts/g9p_installer.sh)

Use the options at the beginning of the script to turn off compilation of some of the libraries:
```
gsl=true
fftw=true
hdf5=true
sprng=true
g9p=true
```

Also one can use it as an example of what build flags are required.

It is usually *NOT* recommended to use a version of HDF5 provided by your system, since Ginnungagap uses a custom set of compile flags. 

After installation, all the Ginnungagap binaries can be found in `ginnungagap/bin/`. All the other libraries compiled by the installer script are at `libs/`. You need to update your LD_LIBRARY_PATH with the path to the newely installed libraries, e.g., in bash shell:
```
export LD_LIBRARY_PATH=/full/path/to/installation/libs
```

Basic usage
===========

A collection of basic examples can be found in `ginnungagap/doc/examples/`. In order to prepare a simple homogeneous ICs, please, create a working directory, and copy to it `example_64.ini`, `prepare_ini.sh` and `batTemplate_local.sh`. Also copy all the contents of `ginnungagap/bin`.

Ginnungagap consists of several tools, and running the code can be done with the following commands (assuming you are in the working directory):
```
./prepare_ini.sh example_64.ini
make gadget
```
After that you will find the file called `GADGET` which contains the ICs ready to use. Also many other files are created.

The first line invokes the helper script which creates a Makefile containing actual commands. In the case of a non-zoomed ICs, there are two commands: a call to `ginnungagap` and a call to `generateICs`.

`ginnungagap` tool first creates a White Noise (WN), which is a 3D grid of random numbers with Gaussian distirbution. This grid is saved to a file `wn_64.h5` and can be used for e.g. making a higher resolution simulation with the same large scale structure. Then, the WN is convolved with the transfer function (which depends on the cosmological power spectrum) to compute the velocity field (VF): three grids, one for each velocity component. These grids are also saved as `g9p_vel*.h5`. Details on their computation can be found in the code paper.

`generateICs` converts the VFs into a particle representation in GADGET-2 format (actually, `SnapFormat 1` in GADGET's terminology).

This example file, `example_64.ini` can be easily modified for the user's needs, all its options are either self-documenting, or have comments.

In order to make a simple resimulation with higher resolution, 128^3 particles, see the `example_upgrade.ini`. This example assumes you have already run the previous example, `example_64.ini` and have not deleted the `wn_64.h5` file. The Makefile now contains the following commands:

1. `ginnungagap ggp_64.ini` to make 64^3 VFs from the existing WN.
1. `realSpaceConstraints scale_wn128.ini` to scale up the WN to a 128^3 grid.
1. `ginnungagap ggp_128.ini` to make small scale VFs from the WN on 128^3 grid.
1. `refineGrid ref_{x,y,z}_128.ini` to take the large scale Fourier modes from 64^3 VFs, small scale Fourier modes from the previous step, and make the final 128^3 VFs.
1. `generateICs genics_128.ini` to save the particle data in GADGET_UP file in GADGET format.

The third example, `example_zoom.ini` is aimed at making the zoom-in ICs with the resolutions 32^3, 64^3, 128^3 and 256^3. Also the file with the mask, defining the zoom region, `lare.dat`, is needed. The ini file, besides quite obvious list of zoom resolutions and random seeds for them also contains the information on how to distribute zoom levels across GADGET particle types and how many files per level is to be made.

GADGET-2 files contain 6 particle types, type 0 is gas, type 1 is DM. Other types also behave as DM in non-hydro simulations. In zoom simulations particles of different zoom levels have different masses, and there are several options how they can be represented in the ICs: 

* A mass block can be provided in the ICs with individual particle masses using `doMassBlock = true` in the ini file. In that case all the particles can be set to type 1.
* Assign different types to different levels for DM only runs. Highest resolution is usually type 1. No individual masses are needed, as masses for each type can be given in the ICs file header. This option is limited by the number of available types.
* A combined method: assign the highest resolution particles to type 1 and assign all or some of the other levels to a single another type (e.g., type 2). Only when there are several levels per type the mass block will be automatically created for this particular type.

In the example, find the line
```
gadgetTypes = 4 2 2 1
```
The types are given in the same order as meshes in the beginning of the ini file. This means that lowest resolution level (32^3) will have type 4, and particle mass for this level will be given in the header (no mass block). Levels 64^3 and 128^3 will share type 2, so for them there will be a mass block created. Higest resolution particles (256^3) will go to type 1, as usual (and with no mass block).

GADGET-2 supports multiple file ICs and in Ginningagap every level is written to a separate file (even if several levels share the same particle type). A single GADGET-2 file is limited by approximately 300*10^6 particles (if single precision is used), since data blocks must be compatible with Fortran records. Thus, user can divide each level into many files by providing corresponding numbers in the `gadgetNFiles` parameter in the ini file.

The zoomed ICs also require a mask which defines which parts of the simulation volume are filled with highest resolution level particles. This mask is obtained by running a low resolution simulation e.g. 64^3 in this example, and marking all cells at the 64^3 grid which belong to the zoom region, but in Lagrangian coordinates, i.e. in the initial conditions. The Lagrangian region covering the high resolution region is provided in the file `ginnungagap/doc/examples/lare.dat`. This file contains 10 columns: particle IDs, 3 columns of coordinates, 3 columns of velocities and 3 columns of 1D cell indices, in which these particles reside. Only the last three columns are used by Ginnungagap. Other columns are needed for compatibility with some other tools. Every row of `lare.dat` contains coordinates of a single cell in the base level resolution, which is provided by the `maskMesh` parameter.

You also may notice that all the commands are placed not directly in the Makefile, but in a bunch of `bat_*` files. This is not needed for the code execution on the local machine, but allows to prepare submit scripts for supercomputers using some workload managers like Slurm.

Advanced usage
==============


End note
========

Copyright (C) 2010, 2011, 2012, Steffen Knollmann, 
                2013-2025 Sergey Pilipenko

  Copying and distribution of this file, with or without modification,
  are permitted in any medium without royalty provided the copyright
  notice and this notice are preserved.
