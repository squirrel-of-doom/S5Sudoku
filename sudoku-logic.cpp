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

int MakeEntry(SudokuWorkspace workspace, int Row, int Column, int Number, bool bCleanUp /*= false*/) {
	int iStatus;
	int oocList[20];
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
	//oocList[0] = 0;
	// Set entry
	workspace[nCellIdx] = -Number;
	// Eliminate field as fixed in 0-index
	workspace[nRowIdx] &= ~BITMASKS[Column];
	workspace[Column] &= ~BITMASKS[Row];

    if (S_OK != (iStatus = EliminateCandidate(workspace, Row, Column, Number, oocList))) { return iStatus; }

    /*
	if (oocList[0] != 0) {
		//iStatus = OnlyOneCandidateInGroup(workspace, oocList);
		if (iStatus != S_OK) { return iStatus; }
	}
    
	if (bCleanUp) {
		iStatus = DeterministicSolving(workspace);
	}*/
	return iStatus;
}

int EliminateCandidate(SudokuWorkspace workspace, int Row, int Column, int Number, CellList oocList) {
    int bmRowEntered, bmRowCands;
    int bmColEntered, bmColCands;
    int bmSquEntered, bmSquCands;
    int bmInverse = ~BITMASKS[Number];
    int iRowIdx = S_TEN * Row;
    int iColIdx = Column;
    int iSquIdx = (SQUARE_IDX[iRowIdx + iColIdx] - 1) * S_NINE;
    int iToken  = 0;

    while (iToken < S_NINE) {
        iRowIdx += 1;
        iColIdx += S_TEN;

        if (IS_Entered(workspace[iRowIdx])) {
            bmRowEntered &= BITMASKS[-workspace[iRowIdx]];
        } else {
            workspace[iRowIdx] &= bmInverse;
            bmRowCands |= workspace[iRowIdx];
        }

        if (IS_Entered(workspace[iColIdx])) {
            bmColEntered &= BITMASKS[-workspace[iColIdx]];
        } else {
            workspace[iColIdx] &= bmInverse;
            bmColCands |= workspace[iColIdx];
        }

        int nSquareIdx = S_TEN * SQUARE_COORDS_ROW[iSquIdx] + SQUARE_COORDS_COL[iSquIdx];
        if (IS_Entered(workspace[nSquareIdx])) {
            bmSquEntered &= BITMASKS[-workspace[nSquareIdx]];
        } else {
            workspace[nSquareIdx] &= bmInverse;
            bmSquCands |= workspace[nSquareIdx];
        }

        iSquIdx += 1;
        iToken += 1;
    }

    // check  ccontradictions found <-> cands
    // check only one cell empty
    // check done

}

