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

const int ROW_IDX[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					   0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
					   0, 2, 2, 2, 2, 2, 2, 2, 2, 2,
					   0, 3, 3, 3, 3, 3, 3, 3, 3, 3,
					   0, 4, 4, 4, 4, 4, 4, 4, 4, 4,
					   0, 5, 5, 5, 5, 5, 5, 5, 5, 5,
					   0, 6, 6, 6, 6, 6, 6, 6, 6, 6,
					   0, 7, 7, 7, 7, 7, 7, 7, 7, 7,
					   0, 8, 8, 8, 8, 8, 8, 8, 8, 8,
                       0, 9, 9, 9, 9, 9, 9, 9, 9, 9};

const int COL_IDX[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			 		   0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
					   0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
					   0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
					   0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
					   0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
					   0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
					   0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
					   0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                       0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

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

const int WS_IDX[] = {11, 12, 13, 14, 15, 16, 17, 18, 19,
					  21, 22, 23, 24, 25, 26, 27, 28, 29,
					  31, 32, 33, 34, 35, 36, 37, 38, 39,
					  41, 42, 43, 44, 45, 46, 47, 48, 49,
					  51, 52, 53, 54, 55, 56, 57, 58, 59,
					  61, 62, 63, 64, 65, 66, 67, 68, 69,
					  71, 72, 73, 74, 75, 76, 77, 78, 79,
					  81, 82, 83, 84, 85, 86, 87, 88, 89,
                      91, 92, 93, 94, 95, 96, 97, 98, 99};

const int S_BLOCK_SIZE_100 = S_100 * sizeof(int);
const int S_BLOCK_SIZE_TEN = S_TEN * sizeof(int);

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
		for (iToken = 1, iRowIdx = S_TEN * Row + 1; iToken <= S_NINE; iToken++, iRowIdx++) {
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
        for (iToken = 1, iSquIdx = (SQUARE_IDX[S_TEN * Row + Column] - 1) * S_NINE; iToken <= S_NINE; iToken++, iSquIdx++) {
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

int BacktrackInCell(SudokuWorkspace* wsStack, int CellWs, size_t &nRecDepth) {
	int iStatus = S_OK;
	int nCellCands;
    //SudokuWorkspace workspace = wsStack[nRecDepth - 1];
	//SudokuWorkspace wsTemp = wsStack[nRecDepth];
	int nNumberThatSolves = 0;
	//SudokuWorkspace wsSolved = wsStack[S_SOLVED];

#ifdef TIMING
	LARGE_INTEGER TS, TE, Freq;
	QueryPerformanceFrequency( &Freq );
	double dInvFreq = 1000.0 / static_cast<double>( Freq.QuadPart );
	QueryPerformanceCounter( &TS );
#endif
	
	nCellCands = wsStack[nRecDepth - 1][CellWs];
	if (IS_Entered(nCellCands)) { return S_OK; }

	for (int iNumber = 1; iNumber <= S_NINE; iNumber++) {
		if (nCellCands == 0) {
			break;
		}
		if ((nCellCands & BITMASKS[iNumber]) != BITMASKS[iNumber]) { 
			continue; 
		}
        memcpy(wsStack[nRecDepth], wsStack[nRecDepth - 1], S_BLOCK_SIZE_100);
        iStatus = MakeEntry(wsStack[nRecDepth], ROW_IDX[CellWs], COL_IDX[CellWs], iNumber);

		if (iStatus == S_OK) {
			iStatus = SolveByBacktracking(wsStack, nRecDepth);
		}
		if (iStatus == S_CONTRADICTION) {
			nCellCands &= ~BITMASKS[iNumber];
			wsStack[nRecDepth - 1][CellWs]	&= ~BITMASKS[iNumber];
		} else if (iStatus == S_DONE) {
			if (nNumberThatSolves > 0) { 
                memcpy(wsStack[nRecDepth - 1], wsStack[S_SOLVED], S_BLOCK_SIZE_100);
				wsStack[nRecDepth - 1][0] = S_MULTI;
				iStatus = S_MULTIPLE;
				nCellCands = 0;
				return iStatus;
			} else {
				nCellCands &= ~BITMASKS[iNumber];
				nNumberThatSolves = iNumber;
				if (wsStack[nRecDepth - 1][0] == S_MULTI) {
                    memcpy(wsStack[nRecDepth - 1], wsStack[nRecDepth], S_BLOCK_SIZE_100);
					return S_MULTIPLE;
				} else {
                    memcpy(wsStack[S_SOLVED], wsStack[nRecDepth], S_BLOCK_SIZE_100);
				}
			}
		} else {
			if (iStatus == S_MULTIPLE) {
                memcpy(wsStack[nRecDepth - 1], wsStack[nRecDepth], S_BLOCK_SIZE_100);
			}
			return iStatus;
		}
	}
	if (nNumberThatSolves > 0) {
        memcpy(wsStack[nRecDepth - 1], wsStack[S_SOLVED], S_BLOCK_SIZE_100);
		return S_DONE;
	}
#ifdef TIMING
	QueryPerformanceCounter( &TE );
	printf("IN CELL elapsed (level %d): %f ms\n", 0, ((static_cast<double>( TE.QuadPart - TS.QuadPart )) * dInvFreq));
#endif

	return iStatus;
}

int FixAndGetNextIndex(SudokuWorkspace workspace, int &nextCellWs) {
	int iCell, iWsIdx;
	int iMinCount, iMinCell;
	int iMaxNumber, iThisNumber;
	int *iCountNumber;
	int iStatus;
	bool bEntryMade = true;

    iCountNumber = (int*)malloc(S_BLOCK_SIZE_TEN);
	while (bEntryMade) {
		bEntryMade = false;
		iMinCount = S_NINE;
		iMinCell = S_UNDEFINED;
		iMaxNumber = 0;
        memset(iCountNumber, 0, S_TEN);

		iCell = 0;
		while (iCell < S_81) {
            iWsIdx = WS_IDX[iCell];
			if (!IS_Entered(workspace[iWsIdx])) {
				if (BITCOUNT[workspace[iWsIdx]] <= 1) {
					if (BITCOUNT[workspace[iWsIdx]] == 0) {
						return S_CONTRADICTION;
					}
    				iThisNumber = REVERSE_BITMASKS[workspace[iWsIdx]];
					iStatus = MakeEntry(workspace, ROW_IDX[iWsIdx], COL_IDX[iWsIdx], iThisNumber);
					if (iStatus != S_OK) { return iStatus; }
	    			iCountNumber[iThisNumber] += 1;
		    		if (iCountNumber[iThisNumber] > iCountNumber[iMaxNumber]) {
			    		iMaxNumber = iThisNumber;
				    }
					bEntryMade = true;
				} else if (BITCOUNT[workspace[iWsIdx]] < iMinCount) {
					iMinCount = BITCOUNT[workspace[iWsIdx]];
					iMinCell = iCell;
				} else if ((BITCOUNT[workspace[iWsIdx]] == iMinCount) && 
						   (workspace[iWsIdx] & BITMASKS[iMaxNumber])) {
					iMinCell = iCell;
				}
			} else {
				iThisNumber = -workspace[iWsIdx];
				iCountNumber[iThisNumber] += 1;
				if (iCountNumber[iThisNumber] > iCountNumber[iMaxNumber]) {
					iMaxNumber = iThisNumber;
				}
			}
			iCell += 1;
		}
	}
    free(iCountNumber);
	if (iMinCell >= 0) {
        nextCellWs = WS_IDX[iMinCell];
	} else {
        return S_ERROR;
    }
	return S_OK;
}
