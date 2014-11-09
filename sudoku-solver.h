#include "sudoku-types.h"

void SolveSudoku(SudokuPuzzle sudoku, FILE* outFile);

int SolveByBacktracking(SudokuWorkspace workspace);

int TransferSudokuToWorkspace(SudokuPuzzle sudoku, SudokuWorkspace workspace);
void WriteOutSolutionFromWorkspace(FILE* outFile, SudokuWorkspace workspace, int nStatus);

SudokuPuzzle ReadAllSudokus(FILE* inFile, long& lSize);
SudokuPuzzle GetNextSudoku(SudokuPuzzle list, SudokuPuzzle current);
