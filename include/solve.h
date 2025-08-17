#include <stdbool.h>
#include <unistd.h>

#include "maze.h"

typedef
struct vertex {
    position_t position;
    /* the direction from which we reached the vertex */
    dir_t direction;
    /* index used to go back from snd_ent to fst_ent */
    /* ssize_t because start has parent_index = -1*/
    ssize_t parent_index;
    ssize_t index;
}
vertex_t;

/**
 * Tries to find the shortest path using BFS.
 * 
 * @return true on success, false otherwise
 */
bool solve_maze(maze_t *maze, matrix_t *matrix, position_t start);
