#include "sudoku-types.h"

#define IS_OutOfBounds(N) ((N < 1) || (N > S_NINE))

/* Make an entry */
int MakeEntry(SudokuWorkspace workspace, int Row, int Column, int Number);
int EliminateCandidate(SudokuWorkspace workspace, int Row, int Column, int Number);

int BacktrackInCell(SudokuWorkspace* wsStack, int CellWs, size_t &nRecDepth);
int FixAllSingleCandidates(SudokuWorkspace workspace);
int FixAndGetNextIndex(SudokuWorkspace workspace, int &nextCellWs);

bool inline IS_Entered(int value) { return (value < 0); }