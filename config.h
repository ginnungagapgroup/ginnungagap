#ifndef CONFIG_H
#define CONFIG_H

/* 
 * Copyright (C) 2008, Steffen Knollmann
 * 
 * This file is part of `mtree2'.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA. 
 */


/**
 * \file  config.h
 *
 * Here you can configure the code features.
 */


/*************************************************************************\
 *   General defines                                                     *
\*************************************************************************/
//#define LONG_IDS
//#define REALLY_LARGE
//#define XMEM_TRACK_MEM

/**
 * If an OpenMP compiler is used, automatically switch on the OpenMP
 * Timing.
 */
#ifdef _OPENMP
#  define WITH_OPENMP_TIMING
#endif

/**
 * Used for figuring out if a halo is a progenitor of another.  If the
 * ratio of current_mass/progenitor_mass is larger than this number, it
 * will not be considered to be a progenitor.
 */
#define RATIO_MAX 2.0
/**
 * Same as above, but now the other end of the range.
 */
#define RATIO_MIN 0.5

/**
 * This is gives the maximum number of files a halo can go missing
 * before it is discarded.
 */
#define MAX_MISSING_FILES 5

/**
 * This defines the default algorithm that is used for sorting the
 * particle collections.  Possible values are 0 (stdlib implementation
 * of QuickSort) and 1 (Fast Radix Sort implemented in libsort).
 */
#define DEFAULT_SORTING_ALGORITHM 0


/*************************************************************************\
 *   Defines for datatypes, only edit when you really sure about it      *
\*************************************************************************/
#ifdef LONG_IDS
#	define id_t uint64_t
#	define SCNid SCNu64
#	define PRIid PRIu64
#else
#	define id_t uint32_t
#	define SCNid SCNu32
#	define PRIid PRIu32
#endif

#ifdef REALLY_LARGE
#	define num_t uint64_t
#	define SCNnum SCNu64
#	define PRInum PRIu64
#else
#	define num_t uint32_t
#	define SCNnum SCNu32
#	define PRInum PRIu32
#endif

#define BITS_USED_FOR_FILE 10


#endif /* CONFIG_H */
