#include "stdafx.h"
#include "sudoku-solver.h"
#include "sudoku-logic.h"

//#include <Windows.h>
//#define TIMING 1

void SolveSudoku(SudokuPuzzle sudoku, FILE* outFile) {
	int iStatus;
	int workspace[S_100];
	CellList candlist = new int[S_81];

	InitWorkspace(workspace);
	candlist[0] = 0;
	iStatus = TransferSudokuToWorkspace(sudoku, workspace);
    /*
	if ((iStatus != S_CONTRADICTION) && (iStatus != S_EMPTY)) {
		iStatus = DeterministicSolving(workspace, candlist);
	}
	if ((iStatus == S_CONTRADICTION) || (iStatus == S_DONE) || (iStatus == S_EMPTY)) {
		WriteOutSolutionFromWorkspace(outFile, workspace, iStatus);
		return;
	}

#ifdef TIMING
	LARGE_INTEGER TS, TE, Freq;
	QueryPerformanceFrequency( &Freq );
	double dInvFreq = 1000.0 / static_cast<double>( Freq.QuadPart );
	QueryPerformanceCounter( &TS );
#endif
    */
	while (iStatus == S_OK) {
		iStatus = SolveByBacktracking(workspace, candlist);
		
		if (iStatus == S_OK) {
			candlist[0] = 0;
			iStatus = DeterministicSolving(workspace, candlist);
		}
	}/*
    */
#ifdef TIMING
	QueryPerformanceCounter( &TE );
	printf("BACKTRACK elapsed: %f ms\n", ((static_cast<double>( TE.QuadPart - TS.QuadPart )) * dInvFreq));
#endif

	WriteOutSolutionFromWorkspace(outFile, workspace, iStatus);
}

/* Backtracking */
int SolveByBacktracking(SudokuWorkspace workspace, CellList candlist /*= NULL*/) {
	int iStatus = S_OK;
	int iCand = 0;
	int iCell = S_CAND_TAKEN;

	if (candlist != NULL) {
		while (candlist[iCand] != 0) {
			if (candlist[iCand] == S_CAND_TAKEN) { iCand += 1; continue; }
			iCell = candlist[iCand];
			candlist[iCand] = S_CAND_TAKEN;
			if (! IS_Entered(workspace[iCell])) {
				iStatus = BacktrackInCell(workspace, iCell / 10, iCell % 10);
				if (iStatus != S_OK) { return iStatus; }
			}
			iCand += 1;
		}
		if (iCell != S_CAND_TAKEN) { 
			return iStatus; 
		}
	}

    return iStatus;
}

int TransferSudokuToWorkspace(SudokuPuzzle sudoku, SudokuWorkspace workspace) {
	int iStatus;
	int numFound = 0;
    int iRowIdx = 1;
    int iColIdx = 1;
    char* charIter = sudoku;

    while (iRowIdx <= S_NINE) {
        if (*charIter != S_CHAR_NULL) {
            numFound += 1;
            iStatus = MakeEntry(workspace, iRowIdx, iColIdx, (int)(*charIter - S_CHAR_NULL));
            if (iStatus != S_OK) { 
                return iStatus; 
            }
        }
        charIter += 2; /* skip comma (or \13 at end of line) */
        iColIdx += 1;
        if (iColIdx > S_NINE) {
            charIter += 1; /* skip \10 at end of line */
            iRowIdx += 1;
            iColIdx = 1;
        }
    }
	if (numFound == 0) { return S_EMPTY; }
	if (numFound < 17) { workspace[0] = S_MULTI; }
	return iStatus;
}

const char* sImpossible = "0,0,0,0,0,0,0,0,0\n0,0,0,0,0,0,0,0,0\n0,0,0,0,0,0,0,0,0\n0,0,0,0,0,0,0,0,0\n0,0,0,0,0,0,0,0,0\n0,0,0,0,0,0,0,0,0\n0,0,0,0,0,0,0,0,0\n0,0,0,0,0,0,0,0,0\n0,0,0,0,0,0,0,0,0\n\n";
const char* sSolvesEmpty = "-1,-2,-3,-4,-5,-6,-7,-8,-9\n-4,-5,-6,-7,-8,-9,-1,-2,-3\n-7,-8,-9,-1,-2,-3,-4,-5,-6\n-2,-3,-1,-5,-6,-4,-8,-9,-7\n-5,-6,-4,-8,-9,-7,-2,-1,-3\n-8,-9,-7,-2,-3,-1,-5,-6,-4\n-3,-1,-2,-6,-4,-5,-9,-7,-8\n-6,-4,-5,-9,-7,-8,-3,-1,-2\n-9,-8,-7,-3,-1,-2,-6,-4,-5\n\n";

void WriteOutSolutionFromWorkspace(FILE* outFile, SudokuWorkspace workspace, int nStatus) {
	bool bMulti = (nStatus == S_MULTIPLE);
	int nRowIdx = 0;

	if (nStatus == S_CONTRADICTION) {
		fputs(sImpossible, outFile);
		return;
	}

	if (nStatus == S_EMPTY) {
		fputs(sSolvesEmpty, outFile);
		return;
	}

	for (int row = 1; row <= S_NINE; row++) {
		nRowIdx += S_TEN;
		for (int col = 1; col <= S_NINE; col++) {
			if (col > 1) { fputc(S_CHAR_DEL, outFile); }
			if (bMulti) { fputc(S_CHAR_MINUS, outFile); }
			fputc(S_CHAR_NULL - workspace[nRowIdx + col], outFile);
		}
		fputc(S_CHAR_LINEBREAK, outFile);
	}
	fputc(S_CHAR_LINEBREAK, outFile);
}

SudokuPuzzle ReadAllSudokus(FILE* inFile, long &lSize) {
    if ( (inFile == NULL) || feof(inFile) ) return NULL;

    char* sBuffer;

    fseek(inFile, 0, SEEK_END);
    lSize = ftell(inFile);
    rewind(inFile);

    sBuffer = (SudokuPuzzle)malloc(sizeof(char) * (lSize + 1));
    if (sBuffer == NULL) return NULL;

    if (lSize != fread(sBuffer, 1, lSize, inFile)) {
        return NULL;
    }
    sBuffer[lSize] = '\0';

    return sBuffer;
}

const int SUDOKU_SIZE = sizeof(char) * (/* Sudoku numbers: */ 81 + 
                                        /* Separators: */ 81 + 
                                        /* add. linebreaks: */ 9);
SudokuPuzzle GetNextSudoku(SudokuPuzzle list, SudokuPuzzle current) {
    if (list == NULL) {
        return NULL;
    }
    if (current == NULL) {
        return list;
    }
    SudokuPuzzle newCurrent = current + SUDOKU_SIZE;
    if (*newCurrent == '\0') {
        return NULL;
    }
    return newCurrent + 2; /* skip blank line */
}

