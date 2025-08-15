#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "maze.h"

/** creates the longest path around the maze by assigning .is_edge,
 * also checks if the entrances are correct
 * returns true on success, false on failure
**/
bool restrict_matrix(maze_t *maze, matrix_t* matrix);

/** goes in a direction to check if there is # out of the maze
 * used by check_outside
 * returns false if there is # out of the maze*
**/
bool check_line(position_t pos, dir d, matrix_t* matrix);

/** checks if there are any walls outside of the maze 
 * returns false if there is # out of the maze*
**/
bool check_outside(position_t pos, dir d, maze_t *maze, matrix_t* matrix);
