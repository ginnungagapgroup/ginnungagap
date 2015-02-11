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


/*--- MAIN PAGE ---------------------------------------------------------*/

/**
 * @mainpage Reference documentation for ginnungagap
 *
 * @section intro Introduction
 *
 * Ginnungagap is a code for generating cosmological initial conditions.
 * It is aimed at high scalability and ease of use.  The main code will
 * generate the velocity field at the requested starting redshift, which
 * is then used by extra tools to generated a particle representation.
 *
 * Furthermore, tools are provided to generate realisations at different
 * resolution levels while keeping the large scale structure fixed (by
 * means of refining the underlying white noise field).
 *
 * See @ref pageQuickstart for a quick run-through, explaining how to
 * generate a box at two different resolution levels.
 *
 * @section requirements  Requirements
 *
 * To successfully compile ginnungagap, the following prerequisites must
 * be met:
 * <dl>
 *  <dt>C compiler</dt>
 *  <dd>Since ginnungagap uses features specific to C99, a compiler
 *      supporting this standard is required.  Additionally, your
 *      compiler should support <b>OpenMP</b> to make use of the shared
 *      memory parallelization of ginnungagap.</dd>
 *  <dt>GSL</dt>
 *  <dd>The GNU Scientific Library must be available.  GSL is used for
 *      numerical integration and spline interpolations.</dd>
 *  <dt>FFT</dt>
 *  <dd>Generating initial conditions requires computing various Fast
 *      Fourier Transforms.  Basically, ginnungagap is written to
 *      support a variety of different FFT implementations, practically
 *      only FFTW3 is supported at the moment.
 *      - <b>FFTW3</b>: Please note that you will need single and double
 *        precision versions of FFTW3 and you might want to enable the
 *        OpenMP parallelization of FFTW3 if you plan to also use the
 *        OpenMP features of ginnungagap.
 *        See @ref pageDeps_FFTW3 for instruction on how to build FFTW on
 *        your system.
 *  </dd>
 *  <dt>Build tools</dt>
 *  <dd>A make command should be available.  Ginnugagap has been tested
 *      with GNU make and other makes might not support all features
 *      used in the Makefiles.  In case make is not GNU make, your
 *      system has most likely gmake available.</dd>
 *  <dt>Shell</dt>
 *  <dd>A couple of Shell scripts need to be executed during the build
 *      process, most prominently the @link pageConfigureScript
 *      configure script @endlink.  They are supposed to work on POSIX
 *      shells and are tested on bash and dash.</dd>
 * </dl>
 *
 * Additionally, the following libraries should be available to make
 * full use of ginnungagap:
 * <dl>
 *  <dt>SPRNG</dt>
 *   <dd>This library is used to generate random numbers of high quality
 *       in parallel. You will most likely want to use this library, the
 *       only way to not use it, is to use a pre-generated white noise
 *       field.  The required version is 2.0b and please note that
 *       you only need the serial version, i.e. do not build SPRNG with
 *       MPI support.  See @ref pageDeps_SPRNG for build
 *       instructions.</dd>
 *  <dt>MPI</dt>
 *  <dd>To get distributed memory parallelization you need the MPI
 *      libraries.  Please use MPI-2 or above.</dd>
 *  <dt>HDF5</dt>
 *  <dd>This library can be used to provide a scalable and fast IO for
 *      large initial condition.  This is the advised format for
 *      writing initial condition data beyond a resolution of 1024^3, in
 *      which case the parallel version of HDF5 (using MPI-IO under the
 *      hood) is required.
 *      See @ref pageDeps_HDF5 for instructions on how to build
 *      HDF5.</dd>
 * </dl>
 *
 * Furthermore the following libraries can be used to extend the
 * features of ginnungagap:
 * <dl>
 *  <dt>Silo</dt>
 *  <dd>One of the output formats of ginnungagap is Silo.  It
 *      provides an easy and quick way to write large data-sets that
 *      can directly be visualized (using Visit or Paraview).  This is
 *      mostly of use for debugging and testing purposes.  Note that
 *      only output is supported in Silo format.
 *      See @ref pageDeps_Silo for instructions on how to build
 *      Silo.</dd>
 *  <dt>MPItrace</dt>
 *  <dd>Tracing utilities can be used to generate trace files of
 *      ginnungagap runs.</dd>
 * </dl>
 */


/*--- Page: Configuring -------------------------------------------------*/

