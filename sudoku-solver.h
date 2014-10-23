#include "sudoku-types.h"

//#include <deque>

//typedef std::deque<int[81]> SudokuPuzzleList;

void SolveSudoku(SudokuPuzzle sudoku, FILE* outFile);

int SolveByBacktracking(SudokuWorkspace workspace, CellList candlist = NULL);

int TransferSudokuToWorkspace(SudokuPuzzle sudoku, SudokuWorkspace workspace);
void WriteOutSolutionFromWorkspace(FILE* outFile, SudokuWorkspace workspace, int nStatus);

SudokuPuzzle ReadAllSudokus(FILE* inFile, long& lSize);
SudokuPuzzle GetNextSudoku(SudokuPuzzle list, SudokuPuzzle current);
