/***********************************
* Constants for the Sudoku-Solver
***********************************/

/* Output mode */
#define OUTFILE 1

/* Sudoku dimension */
#define S_NINE    9
#define S_TEN    10
#define S_81     81
#define S_100   100

/* Return values */
#define S_OK			  0
#define S_ERROR			 -1
#define S_IO_ERROR		-11
#define S_CONTRADICTION -31
#define S_DONE			-32
#define S_MULTIPLE		-33
#define S_EMPTY			-34

#define S_ALL_BITS_SET  511
#define S_CAND_TAKEN    -99
#define S_UNDEFINED      -1
#define S_MARKER          1
#define S_SOLVED         80

#define S_UNIQUE          1
#define S_UNSOLVABLE      0
#define S_MULTI          -1

/* Dimension indices for cells */
#define S_ROW 0
#define S_COL 1
#define S_SQU 2

#define S_CHAR_DEL          ','
#define S_CHAR_MINUS        '-'
#define S_CHAR_LINEBREAK    '\n'
#define S_CHAR_LINEBREAK_13    13
#define S_CHAR_LINEBREAK_10    10
#define S_CHAR_NULL          48
#define S_CHAR_ONE           49
