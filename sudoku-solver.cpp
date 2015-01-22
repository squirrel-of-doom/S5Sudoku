#include "stdafx.h"
#include "sudoku-solver.h"
#include "sudoku-logic.h"

#ifdef OUTFILE
void SolveSudoku(SudokuWorkspace* wsStack, SudokuPuzzle sudoku, FILE* outFile) {
#else
void SolveSudoku(SudokuWorkspace* wsStack, SudokuPuzzle sudoku, char* &outIter) {
#endif
	int iStatus;
    size_t nRecDepth;
	int* workspace = wsStack[0];

	InitWorkspace(workspace);
	iStatus = TransferSudokuToWorkspace(sudoku, workspace);

    /*
	if ((iStatus == S_CONTRADICTION) || (iStatus == S_DONE) || (iStatus == S_EMPTY)) {
		WriteOutSolutionFromWorkspace(outIter, workspace, iStatus);
		return;
	}
    */

	while (iStatus == S_OK) {
        nRecDepth = 0;
		iStatus = SolveByBacktracking(wsStack, nRecDepth);
	}

#ifdef OUTFILE
	WriteOutSolutionFromWorkspace(outFile, workspace, iStatus);
#else
	WriteOutSolutionFromWorkspace(outIter, workspace, iStatus);
#endif
	return;
}

/* Backtracking */
int SolveByBacktracking(SudokuWorkspace* wsStack, size_t &nRecDepth) {
	int iStatus = S_OK;
	int iNextCell;
    
	while (iStatus == S_OK) {
		iStatus = FixAndGetNextIndex(wsStack[nRecDepth], iNextCell);
		if (iStatus != S_OK) { break; }
        nRecDepth++;
		iStatus = BacktrackInCell(wsStack, iNextCell, nRecDepth);
        nRecDepth--;
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

#ifdef OUTFILE
const char* sImpossible = "0,0,0,0,0,0,0,0,0\n0,0,0,0,0,0,0,0,0\n0,0,0,0,0,0,0,0,0\n0,0,0,0,0,0,0,0,0\n0,0,0,0,0,0,0,0,0\n0,0,0,0,0,0,0,0,0\n0,0,0,0,0,0,0,0,0\n0,0,0,0,0,0,0,0,0\n0,0,0,0,0,0,0,0,0\n\n";
const char* sSolvesEmpty = "-1,-2,-3,-4,-5,-6,-7,-8,-9\n-4,-5,-6,-7,-8,-9,-1,-2,-3\n-7,-8,-9,-1,-2,-3,-4,-5,-6\n-2,-3,-1,-5,-6,-4,-8,-9,-7\n-5,-6,-4,-8,-9,-7,-2,-3,-1\n-8,-9,-7,-2,-3,-1,-5,-6,-4\n-3,-1,-2,-6,-4,-5,-9,-7,-8\n-6,-4,-5,-9,-7,-8,-3,-1,-2\n-9,-7,-8,-3,-1,-2,-6,-4,-5\n\n";
#else
const char* sImpossible = "0,0,0,0,0,0,0,0,0\r\n0,0,0,0,0,0,0,0,0\r\n0,0,0,0,0,0,0,0,0\r\n0,0,0,0,0,0,0,0,0\r\n0,0,0,0,0,0,0,0,0\r\n0,0,0,0,0,0,0,0,0\r\n0,0,0,0,0,0,0,0,0\r\n0,0,0,0,0,0,0,0,0\r\n0,0,0,0,0,0,0,0,0\r\n\r\n";
const char* sSolvesEmpty = "-1,-2,-3,-4,-5,-6,-7,-8,-9\r\n-4,-5,-6,-7,-8,-9,-1,-2,-3\r\n-7,-8,-9,-1,-2,-3,-4,-5,-6\r\n-2,-3,-1,-5,-6,-4,-8,-9,-7\r\n-5,-6,-4,-8,-9,-7,-2,-3,-1\r\n-8,-9,-7,-2,-3,-1,-5,-6,-4\r\n-3,-1,-2,-6,-4,-5,-9,-7,-8\r\n-6,-4,-5,-9,-7,-8,-3,-1,-2\r\n-9,-7,-8,-3,-1,-2,-6,-4,-5\r\n\r\n";
#endif
const int SUDOKU_SIZE_IN = sizeof(char) * (/* Sudoku numbers: */ 81 + 
                                        /* Separators: */ 81 + 
                                        /* add. linebreaks: */ 9);
const int SUDOKU_SIZE_OUT = sizeof(char) * (/* Sudoku numbers: */ 81 + 
                                        /* Separators: */ 81 + 
                                        /* add. linebreaks: */ 0);
#ifdef OUTFILE
char sSudokuOut[SUDOKU_SIZE_OUT + S_81 + 2];
#endif


#ifdef OUTFILE
void WriteOutSolutionFromWorkspace(FILE* outFile, SudokuWorkspace workspace, int nStatus) {
	bool bMulti = (nStatus == S_MULTIPLE);
	int nRowIdx = 0;

	if (nStatus == S_CONTRADICTION) {
        //memcpy(sSudokuOut, sImpossible, SUDOKU_SIZE_OUT + 2); // +2 for blank line
        fwrite(sImpossible, 1, SUDOKU_SIZE_OUT + 1, outFile);
		return;
	}

	if (nStatus == S_EMPTY) {
        //memcpy(sSudokuOut, sSolvesEmpty, SUDOKU_SIZE_OUT + S_81 + 2); // minus signs, blank line
        fwrite(sSolvesEmpty, 1, SUDOKU_SIZE_OUT + S_81 + 1, outFile);
		return;
	}
	
    memset(sSudokuOut, 0, SUDOKU_SIZE_OUT);
    char* outIter = sSudokuOut;
	for (int row = 1; row <= S_NINE; row++) {
		nRowIdx += S_TEN;
		for (int col = 1; col <= S_NINE; col++) {
            if (col > 1) { *outIter = S_CHAR_DEL; outIter++; }
			if (bMulti) { *outIter = S_CHAR_MINUS; outIter++;  }
            *outIter = S_CHAR_NULL - workspace[nRowIdx + col]; 
            outIter++; 
		}
        //*outIter = S_CHAR_LINEBREAK_13; outIter++;
        *outIter = S_CHAR_LINEBREAK_10; outIter++;
	}
    //*outIter = S_CHAR_LINEBREAK_13; outIter++;
    *outIter = S_CHAR_LINEBREAK_10; outIter++;
    fwrite(sSudokuOut, 1, SUDOKU_SIZE_OUT + (bMulti ? S_81 + 1 : 1), outFile);
}
#else
void WriteOutSolutionFromWorkspace(char* &outIter, SudokuWorkspace workspace, int nStatus) {
	bool bMulti = (nStatus == S_MULTIPLE);
	int nRowIdx = 0;

	if (nStatus == S_CONTRADICTION) {
        memcpy(outIter, sImpossible, SUDOKU_SIZE_OUT + 2); // +2 for blank line
        outIter += SUDOKU_SIZE_OUT + 2;
		return;
	}

	if (nStatus == S_EMPTY) {
        memcpy(outIter, sSolvesEmpty, SUDOKU_SIZE_OUT + S_81 + 2); // minus signs, blank line
        outIter += SUDOKU_SIZE_OUT + S_81 + 2;
		return;
	}
	
	for (int row = 1; row <= S_NINE; row++) {
		nRowIdx += S_TEN;
		for (int col = 1; col <= S_NINE; col++) {
            if (col > 1) { *outIter = S_CHAR_DEL; outIter++; }
			if (bMulti) { *outIter = S_CHAR_MINUS; outIter++;  }
            *outIter = S_CHAR_NULL - workspace[nRowIdx + col]; 
            outIter++; 
		}
        *outIter = S_CHAR_LINEBREAK_13; outIter++;
        *outIter = S_CHAR_LINEBREAK_10; outIter++;
	}
    *outIter = S_CHAR_LINEBREAK_13; outIter++;
    *outIter = S_CHAR_LINEBREAK_10; outIter++;
}
#endif

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

SudokuPuzzle GetNextSudoku(SudokuPuzzle list, SudokuPuzzle current) {
    if (list == NULL) {
        return NULL;
    }
    if (current == NULL) {
        return list;
    }
    SudokuPuzzle newCurrent = current + SUDOKU_SIZE_IN;
    if (*newCurrent == '\0') {
        return NULL;
    }
    return newCurrent + 2; /* skip blank line */
}

