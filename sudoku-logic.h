#include "sudoku-types.h"

#define IS_OutOfBounds(N) ((N < 1) || (N > S_NINE))

/* Make an entry */
int MakeEntry(SudokuWorkspace workspace, int Row, int Column, int Number);
int EliminateCandidate(SudokuWorkspace workspace, int Row, int Column, int Number);

int BacktrackInCell(SudokuWorkspace workspace, int Row, int Column);
int FixAndGetNextIndex(SudokuWorkspace workspace, int &Row, int &Column);

/* Square coordinate arithmetic */
int inline getSquareIndex(int Row, int Column) { return 3 * ((Row - 1) / 3) + ((Column - 1) / 3) + 1; }

bool inline IS_Entered(int value) { return (value < 0); }