int EliminateCandidate2(SudokuWorkspace workspace, int Row, int Column, int Number, CellList oocList) {
	int sqRow, sqCol;
	int iSquare;
	int iRemovedCandidates;
	int iFoundOnce = 0;
	int iFoundTwice = 0;
	int iFoundEntered = 0;
	int iFoundCands = 0;
	int nOoc = 0;
	int nSingles = 0;

	int nRowIdx = S_TEN * Row;
	iSquare = SQUARE_IDX[nRowIdx + Column];
	iRemovedCandidates = workspace[nRowIdx + Column] & ~BITMASKS[Number];

	iFoundEntered = BITMASKS[Number];
	for (int ii = 1; ii <= S_NINE; ii++) {
		if ((ii != Column) && (!IS_Entered(workspace[nRowIdx + ii]))) {
			workspace[nRowIdx + ii] &= ~BITMASKS[Number];
			iFoundCands |= workspace[nRowIdx + ii];
			if (workspace[nRowIdx + ii] == 0) {
				return S_CONTRADICTION;
			} else if ((workspace[nRowIdx + ii] & iRemovedCandidates) > 0) {
				if (iFoundOnce > 0) {
					iFoundTwice |= (workspace[nRowIdx + ii] & iRemovedCandidates) & iFoundOnce;
				}
				iFoundOnce |= (workspace[nRowIdx + ii] & iRemovedCandidates);
			}
		} else if (IS_Entered(workspace[nRowIdx + ii])) {
			iFoundEntered |= BITMASKS[-workspace[nRowIdx + ii]];
		}
	}
	if ((iFoundCands ^ iFoundEntered) != S_ALL_BITS_SET) {
		return S_CONTRADICTION;
	}
	if ((iFoundOnce ^ iFoundTwice) > 0) {
		nSingles = iFoundOnce ^ iFoundTwice;
		while ((oocList[nOoc] != 0) && (nOoc < 20)) { nOoc++; }
		oocList[nOoc] = nRowIdx;
		if ((nSingles & (nSingles - 1)) == 0) {
			oocList[nOoc] += REVERSE_BITMASKS[nSingles] * S_100;
		}
		oocList[++nOoc] = 0;
	}

	iFoundOnce = 0;
	iFoundTwice = 0;
	iFoundCands = 0;
	iFoundEntered = BITMASKS[Number];
	nRowIdx = 0;
	for (int ii = 1; ii <= S_NINE; ii++) {
		nRowIdx += S_TEN;
		if ((ii != Row) && (!IS_Entered(workspace[S_TEN * ii + Column]))){
			workspace[nRowIdx + Column] &= ~BITMASKS[Number];
			iFoundCands |= workspace[nRowIdx + Column];
			if (workspace[nRowIdx + Column] == 0) {
				return S_CONTRADICTION;
			} else if ((workspace[nRowIdx + Column] & iRemovedCandidates) > 0) {
				if (iFoundOnce > 0) {
					iFoundTwice |= (workspace[nRowIdx + Column] & iRemovedCandidates) & iFoundOnce;
				}
				iFoundOnce |= (workspace[nRowIdx + Column] & iRemovedCandidates);
			}
		} else if (IS_Entered(workspace[nRowIdx + Column])) {
			iFoundEntered |= BITMASKS[-workspace[nRowIdx + Column]];
		}
	}
	if ((iFoundCands ^ iFoundEntered) != S_ALL_BITS_SET) {
		return S_CONTRADICTION;
	}
	if ((iFoundOnce ^ iFoundTwice) > 0) {
		nSingles = iFoundOnce ^ iFoundTwice;
		while ((oocList[nOoc] != 0) && (nOoc < 20)) { nOoc++; }
		oocList[nOoc] = Column;
		if ((nSingles & (nSingles - 1)) == 0) {
			oocList[nOoc] += REVERSE_BITMASKS[nSingles] * S_100;
		}
		oocList[++nOoc] = 0;
	}

	iFoundOnce = 0;
	iFoundTwice = 0;
	iFoundCands = 0;
	iFoundEntered = BITMASKS[Number];
	nRowIdx = S_NINE * (iSquare - 1);
	int nSqIdx = 0;
	for (int ii = 1; ii <= S_NINE; ii++) {
		sqRow = SQUARE_COORDS_ROW[nRowIdx];
		sqCol = SQUARE_COORDS_COL[nRowIdx];
		nSqIdx = S_TEN * sqRow + sqCol;
		if (! IS_Entered(workspace[nSqIdx]) ) {
			workspace[nSqIdx] &= ~BITMASKS[Number];
			iFoundCands |= workspace[nSqIdx];
			if ((sqRow != Row) || (sqCol != Column)) {
				if (workspace[nSqIdx] == 0) {
					return S_CONTRADICTION;
				} else if ((workspace[nSqIdx] & iRemovedCandidates) > 0) {
					if (iFoundOnce > 0) {
						iFoundTwice |= (workspace[nSqIdx] & iRemovedCandidates) & iFoundOnce;
					}
					iFoundOnce |= (workspace[nSqIdx] & iRemovedCandidates);
				}
			}
		} else {
			iFoundEntered |= BITMASKS[-workspace[nSqIdx]];
		}
		nRowIdx++;
	}
	if ((iFoundCands ^ iFoundEntered) != S_ALL_BITS_SET) {
		return S_CONTRADICTION;
	}
	if ((iFoundOnce ^ iFoundTwice) > 0) {
		nSingles = iFoundOnce ^ iFoundTwice;
		while ((oocList[nOoc] != 0) && (nOoc < 20)) { nOoc++; }
		oocList[nOoc] = iSquare * 11; //iSquare * S_TEN + iSquare;
		if ((nSingles & (nSingles - 1)) == 0) {
			oocList[nOoc] += REVERSE_BITMASKS[nSingles] * S_100;
		}
		oocList[++nOoc] = 0;
	}
	return S_OK;
}

