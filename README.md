# 15Puzzle-Solver-IDAStar-DFS-Heuristic

* The second project of subject [COMP20003](https://handbook.unimelb.edu.au/2020/subjects/comp20003) (Algorithm and Data Structure) at University of Melbourne, 2018 Sem2.

## Introduction

A solver for the 15–puzzle, take a sequence of states, returns the optimal number of operation needed to get the puzzle in form:
B  1  2  3
4  5  6  7
8  9  10 11
12 13 14 15
Where B means the blank space. A puzzle solution is a sequence of moves which achieves the following state
subject to the constraint that we can only swap positions of the blank tile with some adjacent one. 

Use Iterative Deepening A* (IDA*) algorithm which follows the Depth-first search strategy. IDA* has two parts:
1. The main loop, initializes the thresholds B and B′ first. If no solution is found the search is triggered again but with an updated B = B′ threshold.
2. The recursive function that implements the bounded Depth-First Search.

The other heuristics being used to prune the search space is the sum of Manhattan distances, Linear Conflict and Last Move Heuristic. Most of these works are based on "Finding Optimal Solutions to the Twenty-Four Puzzle" by Richard E. Korf and Larry A. Taylor http://www.aaai.org/Papers/AAAI/1996/AAAI96-178.pdf

## Usage 

**Input**:
Must be solveable!!! Solver has to read the initial configuration from a file called "1.puzzle" with the same format as it appears in the table, that is, a single line, containing a sorted list of indexes separated by a blank space. For example:
14 13 15 7 11 12 9 5 6 0 2 1 4 8 10 3

**Output**:
The solver will print into the stdout the following information: 
1. Initial state of the puzzle.
2. h(s0) heuristic estimate for the initial state.
3. Thresholds the search have used to solve the problem. 
4. Number of moves of the optimal solution.
5. Number of generated nodes (Nodes that have been created within the search. If you imagine the search algorithm building a tree, these are all the nodes of the tree).
6. Number of expanded nodes (Nodes for which its children have been generated. Here, it corresponds to the nodes that haven’t been pruned by the threshold, or in terms of the search tree would be the internal nodes of the tree).
7. Number of expanded nodes per second. 
8. Total Search Time, in seconds.

## Contributions

Input and Output handling are coded by Grady Fitzpatrick (grady.fitzpatrick@unimelb.edu.au).

Soving Algorithm and heuristic are coded by Xiuge Chen (xiugec@student.unimelb.edu.au)

15/12/2018
