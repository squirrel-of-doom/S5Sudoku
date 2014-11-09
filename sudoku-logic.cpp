#include "stdafx.h"
#include "sudoku-logic.h"
#include "sudoku-solver.h"
#include "bitcount.h"

#include <string>

extern const int BITMASKS[] = {  511,	// 111111111 = 0 (for initialization and index shift)
								   1,	// 000000001 = 1
								   2,	// 000000010 = 2
								   4,	// 000000100 = 3
								   8,	// 000001000 = 4
								  16,	// 000010000 = 5
								  32,	// 000100000 = 6
								  64,	// 001000000 = 7
								 128,	// 010000000 = 8
								 256	// 100000000 = 9
						        };

extern const int REVERSE_BITMASKS[] = {  0,
	1,
	2,
	0, 3,
	0, 0, 0, 4,
	0, 0, 0, 0, 0, 0, 0, 5,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9
};

const int SQUARE_COORDS_ROW[] = 
	{1, 1, 1, 2, 2, 2, 3, 3, 3, 1, 1, 1, 2, 2, 2, 3, 3, 3, 1, 1, 1, 2, 2, 2, 3, 3, 3,
	 4, 4, 4, 5, 5, 5, 6, 6, 6, 4, 4, 4, 5, 5, 5, 6, 6, 6, 4, 4, 4, 5, 5, 5, 6, 6, 6,
	 7, 7, 7, 8, 8, 8, 9, 9, 9, 7, 7, 7, 8, 8, 8, 9, 9, 9, 7, 7, 7, 8, 8, 8, 9, 9, 9};
const int SQUARE_COORDS_COL[] = 
	{1, 2, 3, 1, 2, 3, 1, 2, 3, 4, 5, 6, 4, 5, 6, 4, 5, 6, 7, 8, 9, 7, 8, 9, 7, 8, 9,
	 1, 2, 3, 1, 2, 3, 1, 2, 3, 4, 5, 6, 4, 5, 6, 4, 5, 6, 7, 8, 9, 7, 8, 9, 7, 8, 9,
	 1, 2, 3, 1, 2, 3, 1, 2, 3, 4, 5, 6, 4, 5, 6, 4, 5, 6, 7, 8, 9, 7, 8, 9, 7, 8, 9};

const int SQUARE_IDX[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						  0, 1, 1, 1, 2, 2, 2, 3, 3, 3,
						  0, 1, 1, 1, 2, 2, 2, 3, 3, 3,
						  0, 1, 1, 1, 2, 2, 2, 3, 3, 3,
						  0, 4, 4, 4, 5, 5, 5, 6, 6, 6,
						  0, 4, 4, 4, 5, 5, 5, 6, 6, 6,
						  0, 4, 4, 4, 5, 5, 5, 6, 6, 6,
						  0, 7, 7, 7, 8, 8, 8, 9, 9, 9,
						  0, 7, 7, 7, 8, 8, 8, 9, 9, 9,
						  0, 7, 7, 7, 8, 8, 8, 9, 9, 9};

const int S_BLOCK_SIZE = S_100 * sizeof(int);

//#include <Windows.h>
//#define TIMING 1

/* Make an entry */

int MakeEntry(SudokuWorkspace workspace, int Row, int Column, int Number) {
	int iStatus;
	int nRowIdx = S_TEN * Row;
	int nCellIdx = nRowIdx + Column;
	
	// Somebody live here?
	if (workspace[nCellIdx] < 0) {
		if ((Number + workspace[nCellIdx]) == 0) {
			return S_OK;
		} else {
			workspace[0] = S_UNSOLVABLE;
			return S_CONTRADICTION;
		}
	}
	if ((workspace[nCellIdx] & BITMASKS[Number]) != BITMASKS[Number]) {
		return S_CONTRADICTION;
	}

	iStatus = S_OK;
	// Set entry
	workspace[nCellIdx] = -Number;
	// Eliminate field as fixed in 0-index
	workspace[nRowIdx] &= ~BITMASKS[Column];
	workspace[Column] &= ~BITMASKS[Row];

    if (S_OK != (iStatus = EliminateCandidate(workspace, Row, Column, Number))) { return iStatus; }

	return iStatus;
}