/*
int OnlyOneCandidateInGroup(SudokuWorkspace workspace, CellList oocList) {
	if (!oocList) { return S_ERROR; }

	int iStatus;
	int iList = 0;
	int nOocInfo;
	int group;
	int number;
	int bmNumber;

	while (oocList[iList] > 0) {
		nOocInfo = oocList[iList];
		number = nOocInfo / (S_100);
		nOocInfo %= S_100;

		if (number == 0) {
			iStatus = MultipleSingleCandidatesInGroup(workspace, nOocInfo);
			iList++;
			continue;
		}

		bmNumber = BITMASKS[number];
		iStatus = S_OK;

		if (((nOocInfo % 10) == 0) && (workspace[nOocInfo])) { // Is the group the row?
			group = nOocInfo / 10;
			for (int iCell = 1; iCell <= S_NINE; iCell++) {
				if (IS_Entered(workspace[S_TEN * group + iCell])) { continue; }
				if ((workspace[S_TEN * group + iCell] & bmNumber) == bmNumber) {
					iStatus = MakeEntry(workspace, group, iCell, number);
					break;
				}
			}
		} else if ((nOocInfo < S_TEN) && (workspace[nOocInfo])) { // Is the group the column?
			group = nOocInfo;
			for (int iCell = 1; iCell <= S_NINE; iCell++) {
				if (IS_Entered(workspace[S_TEN * iCell + group])) { continue; }
				if ((workspace[S_TEN * iCell + group] & bmNumber) == bmNumber) {
					iStatus = MakeEntry(workspace, iCell, group, number);
					break;
				}
			}
		} else if ((nOocInfo % 11) == 0) { // Is the group the square?
			group = nOocInfo / 11;
			int nSquareIdx = (group - 1) * S_NINE;
			for (int iCell = 1; iCell <= S_NINE; iCell++) {
                if (! IS_Entered(workspace[S_TEN * SQUARE_COORDS_ROW[nSquareIdx] + SQUARE_COORDS_COL[nSquareIdx]])) { 
					if ((workspace[S_TEN * SQUARE_COORDS_ROW[nSquareIdx] + SQUARE_COORDS_COL[nSquareIdx]] & bmNumber) == bmNumber) {
						iStatus = MakeEntry(workspace, SQUARE_COORDS_ROW[nSquareIdx], SQUARE_COORDS_COL[nSquareIdx], number);
						break;
					}
				}
				nSquareIdx++;
			}
		}
		if (iStatus != S_OK) { return iStatus; }
		iList++;
	}
	return S_OK;
}

int MultipleSingleCandidatesInGroup(SudokuWorkspace workspace, int nOocInfo) {
	int iStatus = S_OK;

	int nGroup, nGroupType;
	int sqRow, sqCol;
	int nCellStatus;
	int positions[S_TEN];
	int iCount;

	memset(positions, 0, S_TEN * sizeof(int));
	if ((nOocInfo % 10) == 0) { // Row has single candidates
		if (workspace[nOocInfo] == 0) { return S_OK; }
		nGroupType = S_ROW;
		nGroup = nOocInfo / 10;
	} else if (nOocInfo < S_TEN) { // Column has single candidates
		if (workspace[nOocInfo] == 0) { return S_OK; }
		nGroupType = S_COL;
		nGroup = nOocInfo;
	} else if ((nOocInfo % 11) == 0) { // Square has single candidates
		nGroupType = S_SQU;
		nGroup = nOocInfo / 11;
	} else {
		return S_ERROR;
	}

	for (int cell = 1; cell <= S_NINE; cell++) {
		switch (nGroupType) {
		case S_ROW:
			nCellStatus = workspace[S_TEN * nGroup + cell]; break;
		case S_COL:
			nCellStatus = workspace[S_TEN * cell + nGroup]; break;
		case S_SQU:
			sqRow = SQUARE_COORDS_ROW[(nGroup - 1) * S_NINE + cell - 1];
			sqCol = SQUARE_COORDS_COL[(nGroup - 1) * S_NINE + cell - 1];
			nCellStatus = workspace[S_TEN * sqRow + sqCol]; break;
		}
		if (IS_Entered(nCellStatus)) { 
			positions[-nCellStatus] = S_SKIP;
		} else {
			iCount = 1;
			while (nCellStatus > 0) {
				if (nCellStatus % 2) { 
					if (positions[iCount] > 0) {
						positions[iCount] = S_SKIP;
					} else if (positions[iCount] == 0) {
						positions[iCount] = cell;
					}
				}
				nCellStatus >>= 1;
				iCount++;
			}
		}
	}
	iCount = 1;
	while (iCount <= S_NINE) {
		if (positions[iCount] > 0) {
			switch (nGroupType) {
			case S_ROW:
				iStatus = MakeEntry(workspace, nGroup, positions[iCount], iCount); break;
			case S_COL:
				iStatus = MakeEntry(workspace, positions[iCount], nGroup, iCount); break;
			case S_SQU:
				sqRow = SQUARE_COORDS_ROW[(nGroup - 1) * S_NINE + positions[iCount] - 1];
				sqCol = SQUARE_COORDS_COL[(nGroup - 1) * S_NINE + positions[iCount] - 1];
				iStatus = MakeEntry(workspace, sqRow, sqCol, iCount); break;
			}
			if (iStatus != S_OK) { return iStatus; }
		}
		iCount++;
	}
	return iStatus;
}
*/
/* Deterministic solving */



