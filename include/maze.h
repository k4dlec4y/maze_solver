#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>
#include <stdio.h>

typedef
enum direction {
    NORTH,
    WEST,
    SOUTH,
    EAST,
    INVALID_DIRECTION
}
dir_t;

typedef
struct tile {
    char symbol;
    /* if it is part of maze's most outer wall */
    bool is_fence;
    /* used during BFS */
    bool is_found;
}
tile_t;

typedef
struct position {
    int x;
    int y;
}
position_t;

typedef
struct maze {
    /* index of the furthest # in the line */
    size_t line_length;
    size_t entrance_count;
    /* these two are used to calculate matrix height */
    size_t line_with_first_wall;
    size_t line_with_last_wall;
    /* number of columns made of ' ' to the left of the maze */
    size_t spare_cols;

    /* first entrance + dir is where BFS starts */
    position_t *fst_ent;
    dir_t solve_start_dir;
    position_t *snd_ent;

    /*
     * first wall encountered when loading the file
     * 
     * important part of marking maze's fence - it is the most upper wall
     * on the left -> there must be a wall to the right of it -> this gives us
     * the direction we should take when looking for the fence
     */
    position_t *starting_wall;

    /*
     * the following extrema are used to check, if there are other independent
     * mazes which could not be found with check_line()
     * 
     * lowest_y = 0, because initial empty lines are already removed
     */
    size_t lowest_x;
    size_t highest_x;
    size_t highest_y;
}
maze_t;

typedef
struct matrix {
    /* array is 1-dimensional, see m_get()*/
    tile_t *array;
    size_t cols;
    size_t rows;
}
matrix_t;

size_t max(size_t a, size_t b);

size_t min(size_t a, size_t b);

bool pos_is_equal(position_t a, position_t b);

dir_t turn_left(dir_t d);

dir_t turn_right(dir_t d);

void set_fence(matrix_t *matrix, position_t p);

bool is_fence(matrix_t *matrix, position_t p);

void set_found(matrix_t *matrix, position_t p);

bool is_found(matrix_t *matrix, position_t p);

/*
 * Moves in selected direction.
 * 
 * @return new position
 */
position_t move(position_t a, dir_t d);

/*
 * Checks whether position p has valid (in bounds) matrix coordinates.
 * 
 * @return true if postion is valid, false otherwise
 */
bool position_is_valid(matrix_t *matrix, position_t p);

/*
 * Serves as matrix[y][x].
 * 
 * @return address of requested tile
 */
tile_t *m_get(matrix_t *matrix, int y, int x);

/*
 * Returns tile at given position.
 * 
 * @return selected tile.
 */
tile_t get_tile(matrix_t *matrix, position_t p);

/*
 * Reads the file and stores it into char array,
 * gathers also important information about the maze.
 * 
 * @return true on success, false otherwise
 * 
 * @note address of char_p is passed due to realloc
 */
bool load_file(maze_t *maze, FILE *file, char **char_p);

/*
 * Fills the matrix with characters from char_p.
 * 
 * This can't be done in load_file(), because we didn't know matrix's (maze's)
 * size while loading the file.
 */
void fill_maze_matrix(maze_t *maze, matrix_t *matrix, char *char_p);

/*
 * Writes the matrix to file. It doesn't print chars after the line's last #.
 */
void matrix_to_file(matrix_t *matrix, FILE *file);

/*
 * Prints the matrix on stdout, prints also chars after the line's last #.
 * 
 * Used while debugging only.
 */
void full_matrix_print(matrix_t *matrix);

/*
 * Cleans internal fields.
 */
void maze_destroy(maze_t *maze);

/*
 * Cleans internal fields.
 */
void matrix_destroy(matrix_t *matrix);

#endif  // MAZE_H