int EliminateCandidate(SudokuWorkspace workspace, int Row, int Column, int Number) {
    int bmRowEntered = 0, bmRowCands = 0, bmRowSet = 0;
    int bmColEntered = 0, bmColCands = 0, bmColSet = 0;
    int bmSquEntered = 0, bmSquCands = 0, bmSquSet = 0;
	int bmRowTwice = 0, bmColTwice = 0, bmSquTwice = 0;
    int bmInverse = ~BITMASKS[Number];

    int iRowIdx = S_TEN * Row;
    int iColIdx = Column;
    int iSquIdx = (SQUARE_IDX[iRowIdx + iColIdx] - 1) * S_NINE;
    
	int iToken  = 1;
	bool bDone = true;
    int iStatus = S_OK;

    while (iToken <= S_NINE) {
        iRowIdx += 1;
        iColIdx += S_TEN;

		if (workspace[iToken] > 0) { bDone = false; }

        if (IS_Entered(workspace[iRowIdx])) {
            bmRowEntered |= BITMASKS[-workspace[iRowIdx]];
            bmRowSet |= BITMASKS[iToken];
        } else {
            workspace[iRowIdx] &= bmInverse;
			bmRowTwice |= workspace[iRowIdx] & bmRowCands;
            bmRowCands |= workspace[iRowIdx];
        }

        if (IS_Entered(workspace[iColIdx])) {
            bmColEntered |= BITMASKS[-workspace[iColIdx]];
            bmColSet |= BITMASKS[iToken];
        } else {
            workspace[iColIdx] &= bmInverse;
			bmColTwice |= workspace[iColIdx] & bmColCands;
            bmColCands |= workspace[iColIdx];
        }

        int nSquareIdx = S_TEN * SQUARE_COORDS_ROW[iSquIdx] + SQUARE_COORDS_COL[iSquIdx];
        if (IS_Entered(workspace[nSquareIdx])) {
            bmSquEntered |= BITMASKS[-workspace[nSquareIdx]];
            bmSquSet |= BITMASKS[iToken];
        } else {
            workspace[nSquareIdx] &= bmInverse;
			bmSquTwice |= workspace[nSquareIdx] & bmSquCands;
            bmSquCands |= workspace[nSquareIdx];
        }

        iSquIdx += 1;
        iToken += 1;
    }

    // check  ccontradictions found <-> cands
    if ((bmRowEntered ^ bmRowCands) != S_ALL_BITS_SET) { return S_CONTRADICTION; }
    if ((bmColEntered ^ bmColCands) != S_ALL_BITS_SET) { return S_CONTRADICTION; }
    if ((bmSquEntered ^ bmSquCands) != S_ALL_BITS_SET) { return S_CONTRADICTION; }

	if (bDone) { return S_DONE; }

    // check only one cell empty
    if (BITCOUNT[bmRowSet] == 8) {
        iStatus = MakeEntry(workspace, Row, REVERSE_BITMASKS[S_ALL_BITS_SET ^ bmRowSet], REVERSE_BITMASKS[S_ALL_BITS_SET ^ bmRowEntered]);
        if (iStatus != S_OK) { return iStatus; }
    }
    if (BITCOUNT[bmColSet] == 8) {
        iStatus = MakeEntry(workspace, REVERSE_BITMASKS[S_ALL_BITS_SET ^ bmColSet], Column, REVERSE_BITMASKS[S_ALL_BITS_SET ^ bmColEntered]);
        if (iStatus != S_OK) { return iStatus; }
    }
    if (BITCOUNT[bmSquSet] == 8) {
		int iSquLookup = iSquIdx - S_NINE + REVERSE_BITMASKS[S_ALL_BITS_SET ^ bmSquSet] - 1;
		iStatus = MakeEntry(workspace, SQUARE_COORDS_ROW[iSquLookup], SQUARE_COORDS_COL[iSquLookup], REVERSE_BITMASKS[S_ALL_BITS_SET ^ bmSquEntered]);
        if (iStatus != S_OK) { return iStatus; }
    }

	// check singke candidate in group
	if (bmRowCands ^ bmRowTwice) {
		bmRowCands ^= bmRowTwice; // contains now the single candidates in this group
		for (iToken = 1, iRowIdx = Row * S_TEN + 1; iToken <= S_NINE; iToken++, iRowIdx++) {
			if (!IS_Entered(workspace[iRowIdx]) && (workspace[iRowIdx] & bmRowCands)) {
				if (REVERSE_BITMASKS[workspace[iRowIdx] & bmRowCands] > 0) {
					iStatus = MakeEntry(workspace, Row, iToken, REVERSE_BITMASKS[workspace[iRowIdx] & bmRowCands]);
			        if (iStatus != S_OK) { return iStatus; }
				} else {
					return S_CONTRADICTION;
				}
			}
		}
	}
	if (bmColCands ^ bmColTwice) {
		bmColCands ^= bmColTwice; // contains now the single candidates in this group
		for (iToken = 1, iColIdx = S_TEN + Column; iToken <= S_NINE; iToken++, iColIdx += S_TEN) {
			if (!IS_Entered(workspace[iColIdx]) && (workspace[iColIdx] & bmColCands)) {
				if (REVERSE_BITMASKS[workspace[iColIdx] & bmColCands] > 0) {
					iStatus = MakeEntry(workspace, iToken, Column, REVERSE_BITMASKS[workspace[iColIdx] & bmColCands]);
			        if (iStatus != S_OK) { return iStatus; }
				} else {
					return S_CONTRADICTION;
				}
			}
		}
	}
	if (bmSquCands ^ bmSquTwice) {
		bmSquCands ^= bmSquTwice; // contains now the single candidates in this group
		int nSquareIndex = 0;
		for (iToken = 1, iSquIdx = (SQUARE_IDX[Row * S_TEN + Column] - 1) * S_NINE; iToken <= S_NINE; iToken++, iSquIdx++) {
			nSquareIndex = SQUARE_COORDS_ROW[iSquIdx] * S_TEN + SQUARE_COORDS_COL[iSquIdx];
			if (!IS_Entered(workspace[nSquareIndex]) && (workspace[nSquareIndex] & bmSquCands)) {
				if (REVERSE_BITMASKS[workspace[nSquareIndex] & bmSquCands] > 0) {
					iStatus = MakeEntry(workspace, SQUARE_COORDS_ROW[iSquIdx], SQUARE_COORDS_COL[iSquIdx], REVERSE_BITMASKS[workspace[nSquareIndex] & bmSquCands]);
			        if (iStatus != S_OK) { return iStatus; }
				} else {
					return S_CONTRADICTION;
				}
			}
		}
	}

	return iStatus;
}