int DeterministicSolving(SudokuWorkspace workspace, CellList candlist /*= NULL*/) {
	int iStatus = S_OK;
	bool bEntryMade;
	bool bDone = true;
	bool bFillCandList = false;
	int  nCandList = 0;
	int  nHailMary = 0;
	int  nMinCount = S_NINE;

	int  nGroupStatus;
	int  nSquareNums;
	int  nSquareCands;
	int  nCellStatus;
	int  nRowIdx;
	int  nSquareIdx;
	int  nSquareOnce, nSquareTwice;

	int squareEntered[9];

    return S_OK;

#ifdef TIMING
	LARGE_INTEGER TS, TE, Freq;
	QueryPerformanceFrequency( &Freq );
	double dInvFreq = 1000.0 / static_cast<double>( Freq.QuadPart );
	QueryPerformanceCounter( &TS );
#endif

	bFillCandList = ((candlist != NULL) && (candlist[0] == 0));

	do {
		bDone = true;
		bEntryMade = false;
		nRowIdx = S_TEN;
		for (int group = 1; group <= S_NINE; group++) {
			if ((nGroupStatus = workspace[nRowIdx]) > 0) {
				bDone = false;
				// Is there a row lacking only one entry?
				if (0 == (nGroupStatus & (nGroupStatus - 1))) {
					iStatus = MakeEntry(workspace, group, REVERSE_BITMASKS[nGroupStatus], CompletesRow(workspace, group, REVERSE_BITMASKS[nGroupStatus]));
					if (iStatus != S_OK) { return iStatus; }
					bEntryMade = true;
				}
			}
			nRowIdx += S_TEN;

			// Is there a column lacking only one entry?
			if ((nGroupStatus = workspace[group]) > 0) {
				if (0 == (nGroupStatus & (nGroupStatus - 1))) {
					iStatus = MakeEntry(workspace, REVERSE_BITMASKS[nGroupStatus], group, CompletesColumn(workspace, group, REVERSE_BITMASKS[nGroupStatus]));
					if (iStatus != S_OK) { return iStatus; }
					bEntryMade = true;
				}
			}
		}

		if (bDone) { break; }
		if (bEntryMade) { continue; }

		if (bFillCandList) { 
			 // Remake candidate list with each iteration
			candlist[0] = 0; 
			nCandList = 0;
			nHailMary = 0;
			nMinCount = S_NINE;
		}

		nRowIdx = 0;
		for (int group = 1; group <= S_NINE; group++) {
			nGroupStatus = S_ALL_BITS_SET;
			nSquareNums = S_ALL_BITS_SET;
			nSquareCands = 0;
			nSquareTwice = 0;

			// Collection stage:
			for (int cell = 1; cell <= S_NINE; cell++) {
				//  - Check: Single candidate
				nCellStatus = workspace[group * S_TEN + cell];
				if (nCellStatus == 0) {
					return S_CONTRADICTION;
				} else if (IS_Entered(nCellStatus)) {
					// NOP
				} else if (0 == (nCellStatus & (nCellStatus - 1))) {
					iStatus = MakeEntry(workspace, group, cell, REVERSE_BITMASKS[nCellStatus]);
					if (iStatus != S_OK) { return iStatus; }
					bEntryMade = true;
				} else {
					if (bFillCandList && (CountCandidatesInCell(nCellStatus) == 2)) {
						candlist[nCandList] = group * S_TEN + cell;
						candlist[nCandList + 1] = 0;
						nCandList += 1;
					} else if (bFillCandList && (candlist[0] == 0)) {
						int iCount = CountCandidatesInCell(nCellStatus, false);
						if (iCount < nMinCount) {
							nMinCount = iCount;
							nHailMary = group * S_TEN + cell;
						}
					}
				}
				//  - Status and missing Number for square
				nSquareIdx = nRowIdx + cell - 1;
				nCellStatus = workspace[SQUARE_COORDS_ROW[nSquareIdx] * S_TEN + SQUARE_COORDS_COL[nSquareIdx]];
				if (IS_Entered(nCellStatus)) {
					nGroupStatus ^= BITMASKS[cell];
					nSquareNums ^= BITMASKS[-nCellStatus];
					nSquareCands |= BITMASKS[-nCellStatus];
					nSquareTwice |= BITMASKS[-nCellStatus];
				} else {
					nSquareTwice |= nCellStatus & nSquareCands;
					nSquareCands |= nCellStatus;
				}
			}
			squareEntered[group - 1] = nGroupStatus;
			if (nSquareCands != S_ALL_BITS_SET) {
				return S_CONTRADICTION;
			}

			// Is there a square with only one entry?
			if ((nGroupStatus > 0) && (0 == (nGroupStatus & (nGroupStatus - 1)))) {
				nSquareIdx = nRowIdx + REVERSE_BITMASKS[nGroupStatus] - 1;
				iStatus = MakeEntry(workspace, SQUARE_COORDS_ROW[nSquareIdx], SQUARE_COORDS_COL[nSquareIdx], REVERSE_BITMASKS[nSquareNums]);
				if (iStatus != S_OK) { return iStatus; }
				squareEntered[group - 1] = 0;
				bEntryMade = true;
			}
			if ((nSquareCands ^ nSquareTwice) > 0) {
				nSquareOnce = nSquareCands ^ nSquareTwice;
				if ((nSquareOnce & (nSquareOnce - 1)) != 0) {
					int iCellCount = 0;
					for (int iCell = 0; iCell < S_NINE; iCell++) {
						nCellStatus = workspace[SQUARE_COORDS_ROW[nRowIdx + iCell] * S_TEN + SQUARE_COORDS_COL[nRowIdx + iCell]];
						if (IS_Entered(nCellStatus)) { continue; }
						if ((nCellStatus & nSquareOnce) > 0) { iCellCount++; }
					}
					if (iCellCount == 1) { 
						return S_CONTRADICTION; 
					}
				}
			}
			nRowIdx += S_NINE;
		}

		//if (bEntryMade) { continue; }

	} while (bEntryMade);

	if (!bDone && bFillCandList && (candlist[0] == 0)) {
		candlist[0] = nHailMary;
		candlist[1] = 0;
	}

#ifdef TIMING
	QueryPerformanceCounter( &TE );
	printf("DETERMINISTIC elapsed: %f ms\n", ((static_cast<double>( TE.QuadPart - TS.QuadPart )) * dInvFreq));
#endif

	return bDone ? S_DONE : S_OK;
}

