#include "check.h"

#define SPACE ' '
#define WALL '#'
#define ENTRANCE 'X'

static void maze_add_entrance(maze_t *maze, position_t p, dir_t d)
{
    if (maze->entrance_count == 0) {
        maze->fst_ent = p;
        maze->solve_start_dir = turn_right(d);
    } else if (maze->entrance_count == 1) {
        maze->snd_ent = p;
    }
    /* entrance count is already guaranteed to be 2 (checked in load_file()) */
    ++maze->entrance_count;
}

/**
 * @return true if the entrance's surroundings is in correct format,
 * false otherwise
 */
static bool check_entrance(matrix_t *matrix, position_t p, dir_t d)
{
    position_t forward = move(p, d);
    if (!position_is_valid(matrix, forward) ||
        get_tile(matrix, forward).symbol != WALL) {
        perror("no straight wall (#X#) is passing through X");
        return false;
    }
    position_t left = move(p, turn_left(d));
    if (position_is_valid(matrix, left)) {
        char left_ch = get_tile(matrix, left).symbol;
        if (left_ch == WALL || left_ch == ENTRANCE) {
            fprintf(stderr, "no whitespace to the left of X, x=%d, y=%d, "
                            "char=%d\n", p.x, p.y, left_ch);
            return false;
        }
    }
    position_t right = move(p, turn_right(d));
    if (position_is_valid(matrix, right)) {
        char right_ch = get_tile(matrix, right).symbol;
        if (right_ch == WALL || right_ch == ENTRANCE || right_ch == '\n') {
            fprintf(stderr, "no space to the right of X, x=%d, y=%d, char=%d\n",
                    p.x, p.y, right_ch);
            return false;
        }
    }
    return true;
}

/**
 * @param p in-out parameter, the new current position is stored in there
 * after the call 
 *
 * @return the direction in which we moved, if there is no wall or entrance,
 * `INVALID_DIRECTION` is returned
 */
static dir_t next_dir(matrix_t *matrix, position_t *p, dir_t d)
{
    position_t original = *p;

    position_t left = move(original, turn_left(d));
    if (position_is_valid(matrix, left)) {
        char left_ch = get_tile(matrix, left).symbol;
        if (left_ch == WALL || left_ch == ENTRANCE) {
            *p = left;
            return turn_left(d);
        }
    }
    position_t forward = move(original, d);
    if (position_is_valid(matrix, forward)) {
        char forward_ch = get_tile(matrix, forward).symbol;
        if (forward_ch == WALL || forward_ch == ENTRANCE) {
            *p = forward;
            return d;
        }
    }
    position_t right = move(original, turn_right(d));
    if (position_is_valid(matrix, right)) {
        char right_ch = get_tile(matrix, right).symbol;
        if (right_ch == WALL || right_ch == ENTRANCE) {
            *p = right;
            return turn_right(d);
        }
    }
    return INVALID_DIRECTION;
}

static bool mark_fence_aux(maze_t *maze, matrix_t *matrix,
    position_t p, dir_t d)
{
    if (is_fence(matrix, p)) {
        if (!pos_is_equal(p, maze->starting_wall)) {
            fprintf(stderr, "while marking the fence, fence was found but it"
                            "wasn't the same position from which we started"
                            "x=%d, y=%d\n", p.x, p.y);
            return false;
        }
        if (maze->entrance_count != 2) {
            perror("at least one entrance was not placed in the fence\n");
            return false;
        }
        return true;
    }

    set_fence(matrix, p);

    if (get_tile(matrix, p).symbol == ENTRANCE) {
        maze_add_entrance(maze, p, d);
        if (!check_entrance(matrix, p, d))
            return false;
    }
    dir_t next = next_dir(matrix, &p, d);
    if (next == INVALID_DIRECTION) {
        fprintf(stderr, "while marking the fence, dead end was reached, "
                        "x=%d, y=%d\n", p.x, p.y);
        return false;
    }
    return mark_fence_aux(maze, matrix, p, next);
}

/**
 * @return true if the fence was succesfully marked, false otherwise
 */
bool mark_fence(maze_t *maze, matrix_t *matrix)
{
    size_t i = 0;
    while (m_get(matrix, 0, i)->symbol == SPACE)
        ++i;

    char first_wall = m_get(matrix, 0, i)->symbol;
    if (first_wall == ENTRANCE || first_wall == '\n') {
        fprintf(stderr, "execepted the wall, but %c was encountered\n",
                first_wall);
        return false;
    }
    if (!position_is_valid(matrix, (position_t){0, i + 1}) ||
        (m_get(matrix, 0, i + 1)->symbol != ENTRANCE &&
         m_get(matrix, 0, i + 1)->symbol != WALL)) {
        fprintf(stderr, "while marking the fence, dead end was reached, "
                        "x=%d, y=%zu\n", 0, i);
    }
    maze->starting_wall = (position_t){.x=i, .y=0};
    maze->entrance_count = 0;
    return mark_fence_aux(maze, matrix, maze->starting_wall, EAST);
}

/**
 * @return true if the maze is "extended" through the entire matrix, false
 * otherwise
 */
static bool check_low_high(maze_t *maze, matrix_t *matrix)
{
    /* matrix->cols - 2 because the last char in matrix line is always \n*/
    return maze->lowest_x == 0 &&
           maze->highest_x == matrix->cols - 2 &&
           maze->highest_y == matrix->rows - 1;
}

/**
 * Marches in a straight direction away from the maze's fence to check if there
 * are any independent walls (mazes) outside of the first encountered maze.
 *
 * @return true if there was nothing wrong found, false if there is a wall out
 * of the main maze
 */
static bool check_line(matrix_t *matrix, position_t p, dir_t d)
{
    if (!position_is_valid(matrix, p))
        return true;

    tile_t cur_tile = get_tile(matrix, p);
    while (!cur_tile.is_fence) {
        if (cur_tile.symbol == WALL)
            return false;

        p = move(p, d);
        if (!position_is_valid(matrix, p))
            return true;

        cur_tile = get_tile(matrix, p);
    }
    return true;
}

bool check_outside(maze_t *maze, matrix_t *matrix, position_t p, dir_t d)
{
    if (pos_is_equal(p, maze->starting_wall)) {
        if (!check_low_high(maze, matrix)) {
            perror("other maze exists which can't be found by marching\n");
            return false;
        }
        return true;
    }

    dir_t away_from_fence = turn_left(d);
    if (!check_line(matrix, move(p, away_from_fence), away_from_fence)) {
        perror("other maze was found while marching away from the fence\n");
        return false;
    }
    dir_t next = next_dir(matrix, &p, d);
    if (next == INVALID_DIRECTION) {
        fprintf(stderr, "dead end was reached while checking outside, "
                        "(this should not happen?)\n");
        return false;
    }
    maze->lowest_x = min(maze->lowest_x, p.x);
    maze->highest_x = max(maze->highest_x, p.x);
    maze->highest_y = max(maze->highest_y, p.y);
    return check_outside(maze, matrix, p, next);
}

bool check(maze_t *maze, matrix_t *matrix)
{
    if (!mark_fence(maze, matrix))
        return false;

    maze->lowest_x = maze->starting_wall.x;
    maze->highest_x = maze->starting_wall.x;
    maze->highest_y = 0;
    return check_outside(maze, matrix, move(maze->starting_wall, EAST), EAST);
}
