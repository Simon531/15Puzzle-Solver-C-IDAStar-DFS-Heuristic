/*
func_LC_LM.h
Created by Xiuge Chen (xiugec@student.unimelb.edu.au) 11/10/2018
*/

#define NUM_STATES 16
#define STATES_PERLINE 4

#define TRUE 1
#define FALSE 0

/* Last Moves Heuristic (Last one move) used in compute
  linear conflict heuristic. */
int func_LM (int* state);

/* helper function to compute linear conflict heuristic of each line
  0 is not belong to any line */
int lc_Line (int* line);

/* linear conflict heuristic considered interaction with Last Moves Heuristic
  (Last two moves) used in manhattan distance */
int func_LC_LM (int* state);
