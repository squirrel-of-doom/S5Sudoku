// S5Sudoku.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"

#include <iostream>
#include <string>

#include "constants.h"
#include "sudoku-types.h"
#include "sudoku-solver.h"

#include <Windows.h>
//#define TIMING 1
//#define TIMING_SINGLE 1

int RunProgram(char* sInFile = NULL) {
	FILE* inFile;
	FILE* outFile;
    SudokuPuzzle sudokuList;
    SudokuPuzzle sudokuIter;
    char* outBuffer;
    char* outIter;
    SudokuWorkspace wsStack[S_81];
    long lSize = 0;
	int num = 0;

	InitWorkspaceTemplate();
    InitWorkspaceStack(wsStack);
	if (sInFile == NULL) {
		fopen_s(&inFile, "IN.TXT", "rb");
	} else {
		fopen_s(&inFile, sInFile, "rb");
	}
    if (inFile == NULL) { return S_ERROR; }
    sudokuList = ReadAllSudokus(inFile, lSize);
	fclose(inFile);

    //outBuffer = (char*)malloc(floor(1.5 * lSize) * sizeof(char));
    if (true) { //outBuffer) {
#ifdef OUTFILE
        fopen_s(&outFile, "OUT.TXT", "w");
        setvbuf(outFile, NULL, _IOFBF, 1024);
#else
        outIter = outBuffer;
#endif
        sudokuIter = sudokuList;
#ifdef TIMING_SINGLE
	LARGE_INTEGER TS, TE, Freq;
	QueryPerformanceFrequency( &Freq );
    double dInvFreq = 1000.0 / static_cast<double>( Freq.QuadPart );
    size_t num = 0;
#endif
        while (sudokuIter != NULL) {
#ifdef TIMING_SINGLE
            num++;
	        QueryPerformanceCounter( &TS );
#endif
#ifdef OUTFILE
            SolveSudoku(wsStack, sudokuIter, outFile);
#else
            SolveSudoku(wsStack, sudokuIter, outIter);
#endif
#ifdef TIMING_SINGLE
	        QueryPerformanceCounter( &TE );
            double dTime = ((static_cast<double>( TE.QuadPart - TS.QuadPart )) * dInvFreq);
	        if (dTime > 1.0) printf("%d elapsed: %f ms\n", num, dTime);
#endif
            sudokuIter = GetNextSudoku(sudokuList, sudokuIter);
        }
#ifndef OUTFILE
        fopen_s(&outFile, "OUT.TXT", "wb");
        fwrite(outBuffer, sizeof(char), outIter - outBuffer, outFile);
#endif
        fclose(outFile);

        if (sudokuList) { free(sudokuList); }
        //free(outBuffer);
    }
    FreeWorkspaceStack(wsStack);

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
