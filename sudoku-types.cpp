#include "stdafx.h"
#include "sudoku-types.h"

/**
 * Sudoku workspace structure
 *
 * Linearized 10x10 array of int
 *
 * Meaning of entries:
 *	* [1..9][1..9]: if > 0: Bitmask for all possible candidates
 *	*				if < 0: entered, value is (-1) * Entry
 *	*				if = 0: contradiction!
 *	* [1..9][0]:	Bitmask for all undecided cells in row
 *	* [0][1..9]:	Bitmask for all undecided cells in column
 *	* [0][0]:		Multi marker
 *
 */

const int S_BLOCK_SIZE = S_100 * sizeof(int);

int wsTemplate[S_100];

void InitWorkspace(SudokuWorkspace workspace) {
	memcpy(workspace, wsTemplate, S_BLOCK_SIZE);
}

void InitWorkspaceTemplate() {
	for (int ii = 0; ii <= S_NINE; ii++) {
		for (int jj = 0; jj <= S_NINE; jj++) {
			wsTemplate[S_TEN * ii + jj] = S_ALL_BITS_SET;
		}
	}
	wsTemplate[0] = 1;
}

/**
 * Sudoku simple structure
 *
 * Linearized 9x9 array of int.
 * The entries are in the indices 1..9
 * The zero index is initialized, but as of yet ununsed
 *
 * This structure is reprensenting the Sudoku as read,
 * as well as the solution to be written out.
 */

void ClearSudoku(SudokuPuzzle sudoku) {
	for (int row = 0; row < S_NINE; row++) {
		for (int col = 0; col < S_NINE; col++) {
			sudoku[S_NINE * row + col] = 0;
		}
	}
}

void CopySudoku(SudokuPuzzle dest, SudokuPuzzle orig) {
	for (int row = 0; row < S_NINE; row++) {
		for (int col = 0; col < S_NINE; col++) {
			dest[S_NINE * row + col] = orig[S_NINE * row + col];
		}
	}
}