/**
 * @page pageConfigureScript Configuring the Build of Ginnungagap
 *
 * @section pageConfigureScript_General Basics
 *
 * To compile the code, the two files @c config.h and @c Makefile.config
 * must be generated.  While it is possible to write them by hand, the
 * easiest way is to use the @c configure script.  That script accepts a
 * variety of switches to enable/disable functionality and provide the build
 * system with correct path information (if required).
 *
 * The most basic invocation would be <tt>./configure</tt>, which produces:
 * @code
 * $ ./configure
 * Configuring ginnungagap 0.6.0
 *  o generating `Makefile.config'... done
 *  o generating `config.h'... done
 *
 * Summary
 *  o features in use/code properties:
 *      WITH_SPRNG	1
 *      WITH_FFT_FFTW3	1
 *      NDIM	3
 *      ENABLE_WRITING	1
 *  o features NOT in use:
 *      WITH_OPENMP
 *      WITH_MPI
 *      WITH_MPITRACE
 *      WITH_SILO
 *      WITH_HDF5
 *      WITH_FFT_FFTW2
 *      ENABLE_DOUBLE
 *      ENABLE_DEBUG
 *      WITH_PROC_DIR
 *  o programs and program options:
 *      CC       = gcc
 *      MPICC    =
 *      CFLAGS   =  -Wall -std=c99
 *      CPPFLAGS =  -I/include
 *      DEPCC    = gcc
 *      LD       = ld
 *      LDFLAGS  =  -L/lib
 *      LIBS     =  -lsprng -lfftw3 -lfftw3f -L/usr/lib -lgsl -lgslcblas -lm
 *      AR       = ar
 *      MAKE     = make
 *
 * To build ginnungagap, run `make all'.
 * To generate the code documention execute `make doc'.
 *
 * Have a lot of fun!
 * @endcode
 * @b Note: The exact details of that output may be different for you.
 *
 * The interesting part is the Summary section, which will tell you quickly
 * what options are and are not in use; in this case, only SPRNG and FFTW3
 * are used.  Also, the tools used to compile the code and their parameters
 * are listed.
 *
 * @section pageConfigureScript_Feature  Enabling a Feature
 *
 * To enable a feature, the suitable @c configure switch(es) must be
 * provided.  For activating HDF5:
 * @code
 * $ ./configure --with-hdf5
 * [..]
 * Summary
 *  o features in use/code properties:
 *      WITH_SPRNG	1
 *      WITH_HDF5	1
 *      WITH_FFT_FFTW3	1
 *      NDIM	3
 *      ENABLE_WRITING	1
 *  o features NOT in use:
 *      WITH_OPENMP
 *      WITH_MPI
 *      WITH_MPITRACE
 *      WITH_SILO
 *      WITH_FFT_FFTW2
 *      ENABLE_DOUBLE
 *      ENABLE_DEBUG
 *      WITH_PROC_DIR
 *  o programs and program options:
 *      CC       = gcc
 *      MPICC    =
 *      CFLAGS   =  -Wall -std=c99
 *      CPPFLAGS =  -I/include
 *      DEPCC    = gcc
 *      LD       = ld
 *      LDFLAGS  =  -L/lib
 *      LIBS     =  -lsprng -lhdf5 -lz -lm -lfftw3 -lfftw3f -L/usr/lib \
 *                  -lgsl -lgslcblas -lm -lm
 *      AR       = ar
 *      MAKE     = make
 * [..]
 * @endcode
 * Note that WITH_HDF5 now moved to the features in use and the required
 * libraries have been added to LIBS.
 *
 * To be able to compile the code now (by running @c make), the paths need
 * to be properly set so that the compiler can find the necessary header
 * files and the linker the necessary (static) libraries.
 *
 * The configure script provides standardized ways to do this, i.e.
 * <dl>
 *  <dt><tt>--with-hdf5-prefix</tt></dt>
 *  <dd>Gives the prefix under which the HDF5 library is installed.</dd>
 *  <dt><tt>--with-hdf5-inc-dir</tt></dt>
 *  <dd>Gives the actual directory in which to find the HDF5 header.</dd>
 *  <dt><tt>--with-hdf5-lib-dir</tt></dt>
 *  <dd>The directory in which to find the libaries.</dd>
 *  <dt><tt>--with-hdf5-libs</tt></dt>
 *  <dd>The LIBS required for linking.</dd>
 * </dl>
 *
 * To tell the build system to use the HDF5 installation in
 * <tt>/some/obscure/location/hdf5-1.8.7/</tt>, the proper command would be:
 * @code
 * $ ./configure --with-hdf5 \
 *               --with-hdf5-prefix=/some/obscure/location/hdf5-1.8.7/
 * @endcode
 * this will then search for the headers in
 * <tt>/some/obscure/location/hdf5-1.8.7/include</tt> and the libraries in
 * <tt>/some/obscure/location/hdf5-1.8.7/lib</tt>.  An equivalent invocation
 * would be
 * @code
 * $ ./configure --with-hdf5 \
 *         --with-hdf5-inc-dir=/some/obscure/location/hdf5-1.8.7/include
 *         --with-hdf5-lib-dir=/some/obscure/location/hdf5-1.8.7/lib
 * @endcode
 * @b Note: The version explicitly giving the include and the library
 * directory are only needed if the installation of the feature that should
 * be used it pretty obscure indeed.  In most cases it should be sufficient
 * to specify the correct @c prefix.
 *
 * @section pageConfigureScript_FeatureAvail Available Features
 *
 * To get an up-to-date list of options run
 * @code
 * $ ./configure --help
 * @endcode
 *
 * This will also provide you with detailed explanations of what the
 * different options do.
 *
 * @section pageConfigureScript_Systems Using Systems
 *
 * It can become very tedious to specify the correct prefix information
 * every time the configure script is run.  To ease in that, the script
 * provides a special flag <tt>--system</tt>.
 *
 * This will setup the required path-information for a given system (that
 * the build system knows about). I.e.
 * @code
 * ./configure --with-mpi --with-hdf5 --system=curie
 * @endcode
 * will configure the build to use MPI and HDF5 and the path information to
 * work on curie.
 *
 * The system information is read from <tt>configure.systems</tt> and, if it
 * exists, from <tt>configure.systems.own</tt>.  The former is distributed
 * with ginnungagap and should not be changed.  To use your own system
 * configuration, you should write your own <tt>configure.systems.own</tt>.
 * In <tt>doc/examples/</tt> you can find a template file.
 *
 * To define a system @c myMachine that sets up the path information for
 * some things, the @c configure.systems.own should look like
 * @code
 * OWNSYSTEMTYPE=true
 * case $SYSTEMTYPE in
 *     myMachine)
 *         WITH_MPI=true
 *         WITH_MPI_PREFIX=/opt/openmpi
 *         WITH_FFT_PREFIX=/opt/fftw
 *         WITH_SPRNG_INC_DIR=/opt/sprng/include
 *         WITH_SPRNG_LIB_DIR=/opt/sprng/lib
 *         WITH_HDF5_PREFIX=/some/obscure/location/hdf5-1.8.7/
 *         ;;
 *     *)
 *         OWNSYSTEMTYPE=false
 *         ;;
 * esac
 * @endcode
 *
 * It is possible to use advanced configuration Voodoo in this
 * file, i.e. all flags which are being defined in the configure script are
 * available to <tt>configure.systems.own</tt>.  It is hence possible to
 * conditionally define things, e.g.
 * @code
 *         if test "x$WITH_HDF5" = "xtrue"
 *         then
 *             WITH_HDF5_PREFIX=/some/obscure/location/hdf5-1.8.7/
 *         fi
 * @endcode
 * For this to have the intended effect however, the <tt>--system</tt>
 * switch @b must come after the <tt>--with-hdf5</tt> switch.  It is hence
 * recommended to use the <tt>--system</tt> switch as late as possible in
 * the invocation of @c configure.
 */