int CountCandidatesInCell(int nCellStatus, bool bBreakAfterTwo /*= true*/) {
    return BITCOUNT[nCellStatus];
	int iCandLoop = S_NINE;
	int nCands = 0;
	while ((nCellStatus > 0) && (iCandLoop > 0)){
		// :POT: USE SHIFTS?
		if ((nCellStatus & BITMASKS[iCandLoop]) == BITMASKS[iCandLoop]) {
			nCands++;
			if (nCands > 2) { 
				if (bBreakAfterTwo) { break; }
			}
		}
		iCandLoop--;
	}
	return nCands;
}

/*

int CompletesRow(SudokuWorkspace workspace, int iToComplete, int iFreeCell) {
	int nPossible = S_ALL_BITS_SET;
	for (int cell = 1; cell <= S_NINE; cell++) {
		if (cell != iFreeCell) {
			nPossible ^= BITMASKS[-workspace[S_TEN * iToComplete + cell]];
		}
	}
	if (0 != (nPossible & (nPossible - 1))) { return S_CONTRADICTION; }
	return REVERSE_BITMASKS[nPossible];
}

int CompletesColumn(SudokuWorkspace workspace, int iToComplete, int iFreeCell) {
	int nPossible = S_ALL_BITS_SET;
	for (int cell = 1; cell <= S_NINE; cell++) {
		if (cell != iFreeCell) {
			nPossible ^= BITMASKS[-workspace[S_TEN * cell + iToComplete]];
		}
	}
	if (0 != (nPossible & (nPossible - 1))) { return S_CONTRADICTION; }
	return REVERSE_BITMASKS[nPossible];
}

*/



int BacktrackInCell(SudokuWorkspace workspace, int Row, int Column) {
	int iStatus = S_OK;
	int nCellCands;
	int wsTemp[S_100];
	int newCands[S_100];
	int iNumberThatSolves = 0;
	int wsSolved[S_100];

	//if (IS_OutOfBounds(Row)) { return S_OUTOFBOUNDS; }
	//if (IS_OutOfBounds(Column)) { return S_OUTOFBOUNDS; }

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
		newCands[0] = 0;
 		iStatus = MakeEntry(wsTemp, Row, Column, num, false);
		if (iStatus == S_OK) { 
			iStatus = DeterministicSolving(wsTemp, newCands);
		}

		if (iStatus == S_OK) {
			iStatus = SolveByBacktracking(wsTemp, newCands);
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
