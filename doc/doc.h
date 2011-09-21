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
 *   with MPI support.
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
 * - <b>HDF5</b> - This is not a library used directly by ginnungagap,
 *   but Silo can be compiled to use HDF5 as a file backend.  This is
 *   highly advised.
 * - <b>MPItrace</b> - Tracing utilities can be used to generate trace
 *   files of ginnungagap runs
 *
 * @section history History
 *
 * Here is a chronological overview of the different versions of
 * ginnungagap:
 *
 * - 2011/02/16 <b>ginnungagap-0.3.5</b>  @n
 *   The memory estimate tools is updated to include the file size of
 *   the velocity fields.
 * - 2011/02/03 <b>ginnungagap-0.3.4</b> @n
 *   A new tool to generate masks that will be needed for the
 *   resimulation setup is added.  Support for the legacy format of the
 *   Lagrangian regions is implemented.  Also various updates and
 *   improvements of the code.
 * - 2010/09/15 <b>ginnungagap-0.3.3</b> @n
 *   Bug-fixes to allow compiling on IBM machines.  The grid now can
 *   calculate simple statistics and prints them.
 * - 2010/08/04 <b>ginnungagap-0.3.2</b> @n
 *   A new tool to convert Grafic files into Gadget.
 * - 2010/07/27 <b>ginnungagap-0.3.1</b> @n
 *   Adding a tool to estimate the memory requirements of the code.
 *   Some minor bug-fixes and thread support for the FFTW3 back-end.
 * - 2010/07/22 <b>ginnungagap-0.3.0</b> @n
 *   Some code clean-up, the fields are now correctly calculated.
 * - 2010/07/19 <b>ginnungagap-0.2.6</b> @n
 *   The velocity field is now calculated, albeit not correctly
 *   normalized.
 * - 2010/07/19 <b>ginnungagap-0.2.5</b> @n
 *   Modifying the handling of the power spectrum and correcting the
 *   units of the density field.
 * - 2010/07/17 <b>ginnungagap-0.2.4</b> @n
 *   The grid writing is moved to an abstract interface to allow
 *   different output formats accessible with a common interface.  A
 *   small tool to convert Grafics files to bov is added.
 * - 2010/07/15 <b>ginnungagap-0.2.3</b> @n
 *   Renormalization of the power spectrum is included, allowing to read
 *   grids from a bov.
 * - 2010/06/23 <b>ginnungagap-0.2.2</b> @n
 *   A tool to generate a master file for the SILO output that allows
 *   visualisation of datasets split over multiple domains.
 * - 2010/06/02 <b>ginnungagap-0.2.1</b> @n
 *   Added functionality to calculate the power spectrum according to
 *   the Eisenstein & Hu fitting formula.
 * - 2010/05/21 <b>ginnungagap-0.2.0</b> @n
 *   First version.  Laid the groundwork of the grid infrastructure and
 *   the FFT.  The density field is calculated in Fourier space.
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