/*--- Page: Writing INI Files -------------------------------------------*/

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
 * The ginnungagap package comes with a few example ini files in
 * @c doc/examples/.  In the next section, a simple ini file is presented,
 * where the meaning of each key-value pair is explained.
 *
 * @subsection pageInputfile_Examples_Simple  Very simple example
 *
 * @code
 *
 * [Ginnungagap]
 * # Using a 128^3 grid
 * dim1D = 128
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
 * ## Note:  Instead of the four lines above, you can also read a tabulated
 * ## power spectrum from a file with
 * #powerSpectrumFileName = myPk.txt
 *
 * [Output]
 * # The output is written as grafic format
 * type = grafic
 * # The outout files are prefixed with 'ic' (this will lead to files with
 * # the names 'ic_velx', and so on).
 * prefix = ic
 * # and the details are in that section.
 * writerSection = OutputGrafic
 *
 * [OutputGrafic]
 * prefix = ic_
 * # We do not write white noise files with this writer.
 * isWhiteNoise = false
 * # The size of the grid is 128^3 (MUST be consistent with dim1D)
 * size = 128, 128, 128
 * # This is boxsizeInMpch / dim1D / modelHubble
 * dx = 1.0893485915492958
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
 * # The section in which to look for the information on how to generate the
 * # random numbers.
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
 * # SPRNG Voodoo (sets the generator type, in this case, 4 is the lagged
 * # Fibonacci generator, find out more by reading the SPRNG
 * # documentation).
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


/*--- Page: Quickstart --------------------------------------------------*/

