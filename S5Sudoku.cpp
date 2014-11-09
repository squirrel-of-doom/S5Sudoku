// S5Sudoku.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"

#include <iostream>
#include <string>

#include "constants.h"
#include "sudoku-types.h"
#include "sudoku-solver.h"

#include <Windows.h>
#define TIMING 1

int RunProgram(char* sInFile = NULL) {
	FILE* inFile;
	FILE* outFile;
    SudokuPuzzle sudokuList;
    SudokuPuzzle sudokuIter;
    long lSize = 0;
    char* outBuffer;
	int num = 0;

	InitWorkspaceTemplate();
	if (sInFile == NULL) {
		fopen_s(&inFile, "IN.TXT", "rb");
	} else {
		fopen_s(&inFile, sInFile, "rb");
	}
    sudokuList = ReadAllSudokus(inFile, lSize);
	fclose(inFile);

    outBuffer = (char*)malloc(floor(1.5 * lSize) * sizeof(char));
    if (outBuffer) {
        fopen_s(&outFile, "OUT.TXT", "w");
        setvbuf(outFile, outBuffer, _IOFBF, 65536);

#ifdef TIMING
		LARGE_INTEGER TS, TE, Freq;
		QueryPerformanceFrequency( &Freq );
		double dInvFreq = 1000.0 / static_cast<double>( Freq.QuadPart );
#endif
        sudokuIter = sudokuList;
        while (sudokuIter != NULL) {
            //printf("%d ", num++);
#ifdef TIMING
			QueryPerformanceCounter( &TS );
#endif
            SolveSudoku(sudokuIter, outFile);
#ifdef TIMING
			QueryPerformanceCounter( &TE );
			printf("%d\t%f\n", num++, ((static_cast<double>( TE.QuadPart - TS.QuadPart )) * dInvFreq));
#endif
            sudokuIter = GetNextSudoku(sudokuList, sudokuIter);
        }

        //fopen_s(&outFile, "OUT.TXT", "w");
        //fputs(outBuffer, outFile);
        fclose(outFile);

        if (sudokuList) { free(sudokuList); }
        free(outBuffer);
    }

	return S_OK;
}

int main(int argc, char* argv[])
{
#ifdef TIMING
	LARGE_INTEGER TS, TE, Freq;
	QueryPerformanceFrequency( &Freq );
	double dInvFreq = 1000.0 / static_cast<double>( Freq.QuadPart );
	QueryPerformanceCounter( &TS );
#endif

	if (argc > 1) {
		RunProgram(argv[1]);
	} else {
		RunProgram();
	}

#ifdef TIMING
	QueryPerformanceCounter( &TE );
	printf("TOTAL elapsed: %f ms\n", ((static_cast<double>( TE.QuadPart - TS.QuadPart )) * dInvFreq));
#endif

	return 0;
}
