/*
puzzle.c

a solver for the 15–puzzle, take a sequence of states,
returns the optimal number of operation needed to get the puzzle in form:
B  1  2  3
4  5  6  7
8  9  10 11
12 13 14 15
Where B means the blank space.

Use Iterative Deepening A* (IDA*) algorithm which follows the Depth-first
search strategy. The heuristic be using to prune the search space is the sum
of Manhattan distances, Linear Conflict and Last Move Heuristic.

Most work are based on "Finding Optimal Solutions to the Twenty-Four Puzzle" by
Richard E. Korf and Larry A. Taylor
http://www.aaai.org/Papers/AAAI/1996/AAAI96-178.pdf

input must be solveable!!!

Input and Output handling are created by Grady Fitzpatrick (grady.fitzpatrick@unimelb.edu.au)
Soving Algorithm created by Xiuge Chen (xiugec@student.unimelb.edu.au)
11/10/2018
*/

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>

#include "func_LC_LM.h"

/**
 * READ THIS DESCRIPTION
 *
 * node data structure: containing state, g, f, a (the move executed to
 		reach this node)
 * you can extend it with more information if needed
 */
typedef struct node{
	int state[16];
	int g;
	int f;
	int a;
} node;

/**
 * Global Variables
 */

// used to track the position of the blank in a state,
// so it doesn't have to be searched every time we check if an operator is applicable
// When we apply an operator, blank_pos is updated
int blank_pos;

// Initial node of the problem
node initial_node;

// Statistics about the number of generated and expendad nodes
unsigned long generated;
unsigned long expanded;

/**
 * The id of the four available actions for moving the blank (empty slot). e.x.
 * Left: moves the blank to the left, etc.
 */
#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3

/*
 * Helper arrays for the applicable function
 * applicability of operators: 0 = left, 1 = right, 2 = up, 3 = down
 */
int ap_opLeft[]  = { 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1 };
int ap_opRight[]  = { 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0 };
int ap_opUp[]  = { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
int ap_opDown[]  = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0 };
int *ap_ops[] = { ap_opLeft, ap_opRight, ap_opUp, ap_opDown };

/* print state */
void print_state( int* s )
{
	int i;

	for( i = 0; i < NUM_STATES; i++ )
		printf( "%2d%c", s[i], ((i+1) % STATES_PERLINE == 0 ? '\n' : ' ') );
}

void printf_comma (long unsigned int n) {
    if (n < 0) {
        printf ("-");
        printf_comma (-n);
        return;
    }
    if (n < 1000) {
        printf ("%lu", n);
        return;
    }
    printf_comma (n/1000);
    printf (",%03lu", n%1000);
}

/* return the sum of manhattan distances from state to goal */
int manhattan( int* state )
{
	int sum = 0;
	int i;

	 for (i = 0; i < NUM_STATES; i++) {
		 if (state[i]) {
			 sum += abs(state[i] / STATES_PERLINE - i / STATES_PERLINE) +
				abs(state[i] % STATES_PERLINE - i % STATES_PERLINE);
		 }
	 }

	 /* OPTIMIZATION, comment it if not use */
	 if (sum)
	 	sum += func_LC_LM(state);

	return( sum );
}

/* return 1 if op is applicable in state, otherwise return 0 */
int applicable( int op )
{
	return( ap_ops[op][blank_pos] );
}

/* apply operator */
void apply( node* n, int op )
{
	int t;

	//find tile that has to be moved given the op and blank_pos
	t = blank_pos + (op == 0 ? -1 : (op == 1 ? 1 :
		(op == 2 ? -STATES_PERLINE : STATES_PERLINE)));

	//apply op
	n->state[blank_pos] = n->state[t];
	n->state[t] = 0;

	//update blank pos
	blank_pos = t;
}

/* reverse to previous state before taking action a */
void reverse(node* node, int a) {
	apply(node, (a % 2 == 0) ? a + 1 : a - 1);
}

