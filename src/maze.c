#include "maze.h"

#define SPACE ' '
#define EMPTY_TILE 'O'
#define END_OF_LINE 'n'
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

tile_t* m_get(matrix_t* matrix, int y, int x)
{
    return matrix->m + y * matrix->m_cols + x;
}

bool maze_is_within_bounds(matrix_t *matrix, position_t pos)
{
    return pos.x >=0 && pos.x < (int)matrix->m_cols && pos.y >= 0 && pos.y < (int)matrix->m_rows;
}

position_t add_dir(position_t a, dir d)
{
    switch (d)
    {
    case NORTH:
        return (position_t){a.x, a.y - 1};
    
    case WEST:
        return (position_t){a.x - 1, a.y};
    
    case SOUTH:
        return (position_t){a.x, a.y + 1};

    case EAST:
        return (position_t){a.x + 1, a.y};

    default:
        return a;
    }
}

void set_tile_as_edge(position_t pos, matrix_t* matrix)
{
    m_get(matrix, pos.y, pos.x)->is_edge = true;
}

bool tile_is_edge(position_t pos, matrix_t* matrix)
{
    return m_get(matrix, pos.y, pos.x)->is_edge;
}

void set_found(position_t pos, matrix_t* matrix)
{
    m_get(matrix, pos.y, pos.x)->is_found = true;
}

bool get_found(position_t pos, matrix_t* matrix)
{
    return m_get(matrix, pos.y, pos.x)->is_found;
}

tile_t get_tile(position_t pos, matrix_t* matrix)
{
    return *(m_get(matrix, pos.y, pos.x));
}

dir turn_left(dir d)
{
    return (d + 1) % 4;
}

dir turn_right(dir d)
{
    return (d + 3) % 4;
}

bool eq_pos(position_t a, position_t b)
{
    return a.x == b.x && a.y == b.y; 
}

bool check_amount_of_entrances_and_newline(char **char_p, size_t chars_loaded, size_t entrances)
{
    if (*(*char_p + chars_loaded - 1) != '\n')
    {
        perror("no newline at the end of file\n");
        return false;
    }
    if (entrances < 2)
    {
        perror("less than 2 entrances\n");
        return false;
    }
    return true;
}

bool load_file(maze_t *maze, FILE *file, char **char_p)
{
    size_t bytes_allocated = 50;
    size_t chars_loaded = 0;
    size_t line_length = 0;
    size_t last_wall_in_line = 0;
    size_t height = 1;
    size_t entrances = 0;
    bool is_space = true;

    int ch = fgetc(file);

    while (ch != EOF && ch != '\0')
    {
        chars_loaded++;
        if (chars_loaded >= bytes_allocated) // >= because at the end i want to insert '\0'
        {
            bytes_allocated += 50;
            char* temp_p = (char*)realloc(*char_p, bytes_allocated * sizeof(char));
            if (temp_p == NULL)
            {
                perror("couldn't load the program\n");
                return false;
            }

            *char_p = temp_p;
        }

        if (ch == WALL || ch == ENTRANCE)
        {
            last_wall_in_line = line_length + 1;
            maze->line_with_last_wall = height;
            maze->spare_cols = min(line_length, maze->spare_cols);
        }
        line_length++;

        if (ch != WALL && ch != ENTRANCE && ch != SPACE && ch != '\n')
        {
            fprintf(stderr, "invalid character %d\n", ch);
            return false;
        }
        if (ch != SPACE && ch != '\n')
        {
            is_space = false;
        }

        *(*char_p + chars_loaded - 1) = (char)ch;

        if (ch == '\n')
        {
            if (is_space)
            {
                maze->line_with_first_wall++;
            }
            height++;
            maze->line_length = max(last_wall_in_line, maze->line_length);
            line_length = 0;
        }

        if (ch == ENTRANCE)
        {
            entrances++;
            if (entrances > 2)
            {
                perror("three or more entrances\n");
                return false;
            }
        }
        ch = fgetc(file);
    }
    return check_amount_of_entrances_and_newline(char_p, chars_loaded, entrances);
}

void fill_maze_matrix(char* char_p, maze_t *maze, matrix_t* matrix)
{
    size_t i = 0;
    while (maze->line_with_first_wall > 0)
    {
        while (char_p[i] != '\n')
        {
            i++;
        }
        i++;
        maze->line_with_first_wall--;
    }

    for (size_t y = 0; y < matrix->m_rows; y++)
    {
        for (size_t j = 0; j < maze->spare_cols; j++)
        {
            if (char_p[i] == '\n')
            {
                break;
            }
            i++;
        }

        size_t x = 0;
        size_t last_wall = -1;
        while (char_p[i] != '\n' && x < matrix->m_cols)
        {
            if (char_p[i] == WALL || char_p[i] == ENTRANCE)
            {
                last_wall = x;
            }
            *(m_get(matrix, y, x)) = (tile_t){char_p[i], false, false};
            i++;
            x++;
        }

        *(m_get(matrix, y, last_wall + 1)) = (tile_t){'\n', false, false};

        while (x < matrix->m_cols) // so there is no garbage
        {
            *(m_get(matrix, y, x)) = (tile_t){'\n', false, false};
            x++;
        }

        while (char_p[i] != '\n')
        {
            i++;
        }
        i++;
    }
}

void matrix_to_file(FILE *file, matrix_t* matrix)
{
    for (size_t i = 0; i < matrix->m_rows; i++)
    {
        size_t j = 0;
        while (m_get(matrix, i, j)->symbol != '\n' &&j < matrix->m_cols)
        {
            fputc(m_get(matrix, i, j)->symbol, file);
            j++;
        }
        fputc('\n', file);
    }
}

void full_matrix_print(matrix_t* matrix)
{
    for (size_t i = 0; i < matrix->m_rows; i++)
    {
        size_t j = 0;
        while (j < matrix->m_cols)
        {
            if (m_get(matrix, i, j)->symbol == '\n')
            {
                putchar(END_OF_LINE);
            }
            else if (m_get(matrix, i, j)->symbol == SPACE)
            {
                putchar(EMPTY_TILE);
            }
            else
            {
                putchar(m_get(matrix, i, j)->symbol);
            }
            j++;
        }
        putchar('\n');
    }
}

void maze_destroy(maze_t *maze)
{
    if (maze->fst_ent != NULL)
    {
        free(maze->fst_ent); maze->fst_ent = NULL;
    }
    if (maze->snd_ent != NULL)
    {
        free(maze->snd_ent); maze->snd_ent = NULL;
    }
    if (maze->starting_wall != NULL)
    {
        free(maze->starting_wall); maze->starting_wall = NULL;
    }
    free(maze); maze = NULL;
}

void matrix_destroy(matrix_t *matrix)
{
    if (matrix->m != NULL)
    {
        free(matrix->m); matrix->m = NULL;
    }
    free(matrix); matrix = NULL;
}
