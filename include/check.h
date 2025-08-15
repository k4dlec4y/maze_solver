#ifndef CHECK_H
#define CHECK_H

#include <stdbool.h>

#include "maze.h"

/**
 * Marks the longest path around the maze by assigning .is_fence
 * to relevant tiles.
 * It checks also if the entrances are in correct format.
 * 
 * @return true on success, false otherwise
 */
bool mark_fence(maze_t *maze, matrix_t *matrix);

/**
 * Checks if there are any independent walls (mazes) outside of the first
 * encountered maze.
 * 
 * @return true if there was nothing wrong found, false if there is a wall out
 * of the main maze
 */
bool check_outside(maze_t *maze, matrix_t *matrix, position_t pos, dir_t d);

/**
 * Checks if the maze from the file was in valid format.
 * 
 * @return true if the format is valid, false otherwise
 */
bool check(maze_t *maze, matrix_t *matrix);

#endif  // CHECK_H