int BacktrackInCell(SudokuWorkspace workspace, int Row, int Column) {
	int iStatus = S_OK;
	int nCellCands;
	int wsTemp[S_100];
	int iNumberThatSolves = 0;
	int wsSolved[S_100];

#ifdef TIMING
	LARGE_INTEGER TS, TE, Freq;
	QueryPerformanceFrequency( &Freq );
	double dInvFreq = 1000.0 / static_cast<double>( Freq.QuadPart );
	QueryPerformanceCounter( &TS );
#endif
	
	nCellCands = workspace[S_TEN * Row + Column];
	if (IS_Entered(nCellCands)) { return S_OK; }

	for (int num = 1; num <= S_NINE; num++) {
		if (nCellCands == 0) {
			break;
		}
		if ((nCellCands & BITMASKS[num]) != BITMASKS[num]) { 
			continue; 
		}
		memcpy(wsTemp, workspace, S_BLOCK_SIZE);
 		iStatus = MakeEntry(wsTemp, Row, Column, num);

		if (iStatus == S_OK) {
			iStatus = SolveByBacktracking(wsTemp);
		}
		if (iStatus == S_CONTRADICTION) {
			nCellCands &= ~BITMASKS[num];
			workspace[S_TEN * Row + Column]	&= ~BITMASKS[num];
		} else if (iStatus == S_DONE) {
			if (iNumberThatSolves > 0) { 
				memcpy(workspace, wsSolved, S_BLOCK_SIZE);
				workspace[0] = S_MULTI;
				iStatus = S_MULTIPLE;
				nCellCands = 0;
				return iStatus;
			} else {
				nCellCands &= ~BITMASKS[num];
				iNumberThatSolves = num;
				if (workspace[0] == S_MULTI) {
					memcpy(workspace, wsTemp, S_BLOCK_SIZE);
					return S_MULTIPLE;
				} else {
					memcpy(wsSolved, wsTemp, S_BLOCK_SIZE);
				}
			}
		} else {
			if (iStatus == S_MULTIPLE) {
				memcpy(workspace, wsTemp, S_BLOCK_SIZE);
			}
			return iStatus;
		}
	}
	if (iNumberThatSolves > 0) {
		memcpy(workspace, wsSolved, S_BLOCK_SIZE);
		return S_DONE;
	}
#ifdef TIMING
	QueryPerformanceCounter( &TE );
	printf("IN CELL elapsed (level %d): %f ms\n", 0, ((static_cast<double>( TE.QuadPart - TS.QuadPart )) * dInvFreq));
#endif

	return iStatus;
}

