/************************************************************
*
* Structures defined here: 
* * SudokuPuzzle is a simple 9x9 sudoku
* * SquareCoords is a look-up for square coordinates
* * SudokuWorkspace is the workspace for the solver
*
************************************************************/

#include "constants.h"

#include <vector>

typedef int* CellList;

/* Sudoku simple structure */

typedef char* SudokuPuzzle;
//typedef std::vector<int> SudokuPuzzle;

/* Square helper */

typedef int** SquareCoords;

//SquareCoords InitSquareCoords();
//void FreeSquareCoords(SquareCoords square);

/* Sudoku workspace */

typedef int* SudokuWorkspace;

void InitWorkspaceStack(SudokuWorkspace* wsStack);
void InitWorkspace(SudokuWorkspace workspace);
void InitWorkspaceTemplate();

void FreeWorkspaceStack(SudokuWorkspace* wsStack);