/**
 * @page pageQuickstart Quickstart
 *
 * The following explains how to use ginnungagap (and tools) to generated
 * initial conditions in Gadget format for a 128^3 and a 384^3 simulation
 * which both share the same large scale mode, i.e. the 384^3 increases the
 * resolution of the 128^3 base simulation by a factor 3 in each dimension.
 *
 * <ul>
 *  <li>@ref pageQuickstart_Build</li>
 *  <li>@ref pageQuickstart_Base</li>
 *  <li>@ref pageQuickstart_Scale</li>
 *  <li>@ref pageQuickstart_HighRes</li>
 *  <li>@ref pageQuickstart_RTW</li>
 * </ul>
 *
 * @section pageQuickstart_Build  Configuring and Compiling the Code
 *
 * The first step is to configure and compile the code to have the necessary
 * executables.  For this, unpack your ginnungagap tarball and run the
 * configure script:
 * @code
 * $ tar xf ginnungagap-0.6.0.tar.bz2
 * $ cd ginnungagap-0.6.0
 * $ ./configure --with-mpi --with-openmp
 * @endcode
 * @b Note: This requires your system to be setup correctly so that it will
 * find the MPI libraries/runtime, and the SPRNG and GSL libraries.  Try
 * <tt>./configure --help</tt> to find out how to do that or see
 * @ref pageConfigureScript.
 *
 * You can now build the code with
 * @code
 * $ make -j all
 * @endcode
 * (The -j is used to speed up the build process and may be safely omitted)
 *
 * This will leave you with all codes compiled but in their respective
 * source directories, as a convenience you may now run
 * @code
 * make install
 * @endcode
 * which will create a directory @c bin/ and move all executables there.
 * @b Note: You can specify the directory to which to move the binaries with
 * the @c --prefix configure-option.
 *
 * @section pageQuickstart_Base Generating the Base Box
 *
 * We will now generate the required fields for the 128^3 box.  For this,
 * first change into a new sub-directory:
 * @code
 * $ mkdir -p quickstart/128
 * $ cd quickstart/128
 * @endcode
 *
 * Now generate the ini file for ginnungagap so that
 * @code
 * $ cat 128.ini
 * [Ginnungagap]
 * dim1D = 128
 * boxsizeInMpch = 250
 * zInit = 80.0
 * normalisationMode = sigma8
 * gridName = testGrid
 * doHistograms = true
 * histogramNumBins = 67
 * histogramExtremeWN = 7.
 * histogramExtremeDens = .5
 * histogramExtremeVel = 250.
 *
 * [Output]
 * type = grafic
 * prefix = ic_128
 * writerSection = OutputGrafic
 *
 * [OutputGrafic]
 * isWhiteNoise = false
 * size = 128, 128, 128
 * dx = 2.750880281690141
 * astart = 0.012345679012345678
 * omegam = 0.2669
 * omegav = 0.734
 * h0 = 71
 *
 * [WhiteNoise]
 * useFile = false
 * rngSectionName = rng
 * dumpWhiteNoise = true
 * writerSection = WhiteNoiseWriter
 *
 * [WhiteNoiseWriter]
 * type = grafic
 * prefix = wn_128
 * isWhiteNoise = true
 * size = 128, 128, 128
 * iseed = 4422
 *
 * [MPI]
 * nProcs = 1 0 0
 *
 * [Cosmology]
 * modelOmegaRad0 = 8.348451673443855e-05
 * modelOmegaLambda0 = 0.734
 * modelOmegaMatter0 = 0.2669
 * modelOmegaBaryon0 = 0.0449
 * modelHubble = 0.71
 * modelSigma8 = 0.801
 * modelNs = 0.963
 * modelTempCMB = 2.725
 * powerSpectrumKmin = 1e-5
 * powerSpectrumKmax = 1e4
 * powerSpectrumNumPoints = 501
 * transferFunctionType = EisensteinHu1998
 *
 * [rng]
 * generator = 4
 * numStreamsTotal = 256
 * randomSeed = 4422
 * @endcode
 *
 * We can now run the code and generate the various fields for the 128^3
 * box:
 * @code
 * $ mpiexec -n 1 ../../bin/ginnungagap 128.ini
 * [..lots of output..]
 * $ ls
 * 128.ini             histogram.velz.dat  ic_128_velz         Pk.wn.dat
 * 128.ini.dump        histogram.wn.dat    Pk.deltak.dat       wn_128_delta
 * histogram.dens.dat  ic_128_delta        Pk.input.dat
 * histogram.velx.dat  ic_128_velx         Pk.input_z0.dat
 * histogram.vely.dat  ic_128_vely         Pk.input_zinit.dat
 * @endcode
 * The interesting fields are @c ic_128_velx, @c ic_128_vely,
 * @c ic_128_velz, and @c wn_128_delta, which are the velocity fields and
 * the white-noise field, respectively.  The files prefixed with @c Pk are
 * the power spectra, with @c input being the input P(k), @c delta the power
 * spectrum of the actual realization, and @c wn the power spectrum of the
 * white noise.  Additionally, histograms for the various fields are also
 * computed and stored in the files prefixed with @c histogram.  The file
 * @c 128.ini.dump is input ini file with all fields that were not used
 * commented out (it is worthwhile to check the file to see if everything
 * that was set was also used, for this example, it should).
 *
 * With the velocity fields at hand, we can generate the Gadget file by
 * using one of the tools provided with ginnungagap:
 * @code
 * $ ../../bin/grafic2gadget --omegaBaryon0 0.0449 --numfiles 1 \
 *                           ic_128_velx ic_128_vely ic_128_velz \
 *                           128.dat
 * @endcode
 *
 * @section pageQuickstart_Scale Increasing the Resolution
 *
 * We will now use the just generated 128^3 box to generate the same box at
 * a higher resolution.  First, let's change the directory
 * @code
 * $ mkdir ../384
 * $ cd ../384
 * @endcode
 *
 * We now need to create an ini file for the scaling, i.e.
 * @code
 * $ cat scale.ini
 * [Setup]
 * boxsizeInMpch = 250
 * inputDim1D = 128
 * outputDim1D = 384
 * useFileForInput = true
 * seedIn = 221
 * seedOut = 1235135
 * readerSecName = InputReader
 * writerSecName = OutputWriter
 *
 * [InputReader]
 * type = grafic
 * path = ../128
 * prefix = wn_128
 * qualifier = _delta
 *
 * [OutputWriter]
 * type = grafic
 * prefix = wn_384
 * isWhiteNoise = true
 * size = 384, 384, 384
 * iseed = 1235135
 * @endcode
 * Then run
 * @code
 * $ mpiexec -n 1 ../../bin/realSpaceConstraints scale.ini
 * @endcode
 * This will produce the file @c wn_384_delta which contains the properly
 * enhanced white noise field for the higher resolution box.
 *
 * @section pageQuickstart_HighRes Generating the High Resolution Box
 *
 * We will now have to do essentially the same as in
 * @ref pageQuickstart_Base, only the ini file will be slightly different.
 * We can omit the @c [RNG] section and have to adjust a few things to
 * account for the higher resolution, i.e.
 * @code
 * $ cat 384.ini
 * [Ginnungagap]
 * dim1D = 384
 * boxsizeInMpch = 250
 * zInit = 80.0
 * normalisationMode = sigma8
 * gridName = testGrid
 * doHistograms = true
 * histogramNumBins = 67
 * histogramExtremeWN = 7.
 * histogramExtremeDens = .5
 * histogramExtremeVel = 250.
 *
 * [Output]
 * type = grafic
 * prefix = ic_128
 * writerSection = OutputGrafic
 *
 * [OutputGrafic]
 * isWhiteNoise = false
 * size = 384, 384, 384
 * dx = 0.9169600938967136
 * astart = 0.012345679012345678
 * omegam = 0.2669
 * omegav = 0.734
 * h0 = 71
 *
 * [WhiteNoise]
 * useFile = true
 * rngSectionName = rng
 * dumpWhiteNoise = false
 * readerSection = WhiteNoiseReader
 *
 * [WhiteNoiseReader]
 * type = grafic
 * prefix = wn_384
 * qualifier = _delta
 * isWhiteNoise = true
 *
 * [MPI]
 * nProcs = 1 0 0
 *
 * [Cosmology]
 * modelOmegaRad0 = 8.348451673443855e-05
 * modelOmegaLambda0 = 0.734
 * modelOmegaMatter0 = 0.2669
 * modelOmegaBaryon0 = 0.0449
 * modelHubble = 0.71
 * modelSigma8 = 0.801
 * modelNs = 0.963
 * modelTempCMB = 2.725
 * powerSpectrumKmin = 1e-5
 * powerSpectrumKmax = 1e4
 * powerSpectrumNumPoints = 501
 * transferFunctionType = EisensteinHu1998
 * @endcode
 *
 * Now it is just a question of generating the velocity fields and from
 * those the Gadget ICs just as we did before (with adjusted file names, of
 * course). To produce a zoomed ICs which will combine particles from different
 * resolution level, see @ref pageGenerateICs .
 *
 * @section pageQuickstart_RTW Writing a specified region of the mesh 
 * Sometimes it is needed to write not the full output wite noise and velocity fields, but only some part. For example, this is needed to save disk space when producing high resolution zoomed simulations.
 * This can be done only with the HDF5 writer. To set up the patch to be written, there is an optional key in the writer section:
 * @code
 * [OutputHDF5]
 * ...
 * doPatch = true
 * patchSection = Patch
 * @endcode
 * 
 * @code
 * [Patch]
 * unit = cells
 * patchLo = 384 300 186
 * patchDims = 256 256 190
 * @endcode
 *
 * If the doPatch keyword is ommitted, the feature is disabled. The patch section should contain three keywords:
 * <dl>
 *     <dt>unit</dt><dd>Unit of patch coordinates, either "cells" (numbered from 0), or "Mpch"</dd>
 *     <dt>patchLo</dt><dd>Lower patch vertix coordinates</dd>
 *     <dt>patchDims</dt><dd>Patch dimensions. If you are using chunks, at least one dimension must be greater than the chunk size.</dd>
 * </dl>
 * 
 * The patch section can also be used in GenerateICs tool to tell it that the input data is only a part of the full mesh. See @ref pageGenerateICs_RTW
 */

