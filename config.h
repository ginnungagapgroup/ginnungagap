// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef CONFIG_H
#define CONFIG_H


#ifdef _OPENMP
#  define WITH_OPENMP_TIMING
#endif

#define WITH_MPI

#define DEBUG
#define XMEM_TRACK_MEM


#define ENABLE_DOUBLE

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


//#define WITH_FFTW2
#define WITH_FFTW3
#if (defined WITH_FFTW2 && defined WITH_FFTW3)
#  pragma \
    message "WARNING: Do not define WITH_FFTW2 and WITH_FFTW3 at the same time!"
#  undef WITH_FFTW3
#endif


#define diediedie(message)                               \
    fprintf(stderr, "FATAL:  %s\n%s in %s:%i\n", \
            message, __func__, __FILE__, __LINE__);      \
    exit(EXIT_FAILURE);

#endif
