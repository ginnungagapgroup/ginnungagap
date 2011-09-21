// Copyright (C) 2011 Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

/**
 * @file doc/doc.h
 * @brief  Contains the package documentation in doxygen format.
 *
 *
 * This files contains no useful code.  It only contains part of the
 * package documentation in doxygen format.  The file only has the .h
 * extension to ensure that doxygen uses the correct language parser for
 * this file and to enable automatic syntax highlighting.
 */

/**
 * @mainpage Reference documentation for ginnungagap
 *
 * @author  Steffen Knollmann\n Grupo de Astrofísica\n Departamento de
 * Física Teórica C-15\n Universidad Autónoma de Madrid\n Cantoblanco
 * 28049, Madrid\n steffen.knollmann@uam.es
 *
 * @n
 *
 * @section intro Introduction
 *
 * Ginnungagap is a code for generating cosmological initial conditions.
 * It is aimed at high scalability and ease of use.
 *
 * @section requirements  Requirements
 *
 * To successfully compile ginnungagap, the following prerequisites must
 * be met:
 *
 * - <b>C compiler</b> - Since ginnungagap uses features specific to
 *   C99, a compiler supporting this standard is required.
 *   Additionally, your compiler should support <b>OpenMP</b> to make use
 *   of the shared memory parallelization of ginnungagap.
 * - <b>GSL</b> - The GNU Scientific Library must be available.  GSL is
 *   used for numerical integration and spline interpolations.
 * - <b>FFT</b> - Generating initial conditions requires computing
 *   various Fast Fourier Transforms.  Basically, ginnungagap is written
 *   to support a variety of different FFT implementations, practically
 *   only FFTW3 is supported at the moment.
 *   - <b>FFTW3</b>: Please note that you will need single and double
 *     precision versions of FFTW3 and you might want to enable the
 *     OpenMP parallelization of FFTW3 if you plan to also use the
 *     OpenMP features of ginnungagap.
 *   See @ref pageDeps_FFTW3 for instruction on how to build FFTW on
 *   your system.
 * - <b>Build tools</b> - A make command should be available.
 *   Ginnugagap has been tested with GNU make and other makes might not
 *   support all features used in the Makefiles.  In case make is not
 *   GNU make, your system has most likely gmake available.
 * - <b>Shell</b> - A couple of Shell scripts need to be executed during
 *   the build process, most prominently the configure script.  They are
 *   supposed to work on POSIX shells and are tested on bash and dash.
 *
 * Additionally, the following libraries should be available to make
 * full use of ginnungagap:
 *
 * - <b>SPRNG</b> - This library is used to generate random numbers of
 *   high quality in parallel. You will most likely want to use this
 *   library, the only way to not use it, is to use a pre-generated
 *   white noise field.  The required version is SPRNG2.0b and please
 *   note that you only need the serial version, i.e. do not build SPRNG
 *   with MPI support.  See @ref pageDeps_SPRNG for build instructions.
 * - <b>MPI</b> - To get distributed memory parallelization you need the
 *   MPI libraries.  Please use MPI-2 and above.
 *
 * Furthermore the following libraries can be used to extend the
 * features of ginnungagap:
 *
 * - <b>Silo</b> - One of the output formats of ginnungagap is Silo.  It
 *   provides an easy and quick way to write large data-sets that can
 *   directly be visualized (using Visit or Paraview).  This is mostly
 *   of use for debugging and testing purposes.
 *   See @ref pageDeps_Silo for instructions on how to build Silo.
 * - <b>HDF5</b> - This is not a library used directly by ginnungagap,
 *   but Silo can be compiled to use HDF5 as a file backend.  This is
 *   highly advised.
 *   See @ref pageDeps_HDF5 for instructions on how to build HDF5.
 * - <b>MPItrace</b> - Tracing utilities can be used to generate trace
 *   files of ginnungagap runs.
 */