/*--- Page: Example of ICs with zoom ---------------------------------------*/
/**
 * @page pageZoomExample Zoom example
 * 
 * Zoomed initial conditions can be imagined as a set of particles which are extracted from 
 * several homogeneous initial conditions with different resolutions and combined together.
 * These homogeneous initial conditions should 
 * contain the same large-scale phases and hereafter they are called 'levels'.
 * 
 * The workflow for producing a multi-level ICs:
 *
 * 1) Suppose you have a white noise field for one level.<br>
 * 2) Use realSpaceConstraints to produce white noise fields with 
 * all steps in resolution you need (see @ref pageQuickstart_Scale). This will provide the same 
 * large-scale phases.<br>
 * 3) Run ginnungagap for each white noise field to make velocity fields for each level.<br>
 * 4) Produce a mask, e.g. by running and analysing a low resolution simulation.<br>
 * 5) Run generateICs for each level of resolution to extract the corresponding part using the mask
 * and write the result as GADGET-2 files.
 * 
 * As a result, a set of GADGET-2 files will be produced. GenerateICs uses
 * multiple-file format for storing such ICs, each level is represented by at least one file.
 * 
 * In order to produce ICs with N levels, (N-1) .ini files for rescaling the white noise
 * fields are needed. These .ini files are used only by realSpaceConstraints tool.
 * Besides this, N .ini files are needed to generate velocity fields and GADGET files.
 * So, for each level one .ini file is needed which will be used by both ginnungagap
 * and generateICs.
 * Additionaly, one .ini file may be used to produce ICs and run a non-zoom simulation
 * which is needed to construct the mask.
 * 
 * Here is a basic example of 3-level initial conditions. The 1D resolutions of the levels are
 * 32, 64 and 128 particles. The box size is 100 Mpc/h and the zoom region is a sphere of radius 15 Mpc/h
 * located at coordinates x=21, y=40, z=58 Mpc/h.
 * The files for the example can be found in the doc/examples/zoom directory.
 * 
 * To run the example, please, copy the executables of ginnungagap, generateICs, realSpaceConstraints,
 * LareWriter and Gadget-2 to the example directory. For this example ginnungagap and generateICs must
 * be compilled with the HDF5 support enabled (@ref pageDeps_HDF5).
 * 
 * First, file base32.ini is used to produce a white noise field and also the non-zoom simulation.
 * @code
 * ./ginnungagap base32.ini
 * ./generateICs base32.ini
 * @endcode
 * 
 * Run the simulation:
 * @code
 * ./Gadget nozoom.param
 * @endcode
 * 
 * While the simulation is running, you can scale the white noise fields:
 * @code
 * ./realSpaceConstraints scale64.ini
 * ./realSpaceConstraints scale128.ini
 * @endcode
 * 
 * When the file snapshot_000 is created in out_nozoom32, the mask can be produced:
 * @code
 * ./LareWriter
 * @endcode
 * 
 * Finally, the zoomed ICs are created:
 * @code
 * ./generateICs zoom32.ini
 * 
 * ./ginnungagap zoom64.ini
 * ./generateICs zoom64.ini
 * 
 * ./ginnungagap zoom128.ini
 * ./generateICs zoom128.ini
 * @endcode
 * Note that the lines which should change between the levels and the .ini files are preceded by a comment line
 * @code
 * # this is different for each level:
 * @endcode
 * 
 * To run the simulation:
 * @code
 * ./Gadget zoom.param
 * @endcode
 * 
 * Note that the box has been shifted to be centered on the zoom region. This
 * is enabled by setting autoCenter = true in the .ini files.
*/

