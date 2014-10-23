#include "sudoku-types.h"

#define IS_OutOfBounds(N) ((N < 1) || (N > S_NINE))

/* Make an entry */
int MakeEntry(SudokuWorkspace workspace, int Row, int Column, int Number, bool bCleanUp = false);
int EliminateCandidate(SudokuWorkspace workspace, int Row, int Column, int Number, CellList oocList);
//int EliminatePencilmark(SudokuHelper helper, SquareHelper square, int Row, int Column, int Number);

/* Deterministic methods */
int DeterministicSolving(SudokuWorkspace workspace, CellList candlist = NULL);
int CountCandidatesInCell(int nCellStatus, bool bBreakAfterTwo = true);

int OnlyOneCandidateInGroup(SudokuWorkspace workspace, CellList oocList);
int MultipleSingleCandidatesInGroup(SudokuWorkspace workspace, int nOocInfo);

int CompletesRow(SudokuWorkspace workspace, int iToComplete, int iFreeCell);
int CompletesColumn(SudokuWorkspace workspace, int iToComplete, int iFreeCell);

int BacktrackInCell(SudokuWorkspace workspace, int Row, int Column);

/* Square coordinate arithmetic */
int inline getSquareIndex(int Row, int Column) { return 3 * ((Row - 1) / 3) + ((Column - 1) / 3) + 1; }

bool inline IS_Entered(int value) { return (value < 0); }