/**
 * @page pageInputfile Writing Input Files.
 *
 * This page will explain briefly how the input files for Ginnungagap
 * should be written.  Input files may become complex and not all
 * nuances are covered here.  For the details, follow the provided links
 * to the in-depth discussions of the individual sections.
 *
 * @section pageInputfile_Basic Basics
 *
 * The format for the input files is the standard INI format.  That
 * means, the input files show the following general structure:
 *
 * @code
 * [SectionName1]
 * keyName1 = keyValue1
 * keyName2 = keyValue2
 * keyName3 = keyValue3
 *
 * [SectionName2]
 * keyName1 = keyValue1
 * keyName2 = keyValue2
 * keyName3 = keyValue3
 * @endcode
 *
 *
 * @section pageInputfile_Sections Main Sections
 *
 * The actual ini file for Ginnungagap consists of the following main
 * sections:
 *
 * <dl>
 *   <dt>[Ginnungagap]</dt>
 *   <dd>Contains basic informations for the code.  The structure of
 *       this section is described in @ref ginnungagapSetupIniFormat and
 *       handled by g9pSetup_new().</dd>
 *   <dt>[Cosmology]</dt>
 *   <dd>Construction information for the cosmological model and the
 *       power spectrum.  This is used from ginnungagap_new() and calls
 *       cosmoModel_newFromIni() and cosmoPk_newFromIni().  See @ref
 *       libcosmoModelIniFormat.  This section is handled from
 *       ginnungagap_new().</dd>
 *   <dt>[WhiteNoise]</dt>
 *   <dd>Contains information about the white noise generation.  This is
 *       described in more detail in @ref ginnungagapWNIniFormat and the
 *       section is handled by g9pWN_newFromIni().</dd>
 *   <dt>[Output]</dt>
 *   <dd>This section contains the information for the output of the
 *       four main fields, the density field and the three velocity
 *       components (note that the writing of the density field can be
 *       disabled in <tt>[Ginnungagap]</tt>.  The content of this
 *       section, as it is essentially only a writer, is performed by
 *       gridWriter_newFromIni() and the details are described in
 *       @ref libgridIOOutIniFormat.  This section is handled from
 *       ginnungagap_new().</dd>
 *   <dt>[MPI]</dt>
 *   <dd>This section contains information about the process grid used
 *       for the domain decomposition.  This is only read if the code is
 *       compiled with MPI enabled (<tt>./configure</tt> option
 *       <tt>--with-mpi</tt>).  In that case, it is handled by
 *       g9pSetup_new().  See @ref ginnungagapSetupIniFormatMPI for the
 *       details.</dd>
 * </dl>
 *
 * Additionally, a variety of other section may appear, as some
 * components require additional information that are best kept in their
 * own sections.  This is mostly true for the IO related components.
 * This best understood by looking at annotated example input files.
 *
 * @section pageInputfile_Examples Examples
 *
 * @subsection pageInputfile_Examples_Simple  Very simple example
 *
 * @code
 *
 * [Ginnungagap]
 * # Using a 200^3 grid
 * dim1D = 200
 * # in a 99 Mpc/h box
 * boxsizeInMpch = 99
 * # starting at redshift 40.
 * zInit = 40.0
 * # using a bogus name for the grid
 * gridName = testGrid
 * # and the power spectrum should be normalised to have the proper sigma8.
 * normalisationMode = sigma8
 *
 * [Cosmology]
 * # Our model of choice
 * modelOmegaRad0 = 8.348451673443855e-05
 * modelOmegaLambda0 = 0.734
 * modelOmegaMatter0 = 0.2669
 * modelOmegaBaryon0 = 0.0449
 * modelHubble = 0.71
 * modelSigma8 = 0.801
 * modelNs = 0.963
 * modelTempCMB = 2.725
 * # And the power spectrum is calculated by a transfer function
 * powerSpectrumKmin = 1e-5
 * powerSpectrumKmax = 1e4
 * powerSpectrumNumPoints = 501
 * transferFunctionType = EisensteinHu1998
 *
 * [Output]
 * # The output is written as grafic format
 * writerType = grafic
 * # and the details are in that section.
 * writerSection = OutputGrafic
 *
 * [OutputGrafic]
 * # The output files should start with ic_ (the writer will
 * # successively add delta, velx, vely, and velz).
 * prefix = ic_
 * # We do not write white noise files with this writer.
 * isWhiteNoise = false
 * # The size of the grid is 200^3 (MUST be consistent with dim1D)
 * size = 200, 200, 200
 * # This is boxsizeInMpch / dim1D / modelHubble
 * dx = 0.69718309859154937
 * # This is just 1. / (zInit + 1.)
 * astart = 0.024390243902439025
 * # The matter content of the universe (must be consistent with
 * # modelOmegaMatter0).
 * omegam = 0.2669
 * # The lambda content of the universe (must be consistent with
 * # modelOmegaLambda0).
 * omegav = 0.734
 * # The Hubble parameter, here in units of km/s/Mpc, must be consistent
 * # with modelHubble.
 * h0 = 71
 *
 * [WhiteNoise]
 * # We want to use the RNG
 * useFile = false
 * rngSectionName = rng
 * # And no writing of the WN please.
 * dumpWhiteNoise = false
 *
 * [MPI]
 * # MPI will decide how to arrange the available processors over the y
 * # and z dimension.
 * nProcs = 1 0 0
 *
 * [rng]
 * # SPRNG Voodoo
 * generator = 4
 * # We use a total of 256 streams (so using more than 256 MPI processes
 * # will not work, and, in fact, 256 must be exactly divisible by the
 * # number of MPI tasks,
 * numStreamsTotal = 256
 * # The seed for the random numbers.
 * randomSeed = 1
 *
 * @endcode
 */

