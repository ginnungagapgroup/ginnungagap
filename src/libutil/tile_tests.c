// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `voelva'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  libutil/tile_tests.h
 * @ingroup  libutilMiscTile
 * @brief  Implements the tests for tile.c
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "tile_tests.h"
#include "tile.h"
#include <stdio.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif


/*--- Implementations of exported functions -----------------------------*/
extern bool
tile_calcIdxsELAE_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	uint32_t idxLo, idxHi;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	tile_calcIdxsELAE(27, 5, 0, &idxLo, &idxHi);
	if ((idxLo != 0) || (idxHi != 4))
		hasPassed = false;
	tile_calcIdxsELAE(27, 5, 1, &idxLo, &idxHi);
	if ((idxLo != 5) || (idxHi != 9))
		hasPassed = false;
	tile_calcIdxsELAE(27, 5, 2, &idxLo, &idxHi);
	if ((idxLo != 10) || (idxHi != 14))
		hasPassed = false;
	tile_calcIdxsELAE(27, 5, 3, &idxLo, &idxHi);
	if ((idxLo != 15) || (idxHi != 20))
		hasPassed = false;
	tile_calcIdxsELAE(27, 5, 4, &idxLo, &idxHi);
	if ((idxLo != 21) || (idxHi != 26))
		hasPassed = false;

	tile_calcIdxsELAE(12, 4, 0, &idxLo, &idxHi);
	if ((idxLo != 0) || (idxHi != 2))
		hasPassed = false;
	tile_calcIdxsELAE(12, 4, 1, &idxLo, &idxHi);
	if ((idxLo != 3) || (idxHi != 5))
		hasPassed = false;
	tile_calcIdxsELAE(12, 4, 2, &idxLo, &idxHi);
	if ((idxLo != 6) || (idxHi != 8))
		hasPassed = false;
	tile_calcIdxsELAE(12, 4, 3, &idxLo, &idxHi);
	if ((idxLo != 9) || (idxHi != 11))
		hasPassed = false;

	return hasPassed ? true : false;
}

extern bool
tile_calcTileNumberForIdxELAE_test(void)
{
	bool hasPassed = true;
	int  rank      = 0;
	uint32_t tileNumber,correct, idx;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	// Grid of 20 in 4 tiles (5 cells in each tile)
	for (idx = 0, correct = 0; correct < 4; correct++) {
		for (int i = 0; i < 5; i++) {
			tileNumber = tile_calcTileNumberForIdxELAE(20, 4, idx++);
			if (tileNumber != correct)
				hasPassed = false;
		}
	}

	// Grid of 27 in 5 tiles, 3x5 + 2x6
	for (idx = 0, correct = 0; correct < 3; correct++) {
		for (int i=0; i < 5; i++) {
			tileNumber = tile_calcTileNumberForIdxELAE(27, 5, idx++);
			if (tileNumber != correct)
				hasPassed = false;
		}
	}
	for (; correct < 5; correct++) {
		for (int i=0; i < 6; i++) {
			tileNumber = tile_calcTileNumberForIdxELAE(27, 5, idx++);
			if (tileNumber != correct)
				hasPassed = false;
		}
	}

	return hasPassed ? true : false;
}

extern bool
tile_calcIdxsELAB_test(void)
{
	bool hasPassed = true;
	int  rank      = 0;
	uint32_t idxLo, idxHi;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	tile_calcIdxsELAB(27, 5, 0, &idxLo, &idxHi);
	if ((idxLo != 0) || (idxHi != 5))
		hasPassed = false;
	tile_calcIdxsELAB(27, 5, 1, &idxLo, &idxHi);
	if ((idxLo != 6) || (idxHi != 11))
		hasPassed = false;
	tile_calcIdxsELAB(27, 5, 2, &idxLo, &idxHi);
	if ((idxLo != 12) || (idxHi != 16))
		hasPassed = false;
	tile_calcIdxsELAB(27, 5, 3, &idxLo, &idxHi);
	if ((idxLo != 17) || (idxHi != 21))
		hasPassed = false;
	tile_calcIdxsELAB(27, 5, 4, &idxLo, &idxHi);
	if ((idxLo != 22) || (idxHi != 26))
		hasPassed = false;

	tile_calcIdxsELAB(12, 4, 0, &idxLo, &idxHi);
	if ((idxLo != 0) || (idxHi != 2))
		hasPassed = false;
	tile_calcIdxsELAB(12, 4, 1, &idxLo, &idxHi);
	if ((idxLo != 3) || (idxHi != 5))
		hasPassed = false;
	tile_calcIdxsELAB(12, 4, 2, &idxLo, &idxHi);
	if ((idxLo != 6) || (idxHi != 8))
		hasPassed = false;
	tile_calcIdxsELAB(12, 4, 3, &idxLo, &idxHi);
	if ((idxLo != 9) || (idxHi != 11))
		hasPassed = false;

	return hasPassed ? true : false;
}

extern bool
tile_calcTileNumberForIdxELAB_test(void)
{
	bool hasPassed = true;
	int  rank      = 0;
	uint32_t tileNumber,correct, idx;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	// Grid of 20 in 4 tiles (5 cells in each tile)
	for (idx = 0, correct = 0; correct < 4; correct++) {
		for (int i = 0; i < 5; i++) {
			tileNumber = tile_calcTileNumberForIdxELAB(20, 4, idx++);
			if (tileNumber != correct)
				hasPassed = false;
		}
	}

	// Grid of 27 in 5 tiles, 2x6 + 3x5
	for (idx = 0, correct = 0; correct < 2; correct++) {
		for (int i=0; i < 6; i++) {
			tileNumber = tile_calcTileNumberForIdxELAB(27, 5, idx++);
			if (tileNumber != correct)
				hasPassed = false;
		}
	}
	for (; correct < 5; correct++) {
		for (int i=0; i < 5; i++) {
			tileNumber = tile_calcTileNumberForIdxELAB(27, 5, idx++);
			if (tileNumber != correct)
				hasPassed = false;
		}
	}

	return hasPassed ? true : false;
}

extern bool
tile_calcNumLargeTilesEven_test(void)
{
	bool hasPassed = true;
	int  rank      = 0;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	if (tile_calcNumLargeTilesEven(27, 5) != 2)
		hasPassed = false;

	if (tile_calcNumLargeTilesEven(12, 4) != 0)
		hasPassed = false;

	return hasPassed ? true : false;
}

extern bool
tile_calcNumSmallTilesEven_test(void)
{
	bool hasPassed = true;
	int  rank      = 0;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	if (tile_calcNumSmallTilesEven(27, 5) != 3)
		hasPassed = false;

	if (tile_calcNumSmallTilesEven(12, 4) != 4)
		hasPassed = false;

	return hasPassed ? true : false;
}

extern bool
tile_calcMinTileSizeEven_test(void)
{
	bool hasPassed = true;
	int  rank      = 0;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	if (tile_calcMinTileSizeEven(27, 5) != 5)
		hasPassed = false;

	if (tile_calcMinTileSizeEven(12, 4) != 3)
		hasPassed = false;

	return hasPassed ? true : false;
}

extern bool
tile_calcMaxTileSizeEven_test(void)
{
	bool hasPassed = true;
	int  rank      = 0;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	if (tile_calcMaxTileSizeEven(27, 5) != 6)
		hasPassed = false;

	if (tile_calcMaxTileSizeEven(12, 4) != 3)
		hasPassed = false;

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