/*--- Page: Using generateICs ---------------------------------------*/

/**
 * @page pageGenerateICs Using GenerateICs
 * 
 * GenerateICs is a tool for converting velocity fields produced by Ginnungagap
 * into GADGET-2 format. This tool also can combine several velocity fields
 * of different resolution in order to run zoom-in simulations.
 * 
 * <ul>
 *   <li>@ref pageGenerateICs_inifile</li>
 *   <ul>
 *     <li>@ref pageGenerateICs_subHierarchy</li>
 *     <li>@ref pageGenerateICs_subMask</li>
 *     <li>@ref pageGenerateICs_subLare</li>
 *     <li>@ref pageGenerateICs_subGenerateICs</li>
 *     <li>@ref pageGenerateICs_subInput</li>
 *     <li>@ref pageGenerateICs_subOutput</li>
 *   </ul>
 *   <li>@ref pageGenerateICs_MPI</li>
 *   <li>@ref pageGenerateICs_Mem</li>
 *   <li>@ref pageGenerateICs_singlelevel</li>
 *   <li>@ref pageGenerateICs_LareFormat</li>
 *   <li>@ref pageGenerateICs_RTW</li>
 * </ul>
 *
 * @section pageGenerateICs_inifile .ini file
 * For each zoom level there should be a separate .ini file.
 * The .ini file requires to have sections:
 * @code
 * [Ginnungagap]
 * [Cosmology]
 * 
 * [GenerateICs]
 * [Mask]
 * [Hierarchy]
 * [Lare]
 * [GenicsInput]
 * [GenicsInput_velx]
 * [GenicsInput_vely]
 * [GenicsInput_velz]
 * [GenicsOutput]
 * @endcode
 *
 * The first two sections are just the ones used by Ginnungagap to produce velocity fields.  
 * So these files can be made from the Ginnungagap .ini files used to prepare 
 * velocity fields by adding sections to them.
 * 
 * 
 * @subsection pageGenerateICs_subHierarchy The numbering of levels is defined from the [Hierarchy]
 * @code
 * [Hierarchy]
 * numLevels = 9
 * minDim1D = 1
 * factor = 2
 * @endcode
 * 
 * The levels are numbered from 0 to numLevels-1. The 1D dimension of i-th level grid is minDim1D*factor^i.
 * In the example above i-th level has dimension 2^i, so level 0 has dimension 1x1x1 and level 8 (the highest level) has 256x256x256.
 * 
 * 
 * @subsection pageGenerateICs_subMask The levels which are used for different purposes are defined in [Mask]
 * @code
 * [Mask]
 * maskLevel = 6
 * minLevel = 5
 * maxLevel = 8
 * tileLevel = 4
 * readerType = legacy
 * readerSection = Lare
 * @endcode
 * 
 * - minLevel to maxLevel are the levels that we are going to combine in the current set of ICs.
 * - maskLevel is the level for which we have the mask which will be read in [Lare] section.
 * - tileLevel defines the size of tiles used for memory optimization and the distribution of the output into several files. 
 * Each file contains at least one tile.
 * Also one must have tileLevel <= minLevel. The rule-of-tumb is to have 1D tile dimension equal to sqrt(1D particle mesh dimension).
 * If minDim1D = 1 in the [Hierarchy] section, then tileLevel = zoomlevel / 2 should be optimal. See @ref pageGenerateICs_Mem for
 * more details.
 * - readerType in the current version should be set to legacy, which means ascii reader.
 * - readerSection points to a section where mask is actually read.
 * 
 * 
 * @subsection pageGenerateICs_subLare The initial mask is read in [Lare]
 * @code
 * [Lare]
 * hasHeader = false
 * fileName = lare.dat
 * ngrid = 64 64 64
 * @endcode
 * 
 * - ngrid in each dimension should be equal to the mask level dimension
 * 
 * The lare.dat can be prepared by the LareWrite.f90 utility, see below.
 * 
 * @subsection pageGenerateICs_subGenerateICs The level of current .ini file and the Gadget types for each level are assigned in [GenerateICs]
 * @code
 * [GenerateICs]
 * ginnungagapSection = Ginnungagap
 * doGas = false
 * doLongIDs = false
 * autoCenter = true
 * inputSection = GenicsInput
 * outputSection = GenicsOutput
 * cosmologySection = Cosmology
 * maskSection = Mask
 * hierarchySection = Hierarchy
 * zoomLevel = 3
 * typeForLevel5 = 3
 * typeForLevel6 = 3
 * typeForLevel7 = 2
 * typeForLevel8 = 1
 * @endcode
 * 
 * zoomlevel is the level of the current .ini file. The lines with typeForLevelX 
 * must exist for each level between minLevel and maxLevel. They govern the assignement
 * of GADGET particle types to different levels. If two or more levels 
 * have the same type, they will be assigned particle masses for each particle. 
 * Otherwise massArr is used. Type 0 is gas, type 1 is halo, ...
 *
 * The particle IDs are assigned according to their Lagrangian coordinates 
 * at the highest resolution level. The transform is done with ::lIdx_fromCoord3d function.
 * 
 * The option doGas will enable the production of the gas particles. In zoom simulations,
 * this should be enabled only for the highest resolution level. The gas particles are produced
 * as a copy of DM particles with and offset of 1/2 interparticle separation.
 * 
 * The option doLongIDs controls whether the 32-bit or 64-bit IDs are used.
 * 
 * autoCenter enables the automatic search for the center of the zoomed
 * region(s) and shifting the box periodically. This is especially needed
 * when using GADGET-2 options '-DPLACEHIGHRESREGION', see GADGET manual for
 * more details.
 * 
 * @subsection pageGenerateICs_subInput Input velocity fields are specified in [GenicsInput]
 * This section contains the names of sections for reading x, y and z components of
 * the velocity field:
 * @code
 * [GenicsInput]
 * velxSection = GenicsInput_velx
 * velySection = GenicsInput_vely
 * velzSection = GenicsInput_velz
 * @endcode
 * 
 * An example of GenicsInput_velx:
 * @code
 * [GenicsInput_velx]
 * type = hdf5
 * path = ./
 * prefix = level5
 * qualifier = _velx
 * suffix = .h5
 * @endcode
 * 
 * @subsection pageGenerateICs_subOutput The number of files for each level is set in [GenicsOutput]
 * @code
 * [GenicsOutput]
 * numFilesForLevel5 = 1
 * numFilesForLevel6 = 1
 * numFilesForLevel7 = 1
 * numFilesForLevel8 = 2
 * prefix = gadget_ic
 * @endcode
 * 
 * in this example we will have:
 * 
 * gadget_ic.0 for level 5, Gadget type 3<br>
 * gadget_ic.1 for level 6, Gadget type 3<br>
 * gadget_ic.2 for level 7, Gadget type 2<br>
 * gadget_ic.3 and gadget_ic.4 for level 8, Gadget type 1<br>
 * 
 * When having several files for one level, the code tries to distribute the particles equally between them.
 * But remember that it is done with tiles. If the tile size is too big,
 * the whole zoom region may be fit into just one tile. In this case one
 * file will include all high resolution particles, and others will be empty.
 * To avoid this, increase tileLevel. But increasing it too much require
 *  more disk operations and can slow things down.
 * 
 * This section also can contain keywords for @ref pageGenerateICs_RTW .
 *
 * @section pageGenerateICs_MPI MPI-parallelization
 *
 * generateICs can be run in parallel. In this case the number of output
 * files for the current level must be greater or equal than the number of MPI-processes.
 *
 * @section pageGenerateICs_Mem Memory consumption
 * Each MPI-process of generateICs allocates memory for one GADGET file plus a patch for the input data which is called a tile,
 * plus the structure which holds the pointers to individual tiles. If the mask is used
 * for zoom ICs, then the mask structure is also allocated for each process.
 * 
 * The sizes of tiles and the number of pointers are determined by the tileLevel keyword in section [Mask]. The tileLevel defines the 1D
 * dimension of the grid of tiles. The number of particles in a tile is the total number of particles in the mesh,
 * devided by the number of tiles.
 * Hence the optimal choice of the tile dimension would be sqrt(mesh dimension).
 * Smaller value of tileLevel will result in larger tile size, larger tileLevel will result in larger pointer structure size.
 * 
 * At the same time, one should remember that tileLevel <= minLevel, and number of tiles >= number of output files.
 * 
 * 
 * @section pageGenerateICs_singlelevel Producing single level ICs
 * In order to produce ordinary ICs without zoom (for the whole box), minLevel = maxLevel must be set.
 * In this case, [Lare] will not be read, and empty mask will be used. However, 
 * zoomlevel and typeForLevel still must be present. This can be changed in 
 * the next version to make generateICs compatible with previous version of .ini files.
 * 
 * @section pageGenerateICs_LareFormat Format of the Lagrangian region mask
 * 
 * The Lagrangian Region (LaRe) file contains rows with the following information:
 * @code
 * id, pos[3], vel[3], i, j, k
 * @endcode
 * only the last three numbers are read by generateICs. These numbers i, j, k are the x, y and z indices of the cells which are labelled as high resolution.
 *
 * To prepare the mask there is an utility in tools/zoomTools: LareWrite.f90
 * 
 * To compile it with gfortran, cd to tools/zoomTools and type make.
 *
 * The parameters are given in LareWrite.tbl file:
 *
 * 1st line -- file path with halos in AHF output format<br>
 * 2nd line -- simulation snapshot file path<br>
 * 3rd line -- output file name<br>
 * 4th line -- how many adjacent cells to each initially selected cell will be also marked<br>
 * 5th line -- the multiplication factor for the halo virial radius
 * 
 * The parameter on the 4th line is used to avoid multiconnected zoom region, or holes of low resolution inside the
 * high resolution region.
 * 
 * Important note: it is assumed that the snapshot has distance units of Mpc/h while the AHF output is in kpc/h!
 * 
 * @section pageGenerateICs_RTW Using a partial input mesh
 * When using the partial mesh prepared as described in @ref pageQuickstart_RTW
 * one must tell generateICs that the input is not a full mesh, since 
 * this information is not stored in the HDF5 files. To do this, one 
 * should add two keywords to the generateICs input section:
 * @code
 * [GenicsInput]
 * ...
 * doPatch = true
 * patchSection = Patch
 * @endcode 
 * and also use the same Patch section that was used to write velocity fields by ginnungagap:
 * @code
 * [Patch]
 * unit = cells
 * patchLo = 384 300 186
 * patchDims = 256 256 190
 * @endcode
 *
 */

