#include "maze.h"

typedef
struct vertex{
    position_t pos;
    dir_t d; // direction where vertex heads

    size_t prev; // index used to go back from snd_ent to fst_ent
    size_t index;
} vertex;

/** returns true on success, false on failure **/
bool solve_maze(maze_t *maze, matrix_t* matrix, position_t start);
