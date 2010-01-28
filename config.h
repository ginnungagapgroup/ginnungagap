// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef CONFIG_H
#define CONFIG_H


/*--- Code Features (to be adjusted by the user) ------------------------*/
// Switch this on to use MPI with this code.
//#define WITH_MPI
// Ditto but for OpenMP usage.  It is fine to use both.
#define WITH_OPENMP
// Select the version of FFTW that will be used.  The parameter can be
// either '2' for FFTW2 or '3' for FFTW3.  The default is to use FFTW2,
// as the MPI interface of FFTW3 is declared alpha, you might try it
// though...
// Note that FFTW2 must be compiled with type prefix.
#define WITH_FFTW 2
// This will force the code to use double precision for all variables
// (density field, positions, velocities, etc...).  If this is not
// defined, single precision will be used instead.
#define ENABLE_DOUBLE
// This will produce additional debugging output.  This will also test
// assertions, which are otherwise ignored, the code will likely run
// slower.
#define ENABLE_DEBUG


/*--- Consequences of the Code Features (do NOT change) -----------------*/
#ifdef ENABLE_DEBUG
#  undef NDEBUG
#  define DEBUG
#  define XMEM_TRACK_MEM
#else
#  define NDEBUG
#  undef DEBUG
#  undef XMEM_TRACK_MEM
#endif

#ifdef WITH_OPENMP
#  ifndef _OPENMP
#    undef WITH_OPENMP
#  endif
#endif

#if (WITH_FFTW < 2 || WITH_FFTW > 3)
#  undef WITH_FFTW
#  define WITH_FFTW 2
#endif

#ifdef ENABLE_DOUBLE
typedef double fpv_t;
#  define FPV_C(a) a
#  define SCNfpv "lf"
#  define PRIfpv "g"
#else
typedef float fpv_t;
#  define FPV_C(a) a ## f
#  define SCNfpv "f"
#  define PRIfpv "g"
#endif


#endif