int FixAndGetNextIndex(SudokuWorkspace workspace, int &Row, int &Column) {
	int iCell;
	int iRowOffset, iCol;
	int iMinCount, iMinCell;
	int iMaxNumber, iThisNumber;
	int *iCountNumber;
	int iStatus;
	bool bEntryMade = true;

	while (bEntryMade) {
		bEntryMade = false;

		iMinCount = S_NINE;
		iMinCell = -1;

		iMaxNumber = 0;
		iCountNumber = (int*)calloc(S_TEN, sizeof(int));

		iCell = 0;
		iRowOffset = S_TEN; iCol = 1;
		while (iCell < S_81) {
			if (!IS_Entered(workspace[iRowOffset + iCol])) {
				if (BITCOUNT[workspace[iRowOffset + iCol]] <= 1) {
					if (BITCOUNT[workspace[iRowOffset + iCol]] == 0) {
						return S_CONTRADICTION;
					}
					iStatus = MakeEntry(workspace, iRowOffset / S_TEN, iCol, REVERSE_BITMASKS[workspace[iRowOffset + iCol]]);
					if (iStatus != S_OK) { return iStatus; }
					bEntryMade = true;
				} else if (BITCOUNT[workspace[iRowOffset + iCol]] < iMinCount) {
					iMinCount = BITCOUNT[workspace[iRowOffset + iCol]];
					iMinCell = iCell;
				} else if ((BITCOUNT[workspace[iRowOffset + iCol]] == iMinCount) && 
						   (workspace[iRowOffset + iCol] & BITMASKS[iMaxNumber])) {
					iMinCell = iCell;
				}
			} else {
				iThisNumber = -workspace[iRowOffset + iCol];
				iCountNumber[iThisNumber] += 1;
				if (iCountNumber[iThisNumber] > iCountNumber[iMaxNumber]) {
					iMaxNumber = iThisNumber;
				}
			}
			iCell += 1;
			iCol += 1;
			if (iCol > S_NINE) {
				iRowOffset += S_TEN;
				iCol = 1;
			}
		}
	}
	if (iMinCell >= 0) {
		Row = (iMinCell / 9) + 1;
		Column = (iMinCell % 9) + 1;
	}
	free(iCountNumber);
	return S_OK;
}
