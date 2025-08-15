#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef
enum direction
{
    NORTH,
    WEST,
    SOUTH,
    EAST,
    INVALID
}
dir;

typedef
struct tile_t
{
    char symbol;
    bool is_edge; // edge of maze
    bool is_found; // used while doing bfs
}
tile_t;

typedef
struct position_t
{
    int x;
    int y;
}
position_t;

typedef
struct maze_t
{
    size_t line_length; // index of the farthest # in the line
    size_t entrances; // number of Xs
    size_t line_with_first_wall; // these two are used to calculate the height of matrix
    size_t line_with_last_wall;
    size_t spare_cols; // count of cols made of ' ' on the left of maze

    position_t* fst_ent;
    dir slv_start; // first entrance + dir is the start
    position_t* snd_ent;

    // first wall we encounter when loading the file, important part of restricting the main maze:
    // it is the most upper-left wall -> there needs to be a wall on its right -> we know in which
    // direction we should head while finding the "fence" of the maze
    position_t* starting_wall;

    // the following limits are used to find out, if there is some area outside of maze's vision
    // for example:

    // 012345
    //   ###
    //   X X
    //   ###
    // #

    // in this case lowest_x would be 2 -> there is something outside, which doesn't belong
    // to the maze, and we could not have found it with check_line

    size_t lowest_x;
    size_t highest_x;
    size_t highest_y;
    // we dont need lowest_y, because the start is always on (x, 0)
}
maze_t;

typedef
struct matrix
{
    tile_t* m;
    size_t m_cols;
    size_t m_rows;
} matrix_t;

size_t max(size_t a, size_t b);

size_t min(size_t a, size_t b);

void set_tile_as_edge(position_t pos, matrix_t* matrix);

bool tile_is_edge(position_t pos, matrix_t* matrix);

void set_found(position_t pos, matrix_t* matrix);

bool get_found(position_t pos, matrix_t* matrix);

tile_t get_tile(position_t pos, matrix_t* matrix);

dir turn_left(dir d);

dir turn_right(dir d);

bool eq_pos(position_t a, position_t b);

/** fills char_pointer based on the input @p file,
 * also gather important information about the maze
 * 
 * returns true on success, false on failure**/
bool load_file(maze_t *maze, FILE *file, char **chars);

/** serves as matrix[y][x] **/
tile_t* m_get(matrix_t* matrix, int y, int x);

/** returns neighbour of position is selected direction **/
position_t add_dir(position_t a, dir d);

/** checks whether @p pos are valid coordinates for a tile **/
bool maze_is_within_bounds(matrix_t* matrix, position_t pos);

/** fills matrix with chars from char_pointer
 * can't be done in load_file because we didn't know the matrix parameters there **/
void fill_maze_matrix(char* char_p, maze_t *maze, matrix_t* matrix);

void matrix_to_file(FILE *file, matrix_t* matrix);

/** used while debugging **/
void full_matrix_print(matrix_t* matrix);

/** cleans internal fields **/
void maze_destroy(maze_t *maze);

/** cleans internal fields **/
void matrix_destroy(matrix_t *matrix);

#endif // MAZE_H
