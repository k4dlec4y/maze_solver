#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "maze.h"
#include "check.h"
#include "solve.h"

#define PATH 'o'

void set_maze(maze_t** maze)
{
    (*maze)->line_length = 0;
    (*maze)->line_with_first_wall = 0;
    (*maze)->line_with_last_wall = 0;
    (*maze)->spare_cols = SIZE_MAX;

    (*maze)->fst_ent = NULL;
    (*maze)->snd_ent = NULL;
    (*maze)->starting_wall = NULL;
}

bool check_arguments(int argc, char* argv[], bool* with_solve)
{
    assert(argv[1] != NULL);
    if (strcmp(argv[1], "check") == 0 && argc == 3)
    {
        *with_solve = false;
        return true;
    }
    else if (strcmp(argv[1], "solve") == 0 && argc == 4)
    {
        *with_solve = true;
        return true;
    }
    perror("invalid parameters\n");
    return false;
}

bool creating_the_maze(char* path, maze_t** maze, char** char_pointer)
{
    FILE* program = fopen(path, "r");
    if (program == NULL)
    {
        perror("couldn't load the source file\n");
        return false;
    }

    *maze = malloc(sizeof(maze_t));
    if (maze == NULL)
    {
        perror("couldn't allocate memory\n");
        fclose(program); program = NULL;
        return false;
    }

    set_maze(maze);

    *char_pointer = (char*)malloc(50 * sizeof(char));
    if (char_pointer == NULL)
    {
        perror("couldn't allocate memory\n");
        maze_destroy(*maze); *maze = NULL;
        fclose(program); program = NULL;
        return false;
    }

    if (!load_file(*maze, program, char_pointer))
    {
        maze_destroy(*maze); *maze = NULL;
        free(*char_pointer); *char_pointer = NULL;
        fclose(program); program = NULL;
        return false;
    }
    fclose(program); program = NULL;
    return true;
}

bool creating_the_matrix(matrix_t* matrix, maze_t* maze, char* char_pointer)
{
    if (matrix == NULL)
    {
        perror("couldn't allocate memory\n");
        maze_destroy(maze); maze = NULL;
        free(char_pointer); char_pointer = NULL;
        return false;
    }

    matrix->m_cols = maze->line_length + 1 - maze->spare_cols; // + 1 adding space for \n
    matrix->m_rows = maze->line_with_last_wall - maze->line_with_first_wall;
    matrix->m = malloc(sizeof(tile_t) * matrix->m_cols * matrix->m_rows);
    if (matrix->m == NULL)
    {
        perror("couldn't allocate memory\n");
        maze_destroy(maze); maze = NULL;
        free(char_pointer); char_pointer = NULL;
        free(matrix);
        return false;
    }

    fill_maze_matrix(char_pointer, maze, matrix);
    free(char_pointer); char_pointer = NULL;
    return true;
}

bool check(matrix_t* matrix, maze_t* maze)
{
    if (!restrict_matrix(maze, matrix))
    {
        maze_destroy(maze); maze = NULL;
        matrix_destroy(matrix); matrix = NULL;
        return false;
    }

    maze->lowest_x = maze->starting_wall->x;
    maze->highest_x = maze->starting_wall->x;
    maze->highest_y = 0;

    if (!check_outside(add_dir(*(maze->starting_wall), EAST), EAST, maze, matrix))
    {
        maze_destroy(maze); maze = NULL;
        matrix_destroy(matrix); matrix = NULL;
        return false;
    }
    return true;
}

bool solve(char* path, matrix_t* matrix, maze_t* maze)
{
    position_t start = add_dir(*(maze->fst_ent), maze->slv_start);
    if (!solve_maze(maze, matrix, start))
    {
        maze_destroy(maze); maze = NULL;
        matrix_destroy(matrix); matrix = NULL;
        return false;
    }
    m_get(matrix, maze->fst_ent->y, maze->fst_ent->x)->symbol = PATH;
    m_get(matrix, start.y, start.x)->symbol = PATH;

    FILE* output = fopen(path, "w");
    if (output == NULL)
    {
        perror("couldn't open the program for writing\n");
        maze_destroy(maze); maze = NULL;
        matrix_destroy(matrix); matrix = NULL;
        return false;
    }
    matrix_to_file(output, matrix);
    
    fclose(output); output = NULL;
    return true;
}


int main(int argc, char* argv[])
{
    bool with_solve;
    if (!check_arguments(argc, argv, &with_solve))
    {
        return EXIT_FAILURE;
    }

    maze_t* maze;
    char* char_pointer;
    if (!creating_the_maze(argv[2], &maze, &char_pointer))
    {
        return EXIT_FAILURE;
    }

    matrix_t* matrix = malloc(sizeof(matrix_t));
    if (!creating_the_matrix(matrix, maze, char_pointer))
    {
        return EXIT_FAILURE;
    }

    if (!check(matrix, maze))
    {
        return EXIT_FAILURE;
    }
    
    if (with_solve)
    {
        if (!solve(argv[3], matrix, maze))
        {
            return EXIT_FAILURE;
        }
    }

    maze_destroy(maze); maze = NULL;
    matrix_destroy(matrix); matrix = NULL;
    return EXIT_SUCCESS;
}