/* Recursive IDA */
node* ida(node* node, int threshold, int* newThreshold)
{
	struct node* r = NULL;
 	int a;
	//last move from original node to current node
	int aToNode = node->a;

	// check if reach end before moving
	if (manhattan(node->state) == 0)
		return node;

	//for any action applicable in given state node->s
	for (a = 0; a < 4; a++) {
		//when the move is applicable and doesn't go back to previous state
		if (applicable(a) && (a + node->a) % 4 != 1) {
			apply(node, a);
			node->a = a;
			node->g++;
			node->f = node->g + manhattan(node->state);
			generated++;

			// pruning the node
			if (node->f > threshold) {
				*newThreshold = (*newThreshold < node->f ? *newThreshold : node->f);

				// apply the oppsite movement of a to get back to parent node
				reverse(node, a);
				node->a = aToNode;
				node->g--;
			}
			else {
				// if reach the goal node, return
				if (manhattan(node->state) == 0)
					return node;

				// keep recursively generating
				expanded++;
				r = ida(node, threshold, newThreshold);

			 	if (r)
				 	return r;
				else {
					// apply the oppsite movement of a to get back to parent node
					reverse(node, a);
					node->a = aToNode;
					node->g--;
				}
			}
		}
	}

	return( NULL );
}


/* main IDA control loop */
int IDA_control_loop(  ){
	node* r = NULL;
	node n;

	int threshold;
	int newThreshold;
	int i;

	/* initialize statistics */
	generated = 0;
	expanded = 0;

	/* compute initial threshold B */
	initial_node.f = threshold = manhattan( initial_node.state );

	printf( "Initial Estimate = %d\nThreshold = ", threshold );

	 while (!r) {
		 newThreshold = INT_MAX;

		 //assign initial node to n
		 for (i = 0; i < NUM_STATES; i++) {
			 n.state[i] = initial_node.state[i];
		 }
		 n.g = initial_node.g;
		 n.f = initial_node.f;
		 n.a = initial_node.a;

		 //update threshold and recursively find path
		 printf("%d ", threshold);
		 r = ida(&n, threshold, &newThreshold);

		 //if can't find solution under current threshold, increase it and retry
		 if (!r) {
			 threshold = newThreshold;
		 }
	 }

	if(r)
		return r->g;
	else
		return -1;
}


static inline float compute_current_time()
{
	struct rusage r_usage;

	getrusage( RUSAGE_SELF, &r_usage );
	float diff_time = (float) r_usage.ru_utime.tv_sec;
	diff_time += (float) r_usage.ru_stime.tv_sec;
	diff_time += (float) r_usage.ru_utime.tv_usec / (float)1000000;
	diff_time += (float) r_usage.ru_stime.tv_usec / (float)1000000;
	return diff_time;
}

int main( int argc, char **argv )
{
	int i, solution_length;

	/* check we have a initial state as parameter */
	if( argc != 2 )
	{
		fprintf( stderr, "usage: %s \"<initial-state-file>\"\n", argv[0] );
		return( -1 );
	}

	/* read initial state */
	FILE* initFile = fopen( argv[1], "r" );
	char buffer[256];

	if( fgets(buffer, sizeof(buffer), initFile) != NULL ){
		char* tile = strtok( buffer, " " );
		for( i = 0; tile != NULL; ++i )
			{
				initial_node.state[i] = atoi( tile );
				blank_pos = (initial_node.state[i] == 0 ? i : blank_pos);
				tile = strtok( NULL, " " );
			}
	}
	else{
		fprintf( stderr, "Filename empty\"\n" );
		return( -2 );
	}

	if( i != NUM_STATES )
	{
		fprintf( stderr, "invalid initial state\n" );
		return( -1 );
	}

	/* initialize the initial node */
	initial_node.g = 0;
	initial_node.f = 0;
	initial_node.a = INT_MIN;

	print_state( initial_node.state );

	/* solve */
	float t0 = compute_current_time();

	solution_length = IDA_control_loop();

	float tf = compute_current_time();

	/* report results */
	printf( "\nSolution = %d\n", solution_length);
	printf( "Generated = ");
	printf_comma(generated);
	printf("\nExpanded = ");
	printf_comma(expanded);
	printf( "\nTime (seconds) = %.2f\nExpanded/Second = ", tf-t0 );
	printf_comma((unsigned long int) expanded/(tf+0.00000001-t0));
	printf("\n\n");

	/* aggregate all executions in a file named report.dat, for marking purposes */
	FILE* report = fopen( "report.dat", "a" );

	fprintf( report, "%s", argv[1] );
	fprintf( report, "\n\tSoulution = %d, Generated = %lu, Expanded = %lu", solution_length, generated, expanded);
	fprintf( report, ", Time = %f, Expanded/Second = %f\n\n", tf-t0, (float)expanded/(tf-t0));
	fclose(report);
	fclose(initFile);

	return( 0 );
}