/**
 * @page pageDeps External Dependencies.
 *
 * Silo can be found here: https://wci.llnl.gov/codes/silo/ 
 *
 * @section pageDeps_SPRNG  Building SPRNG
 *
 * You just use this tarball:
 *
 * @code
 * $ md5sum sprng2.0b.tar.gz
 * cf825f9333d07acdcaa599f29f281b8d sprng2.0b.tar.gz
 * @endcode
 *
 * And also look in the you ginnungagap sources, there will be these
 * files:
 *
 * @code
 * $ ls doc/devel/patches/
 * sprng2.0.fixMemoryLeak.patch  sprng2.0.fixMemoryLeak.patch.txt
 * @endcode
 *
 * This is a small patch that resolves a memory hole in SPRNG.  It
 * shouldn't be too critical if you don't patch the code, but a hole is
 * a hole...  Anyhow, you are now set to build SPRNG:
 *
 * @code
 * $ tar xf sprng2.0b.tar.gz
 * $ cd sprng2.0/
 * $ patch -p1 -i ../sprng2.0.fixMemoryLeak.patch
 * patching file make.CHOICES
 * patching file SRC/make.INTEL
 * patching file SRC/mlfg/mlfg.c
 * $ pwd
 * /home/me/software/sprng2.0
 * $ make src
 * [.. lots of output .. should end with:]
 * make[1]: Leaving directory `/home/me/software/sprng2.0/SRC'
 * $ ls lib/
 * libsprng.a  Makefile  Makefile~
 * $ chmod 644 include/*h
 * $ ls include/
 * d~  interface.h  sprng_f.h  sprng.h
 * @endcode
 *
 * You can now compile Ginnungagap with using
 * @code
 * --with-sprng-prefix=/home/me/software/sprng2.0
 * @endcode
 *
 * @section pageDeps_FFTW3  Building FFTW3
 *
 * This is single precision version compiled with the standard compiler
 * (must likely gcc):
 *
 * @code
 * ./configure --prefix=/opt/fftw/fftw-3.2.2-gcc \
 *             --enable-sse \
 *             --enable-float \
 *             --disable-fortran \
 *             --enable-openmp
 * make -j && make install
 * @endcode
 *
 * This is double precision version compiled with the standard compiler
 * (must likely gcc):
 *
 * @code
 * ./configure --prefix=/opt/fftw/fftw-3.2.2-gcc \
 *             --enable-sse2 \
 *             --disable-fortran \
 *             --enable-openmp
 * make -j && make install
 * @endcode
 *
 * This is single precision version compiled with icc:
 *
 * @code
 * CC=icc ./configure --prefix=/opt/fftw/fftw-3.2.2-intel \
 *                    --enable-sse \
 *                    --enable-float \
 *                    --disable-fortran \
 *                    --enable-openmp
 * make -j && make install
 * @endcode
 *
 * This is double precision version compiled with icc:
 *
 * @code
 * CC=icc ./configure --prefix=/opt/fftw/fftw-3.2.2-intel \
 *                 --enable-sse2 \
 *                    --disable-fortran \
 *                    --enable-openmp
 * make -j && make install
 * @endcode
 *
 * Depending on whether you are using icc or gcc, you can select the
 * appropriate FFTW version when configuring ginnungagap:
 *
 * @code
 * --with-fft-prefix=/opt/fftw/fftw-3.2.2-intel
 * @endcode
 *
 * or
 *
 * @code
 * --with-fft-prefix=/opt/fftw/fftw-3.2.2-gcc
 * @endcode
 *
 * Of course, you can use whatever --prefix you like when configuring
 * FFTW.
 *
 *
 * @section pageDeps_HDF5  Building HDF5
 *
 * @code
 * ./configure --prefix=/opt/hdf5/hdf5-1.8.4p1-gcc
 * make -j && make install
 *
 * CC=icc LD=xild AR=xiar ./configure --prefix=/opt/hdf5/hdf5-1.8.4p1-intel
 * make -j && make install
 * @endcode
 *
 *
 * @section pageDeps_Silo  Building Silo
 *
 * @code
 * export HDF5=/opt/hdf5/hdf5-1.8.4p1-gcc
 * ./configure --prefix=/opt/silo/silo-4.7.2-gcc \
 *             --with-hdf5=$HDF5/include,$HDF5/lib \
 *             --enable-portable-binary \
 *             --disable-fortran \
 *             --without-qt
 * make -j && make install
 *
 * export HDF5=/opt/hdf5/hdf5-1.8.4p1-intel
 * CC=icc CXX=icpc LD=xild AR=xiar CFLAGS=-fast ./configure \
 *      --prefix=/opt/silo/silo-4.7.2-intel \
 *      --with-hdf5=$HDF5/include,$HDF5/lib \
 *      --enable-portable-binary \
 *      --disable-fortran \
 *      --without-qt
 * make -j && make install
 * @endcode
 */
