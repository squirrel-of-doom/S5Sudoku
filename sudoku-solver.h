#include "sudoku-types.h"

#ifdef OUTFILE
void SolveSudoku(SudokuWorkspace* wsStack, SudokuPuzzle sudoku, FILE* outFile);
#else
void SolveSudoku(SudokuWorkspace* wsStack, SudokuPuzzle sudoku, char* &outIter);
#endif

//int SolveByBacktracking(SudokuWorkspace workspace, size_t &nRecDepth);
int SolveByBacktracking(SudokuWorkspace* wsStack, size_t &nRecDepth);

int TransferSudokuToWorkspace(SudokuPuzzle sudoku, SudokuWorkspace workspace);
#ifdef OUTFILE
void WriteOutSolutionFromWorkspace(FILE* outFile, SudokuWorkspace workspace, int nStatus);
#else
void WriteOutSolutionFromWorkspace(char* &outFile, SudokuWorkspace workspace, int nStatus);
#endif

SudokuPuzzle ReadAllSudokus(FILE* inFile, long& lSize);
SudokuPuzzle GetNextSudoku(SudokuPuzzle list, SudokuPuzzle current);
