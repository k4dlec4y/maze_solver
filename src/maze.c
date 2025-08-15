#include <stdlib.h>

#include "maze.h"

#define SPACE ' '
#define WALL '#'
#define ENTRANCE 'X'

size_t max(size_t a, size_t b)
{
    return a > b ? a : b;
}

size_t min(size_t a, size_t b)
{
    return a < b ? a : b;
}

bool pos_is_equal(position_t a, position_t b)
{
    return a.x == b.x && a.y == b.y; 
}

dir_t turn_left(dir_t d)
{
    return (d + 1) % 4;
}

dir_t turn_right(dir_t d)
{
    return (d + 3) % 4;
}

void set_fence(matrix_t *matrix, position_t p)
{
    m_get(matrix, p.y, p.x)->is_fence = true;
}

bool is_fence(matrix_t *matrix, position_t p)
{
    return m_get(matrix, p.y, p.x)->is_fence;
}

void set_found(matrix_t *matrix, position_t p)
{
    m_get(matrix, p.y, p.x)->is_found = true;
}

bool is_found(matrix_t *matrix, position_t p)
{
    return m_get(matrix, p.y, p.x)->is_found;
}

position_t move(position_t p, dir_t d)
{
    switch (d) {
    case NORTH:
        return (position_t){p.x, p.y - 1};
    case WEST:
        return (position_t){p.x - 1, p.y};
    case SOUTH:
        return (position_t){p.x, p.y + 1};
    case EAST:
        return (position_t){p.x + 1, p.y};
    default:
        return p;
    }
}

bool position_is_valid(matrix_t *matrix, position_t p)
{
    return p.x >= 0 && p.x < (int)matrix->cols &&
           p.y >= 0 && p.y < (int)matrix->rows;
}

tile_t *m_get(matrix_t *matrix, int y, int x)
{
    return matrix->array + y * matrix->cols + x;
}

tile_t get_tile(matrix_t *matrix, position_t p)
{
    return *m_get(matrix, p.y, p.x);
}

static bool check_amount_of_entrances_and_newline(char **char_p,
    size_t chars_loaded, size_t entrances)
{
    if ((*char_p)[chars_loaded - 1] != '\n') {
        perror("no newline at the end of file\n");
        return false;
    }
    if (entrances < 2) {
        perror("less than 2 entrances\n");
        return false;
    }
    return true;
}

bool load_file(maze_t *maze, FILE *file, char **char_p)
{
    size_t chars_loaded = 0;
    size_t bytes_allocated = 64;
    size_t cur_line_length = 0;
    size_t last_wall_in_line = 0;
    size_t height = 1;
    size_t entrances = 0;
    bool is_space = true;

    int ch;
    while ((ch = fgetc(file)) != EOF) {

        if (chars_loaded + 1 >= bytes_allocated) {
            bytes_allocated *= 2;
            char *temp_p = realloc(*char_p, bytes_allocated * sizeof(char));
            if (temp_p == NULL) {
                perror("insufficient memory\n");
                return false;
            }
            *char_p = temp_p;
        }
        (*char_p)[chars_loaded++] = (char) ch;
        ++cur_line_length;

        if (ch == WALL || ch == ENTRANCE) {
            if (ch == ENTRANCE && ++entrances > 2) {
                perror("three or more entrances\n");
                return false;
            }
            last_wall_in_line = cur_line_length;
            maze->line_with_last_wall = height;
            maze->spare_cols = min(cur_line_length - 1, maze->spare_cols);
            is_space = false;
        }
        else if (ch == '\n') {
            if (is_space)
                ++maze->line_with_first_wall;
            ++height;
            maze->line_length = max(last_wall_in_line, maze->line_length);
            cur_line_length = 0;
        }
        else if (ch != SPACE) {
            fprintf(stderr, "invalid character %d\n", ch);
            return false;
        }
    }
    return check_amount_of_entrances_and_newline(char_p,
        chars_loaded, entrances);
}

void fill_maze_matrix(maze_t *maze, matrix_t *matrix, char *char_p)
{
    size_t i = 0;
    while (maze->line_with_first_wall > 0) {
        if (char_p[i] == '\n')
            --maze->line_with_first_wall;
        ++i;
    }

    for (size_t y = 0; y < matrix->rows; ++y) {
        for (size_t _ = 0; _ < maze->spare_cols; ++_) {
            if (char_p[i] == '\n')
                break;
            ++i;
        }
        size_t x = 0;
        size_t last_wall = -1;
        while (char_p[i] != '\n' && x < matrix->cols) {
            if (char_p[i] == WALL || char_p[i] == ENTRANCE)
                last_wall = x;

            *m_get(matrix, y, x) = (tile_t){char_p[i], false, false};
            ++i;
            ++x;
        }
        *m_get(matrix, y, last_wall + 1) = (tile_t){'\n', false, false};

        for (; x < matrix->cols; ++x)
            *m_get(matrix, y, x) = (tile_t){'\n', false, false};

        while (char_p[i] != '\n')
            ++i;
        ++i;
    }
}

void matrix_to_file(matrix_t *matrix, FILE *file)
{
    for (size_t i = 0; i < matrix->rows; ++i) {
        for (size_t j = 0; j < matrix->cols; ++j) {
            char ch = m_get(matrix, i, j)->symbol;
            fputc(ch, file);
            if (ch == '\n')
                break;
        }
    }
}

#define EMPTY_TILE 'O'
#define END_OF_LINE 'n'

void full_matrix_print(matrix_t *matrix)
{
    for (size_t i = 0; i < matrix->rows; ++i) {
        for (size_t j = 0; j < matrix->cols; ++j) {
            char ch = m_get(matrix, i, j)->symbol;
            if (ch == '\n')
                putchar(END_OF_LINE);
            else if (ch == SPACE)
                putchar(EMPTY_TILE);
            else
                putchar(ch);
        }
        putchar('\n');
    }
}

void maze_destroy(maze_t *maze)
{
    if (maze->fst_ent)
        free(maze->fst_ent);
    maze->fst_ent = NULL;
    if (maze->snd_ent)
        free(maze->snd_ent);
    maze->snd_ent = NULL;
    if (maze->starting_wall)
        free(maze->starting_wall);
    maze->starting_wall = NULL;
    free(maze); maze = NULL;
}

void matrix_destroy(matrix_t *matrix)
{
    if (matrix->array)
        free(matrix->array);
    matrix->array = NULL;
    free(matrix);
    matrix = NULL;
}
