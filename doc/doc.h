// Copyright (C) 2011 Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

/*
 * IMPORTANT
 * IMPORTANT  This files contains no useful code.  It only contains part
 * IMPORTANT  of the package documentation in doxygen format.  The file
 * IMPORTANT  only has the .h extension to ensure that doxygen uses the
 * IMPORTANT  correct language parser for this file and to enable
 * IMPORTANT  automatic syntax highlighting.
 * IMPORTANT
 */

/**
 * \mainpage Reference documentation for ginnungagap
 *
 * \author  Steffen Knollmann\n Grupo de Astrofísica\n Departamento de
 * Física Teórica C-15\n Universidad Autónoma de Madrid\nCantoblanco
 * 28049, Madrid\n steffen.knollmann@uam.es
 *
 * \n
 *
 * \section intro Introduction
 *
 * Ginnungagap is a code for generating cosmological initial conditions.
 * It is aimed at high scalability and ease of use.
 *
 * \section requirements  Requirements
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
 * \section history History
 *
 * Here is a chronological overview of the different versions of
 * ginnungagap:
 *
 * - 2011/02/03 <b>ginnungagap-0.3.4</b> <br />
 *   A new tool to generate masks that will be needed for the
 *   resimulation setup is added.  Support for the legacy format of the
 *   Lagrangian regions is implemented.  Also various updates and
 *   improvements of the code.
 * - 2010/09/15 <b>ginnungagap-0.3.3</b> <br />
 *   Bug-fixes to allow compiling on IBM machines.  The grid now can
 *   calculate simple statistics and prints them.
 * - 2010/08/04 <b>ginnungagap-0.3.2</b> <br />
 *   A new tool to convert Grafic files into Gadget.
 * - 2010/07/27 <b>ginnungagap-0.3.1</b> <br />
 *   Adding a tool to estimate the memory requirements of the code.
 *   Some minor bug-fixes and thread support for the FFTW3 back-end.
 * - 2010/07/22 <b>ginnungagap-0.3.0</b> <br />
 *   Some code clean-up, the fields are now correctly calculated.
 * - 2010/07/19 <b>ginnungagap-0.2.6</b> <br />
 *   The velocity field is now calculated, albeit not correctly
 *   normalized.
 * - 2010/07/19 <b>ginnungagap-0.2.5</b> <br />
 *   Modifying the handling of the power spectrum and correcting the
 *   units of the density field.
 * - 2010/07/17 <b>ginnungagap-0.2.4</b> <br />
 *   The grid writing is moved to an abstract interface to allow
 *   different output formats accessible with a common interface.  A
 *   small tool to convert Grafics files to bov is added.
 * - 2010/07/15 <b>ginnungagap-0.2.3</b> <br />
 *   Renormalization of the power spectrum is included, allowing to read
 *   grids from a bov.
 * - 2010/06/23 <b>ginnungagap-0.2.2</b> <br />
 *   A tool to generate a master file for the SILO output that allows
 *   visualisation of datasets split over multiple domains.
 * - 2010/06/02 <b>ginnungagap-0.2.1</b> <br />
 *   Added functionality to calculate the power spectrum according to
 *   the Eisenstein & Hu fitting formula.
 * - 2010/05/21 <b>ginnungagap-0.2.0</b> <br />
 *   First version.  Laid the groundwork of the grid infrastructure and
 *   the FFT.  The density field is calculated in Fourier space.
 */
