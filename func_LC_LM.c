/*
func_LC_LM.c

****LinearConflict****
Two tile are conflict with each other if they are in their goal line but wrong
ordered. Generally, value of linear conflict can be counted using matrix with
size n * n, where n is the length of each line. (Of course there are much easier
way to do it for 15-puzzle, but it won't be able to suit 24 or other puzzle).
Here, I use 0 in matrix to represents two tiles are not conflict and 1 means
conflict.

For example, in line 2 (4 5 6 7), we could possiblely have order 7 5 6 4, which
is counted as +4 linaer conflict, could be represented in matrix:
  7 5 6 4
7 0 1 1 1
5   0 0 1
6     0 1
4       0

We can then get the value of linear conflict from either the number of row after
column reduction, or the number of column after row reduction, here reduction
means eliminating redundant conflict that already counted by another. To obtain
admissibility, we have to choose the smaller one. More intuitively, the value of
conflict is 2 times the smallest number of circles to include all 1s in matrix,
each circle can only in one line and can't contain any 0.

Since this matrix is always symmetric and has zero diagonal entries. The matrix
could be transformed as a n-1 array, where each number in array is the decimal
value of each row (viewed them as binary number) in matrix.
For instance, the matrix above can be transformed as: 7 (0111), 1 (0001), 1

Similarily, we can still obtain the projection said above from array, column
reduction can be done during generating the array, the result value would be the
number of distinct digit (power of 2) which has been added to some element.
Also, row reduction is done by eliminating continuous 1, for instace,
7 can be reduced to 1 since it contains all 1s, and 5 can't be reduced since
there is a 0 between two 1s. If a number smaller than 2^(i+1) but not smaller
than 2^(i) plus i, then it can be reduced to some smaller number, change the
reduced number to 0. After reduction, count the number of digit greater than 1
left.

****Last Move****
If 1 is not in the left-most column or column conflict with some tiles, and 4
is not in the top row or row conflict with other tiles, two additional move
could be added to Manhattan distance and still keep admissibility.

Created by Xiuge Chen (xiugec@student.unimelb.edu.au) 14/10/2018
*/

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "func_LC_LM.h"

/* Last Moves Heuristic (Last one move) used in compute linear conflict
  heuristic */
int func_LM (int* state) {
    int i, j;
    int lm = 0;
    int max = 0;
    int currentTile = 0;

  /* for first two row, if 4 is in the first row or, in the second row and
  conflicts with some other tiles, the last move should not add 2 to the LC */
  for (i = 0; i < 2; i++) {
    for (j = 0; j < STATES_PERLINE; j++) {
      currentTile = state[i * STATES_PERLINE + j];

      // for the first row, only check if 4 exist
      if (!i) {
        if (currentTile == 4)
          return lm;

        continue;
      }

      // for the second row, check whether 4 conflict with other tile
      if (currentTile && (currentTile / STATES_PERLINE == i)) {
        if (currentTile > max)
          max = currentTile;

        else if (currentTile == 4)
          return lm;
      }
    }
  }

  max = 0;

  /* for first two column, if 1 is in the first col or, in the second col and
  conflicts with some other tiles, the last move should not add 2 to the LC */
  for (j = 0; j < STATES_PERLINE; j++) {
	   for (i = 0; i < STATES_PERLINE; i++) {
		    currentTile = state[j + i * STATES_PERLINE];

        // for the first col, only check if 1 exist
        if (!j) {
          if (currentTile == 1)
            return lm;

          continue;
        }

        // for the second row, check whether 1 conflict with other tile
        if (currentTile && (currentTile % STATES_PERLINE == j)) {
          if (currentTile > max)
            max = currentTile;

          else if (currentTile == 1)
            return lm;
      }
    }
  }

  lm += 1;

  return lm;
}

/* helper function to compute linear conflict heuristic of each line
  0 is not belong to any line */
int lc_Line (int* line) {

  int matrix[STATES_PERLINE - 1] = {0, 0, 0};
  // Variables for counting number of rows/column after reduction
  bool newRow = false;
  bool newCol = false;
  int lcRow = 0;
  int lcCol = 0;
  // previous tile
  int pre = 0;
  int i, j;

  // get the matrix
  for (i = 1; i < STATES_PERLINE; i++) {
    newRow = true;

      for (j = i - 1; j >= 0; j--) {
        if (line[i] && line[i] < line[j]) {
          matrix[j] += pow(2, STATES_PERLINE - 1 - i);

        // count the number of row after column reduction
        if (newRow) {
          lcRow++;
          newRow = false;
        }
      }
    }
  }

  // count the number of column after row reduction
  for (i = 0; i < STATES_PERLINE - 1; i++) {
    if (matrix[i]) {

      // reduction on row, eliminating all consecutive 1s, every reduction
      // counted as one lc move
      for (j = 1; j < STATES_PERLINE - 1; j++) {
        if (matrix[i] < pow(2, j + 1) && matrix[i] >= pow(2, j) + j) {
          lcCol++;
          pre = 0;
          newCol = true;
        }
      }

      if (newCol){
        newCol = false;
        continue;
      }

      // if can't do reduction, try column reduction (comparing to previous tile)
      if (matrix[i] != pre){
        lcCol++;
        pre = matrix[i];
      }
    }
  }

  // return the smaller count
  return (lcRow < lcCol ? lcRow : lcCol);
}

/* linear conflict heuristic considered interaction with Last Moves Heuristic
  (Last one moves) used in manhattan distance */
int func_LC_LM (int* state) {
    int currentTile = -1;
    int line[STATES_PERLINE] = {0, 0, 0, 0};
    int i, j;
	/* the number of tiles that must be removed from line (could be row or column)
	 to resolve the linear conflicts, initial it by Last Move Heuristic, and
    consider the interaction between them while computing LC */
	int lc = func_LM(state);

	/* for each row */
	for (i = 0; i < STATES_PERLINE; i++) {

		// store each element in array
		for (j = 0; j < STATES_PERLINE; j++) {
            currentTile = state[i * STATES_PERLINE + j];

            // if the cell contains not 0 and should be at the same line
            if (currentTile && (currentTile / STATES_PERLINE == i))
                line[j] = currentTile;

            else
                line[j] = 0;
        }

        lc += lc_Line(line);
    }

	/* for each column */
	for (j = 0; j < STATES_PERLINE; j++) {

        // store each element in array
		for (i = 0; i < STATES_PERLINE; i++) {
			currentTile = state[j + i * STATES_PERLINE];

            // if the cell contains not 0 and should be at the same line
            if (currentTile && (currentTile % STATES_PERLINE == j))
                line[i] = currentTile;

            else
                line[i] = 0;
        }

        lc += lc_Line(line);
	}

	return lc * 2;
}
