#include "check.h"

#define SPACE ' '
#define WALL '#'
#define ENTRANCE 'X'

/** updates the informations about entrances
 * used by restrict_maze
 * returns true on success
**/
static bool maze_add_entrance(maze_t* maze, position_t pos, dir_t d)
{
    if (maze->entrance_count == 0)
    {
        maze->fst_ent = malloc(sizeof(position_t));
        if (maze->fst_ent == NULL)
        {
            perror("couldn't allocate memory for fst_ent\n");
            return false;
        }
        *(maze->fst_ent) = pos;
        maze->solve_start_dir = turn_right(d);
        maze->entrance_count++;
    }
    else if (maze->entrance_count == 1)
    {
        maze->snd_ent = malloc(sizeof(position_t));
        if (maze->snd_ent == NULL)
        {
            perror("couldn't allocate memory for snd_ent\n");
            return false;
        }
        *(maze->snd_ent) = pos;
        maze->entrance_count++;
    }
    return true;
}

/** checks if the tiles around the entrance are correct
 * used by restrict_maze
 * returns true on success
**/
static bool check_entrance(matrix_t* matrix, position_t pos, dir_t d)
{
    position_t forward = move(pos, d);
    if (!position_is_valid(matrix, forward) || get_tile(matrix, forward).symbol != WALL)
    {
        perror("no # line going through X");
        return false;
    }
    position_t left = move(pos, turn_left(d));
    if (position_is_valid(matrix, left))
    {
        char left_c = get_tile(matrix, left).symbol;
        if (left_c == WALL || left_c == ENTRANCE)
        {
            fprintf(stderr, "no whitespace around X on left, x %d, y %d, c %d\n", pos.x, pos.y, left_c);
            return false;
        }
    }
    position_t right = move(pos, turn_right(d));
    if (position_is_valid(matrix, right))
    {
        char right_c = get_tile(matrix, right).symbol;
        if (right_c == WALL || right_c == '\n')
        {
            fprintf(stderr, "no whitespace around X on right, x %d, y %d, c %d\n", pos.x, pos.y, right_c);
            return false;
        }
    }
    return true;
}

/** returns direction of next wall while restricting the maze **/
static dir_t next_dir(position_t *pos, dir_t d, matrix_t* matrix)
{
    position_t left = move(*pos, turn_left(d));
    if (position_is_valid(matrix, left))
    {
        char left_c = get_tile(matrix, left).symbol;
        if (left_c == WALL || left_c == ENTRANCE)
        {
            *pos = left;
            return turn_left(d);
        }
    }

    position_t forward = move(*pos, d);
    if (position_is_valid(matrix, forward))
    {
        char forward_c = get_tile(matrix, forward).symbol;
        if (forward_c == WALL || forward_c == ENTRANCE)
        {
            *pos = forward;
            return d;
        }
    }

    position_t right = move(*pos, turn_right(d));
    if (position_is_valid(matrix, right))
    {
        char right_c = get_tile(matrix, right).symbol;
        if (right_c == WALL || right_c == ENTRANCE)
        {
            *pos = right;
            return turn_right(d);
        }
    }
    return INVALID_DIRECTION;
}

static bool restrict_matrix_aux(position_t pos, dir_t d, maze_t *maze, matrix_t* matrix)
{
    if (is_fence(matrix, pos))
    {
        if (!pos_is_equal(pos, *(maze->starting_wall)))
        {
            fprintf(stderr, "same edge but not the end, x %d, y %d\n", pos.x, pos.y);
            return false;
        }
        if (maze->entrance_count != 2)
        {
            perror("entrances are not on edge\n");
            return false;
        }
        return true;
    }
    set_fence(matrix, pos);

    if (get_tile(matrix, pos).symbol == ENTRANCE)
    {
        if (!maze_add_entrance(maze, pos, d) || !check_entrance(matrix, pos, d))
        {
            return false;
        }
    }
    dir_t next = next_dir(&pos, d,matrix);
    if (next == INVALID_DIRECTION)
    {
        fprintf(stderr, "dead end, x %d, y %d\n", pos.x, pos.y);
        return false;
    }
    return restrict_matrix_aux(pos, next, maze, matrix);
}

bool restrict_matrix(maze_t *maze, matrix_t* matrix)
{
    size_t i = 0;
    while (m_get(matrix, 0, i)->symbol == SPACE)
    {
        i++;
    }
    if (m_get(matrix, 0, i)->symbol == ENTRANCE || m_get(matrix, 0, i)->symbol == '\n')
    {
        perror("unexcepted char\n");
        return false;
    }
    if (!position_is_valid(matrix, (position_t){0, i +1}) ||
        (m_get(matrix, 0, i + 1)->symbol != ENTRANCE &&
         m_get(matrix, 0, i + 1)->symbol != WALL)
       )
    {
        fprintf(stderr, "dead end, x %d, y %zu\n", 0, i);
        return false;
    }
    maze->starting_wall = malloc(sizeof(position_t));
    if (maze->starting_wall == NULL)
    {
        perror("couldn't allocate memory\n");
        return false;
    }
    *(maze->starting_wall) = (position_t){.x=i, .y=0};
    maze->entrance_count = 0;
    return restrict_matrix_aux(*(maze->starting_wall), EAST, maze, matrix);
}

/** checks if the maze is "extended" through the entire matrix **/
static bool check_low_high(matrix_t *matrix, maze_t *maze)
{
    // maze->line_length - 2 because the last char in matrix line is always \n
    return maze->lowest_x == 0 &&
           maze->highest_x == matrix->cols - 2 &&
           maze->highest_y == matrix->rows - 1;
}

bool check_line(position_t pos, dir_t d, matrix_t* matrix)
{
    if (!position_is_valid(matrix, pos))
    {
        return true;
    }
    tile_t cur_tile = get_tile(matrix, pos);
    while (!cur_tile.is_fence)
    {
        if (cur_tile.symbol == WALL)
        {
            return false;
        }
        pos = move(pos, d);
        if (!position_is_valid(matrix, pos))
        {
            return true;
        }
        cur_tile = get_tile(matrix, pos);
    }
    return true;
}

/** after restricting the maze, we know, which walls create a fence ->
 * now, we can check, if there are some other components, outside the maze
 * restricting was necessary, otherwise this maze:
 * 
 * ###  ###
 * # 1  2 #
 * # #### #  (1, 2 are #)
 * #      #
 * ########
 * 
 * returns that there are more components, because check_line called on 1
 * will find 2 and without knowing that 2 is part of the fence it would return false
**/

bool check_outside(position_t pos, dir_t d, maze_t *maze, matrix_t* matrix)
{
    if (pos_is_equal(pos, *(maze->starting_wall)))
    {
        if (!check_low_high(matrix, maze))
        {
            perror("2 or more components out of their coordinates\n");
            return false;
        }
        return true;
    }

    if (!check_line(move(pos, turn_left(d)), turn_left(d), matrix))
    {
        perror("2 or more components within their coordinates\n");
        return false;
    }

    dir_t next = next_dir(&pos, d, matrix);
    if (next == INVALID_DIRECTION)
    {
        fprintf(stderr, "dead end, x %d, y %d\n", pos.x, pos.y);
        return false;
    }

    maze->lowest_x = min(maze->lowest_x, pos.x);
    maze->highest_x = max(maze->highest_x, pos.x);
    maze->highest_y = max(maze->highest_y, pos.y);

    return check_outside(pos, next, maze, matrix);
}