/*--- Page: External Dependencies ---------------------------------------*/

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
 * To compile ginnungagap you currently need both the single and double precision versions of FFTW.
 * 
 * This is single precision version compiled with the standard compiler
 * (must likely gcc):
 *
 * @code
 * ./configure --prefix=/opt/fftw/fftw-3.2.2-gcc \
 *             --enable-sse \
 *             --enable-float \
 *             --disable-fortran \
 *             --enable-openmp \
 * 			   --enable-threads
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
 *             --enable-openmp \
 *             --enable-threads
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
 *                    --enable-openmp \
 *                    --enable-threads
 * make -j && make install
 * @endcode
 *
 * This is double precision version compiled with icc:
 *
 * @code
 * CC=icc ./configure --prefix=/opt/fftw/fftw-3.2.2-intel \
 *                 --enable-sse2 \
 *                    --disable-fortran \
 *                    --enable-openmp \
 *                 --enable-threads
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
 * When building with MPI, add --enable-mpi to each of the ./configure examples above.
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
 * There are some hidden configure options.  To exclude deprecated
 * symbols from being exported, use
 * <code>--disable-deprecated-symbols</code>.  Here is the full line for
 * HDF5-1.8.7:
 *
 * @code
 * ./configure --prefix=/opt/hdf5/hdf5-1.8.7-gcc \
 *             --disable-fortran
 *             --disable-cxx \
 *             --with-pthread \
 *             --enable-production \
 *             --disable-deprecated-symbols
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
