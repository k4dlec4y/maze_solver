#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "check.h"
#include "solve.h"

static void maze_init(maze_t *maze)
{
    assert(maze != NULL);

    maze->line_length = 0;
    maze->line_with_first_wall = 0;
    maze->line_with_last_wall = 0;
    maze->spare_cols = SIZE_MAX;
}

static bool check_arguments(int argc, const char *mode, bool *with_solve)
{
    assert(mode != NULL);
    assert(with_solve != NULL);

    if (strcmp(mode, "check") == 0 && argc == 3) {
        *with_solve = false;
        return true;
    } else if (strcmp(mode, "solve") == 0 && argc == 4) {
        *with_solve = true;
        return true;
    }
    fputs("invalid parameters\n", stderr);
    return false;
}

static bool maze_create(char *path, maze_t **maze, char **char_pointer)
{
    assert(path != NULL);
    assert(maze != NULL);
    assert(char_pointer != NULL);

    FILE *program = fopen(path, "r");
    if (program == NULL) {
        perror("couldn't load the source file");
        return false;
    }
    *maze = malloc(sizeof(maze_t));
    if (*maze == NULL) {
        fputs("couldn't allocate memory\n", stderr);
        fclose(program); program = NULL;
        return false;
    }
    maze_init(*maze);

    /*
     * if you edit 64 here, please edit bytes_allocated variable in maze.c,
     * load_file() function to the same amount
     */
    *char_pointer = malloc(64 * sizeof(char));
    if (*char_pointer == NULL) {
        fputs("couldn't allocate memory\n", stderr);
        maze_destroy(*maze); *maze = NULL;
        fclose(program); program = NULL;
        return false;
    }
    if (!load_file(*maze, program, char_pointer)) {
        maze_destroy(*maze); *maze = NULL;
        free(*char_pointer); *char_pointer = NULL;
        fclose(program); program = NULL;
        return false;
    }
    fclose(program); program = NULL;
    return true;
}

static bool matrix_create(maze_t *maze, matrix_t *matrix, char *char_pointer)
{
    assert(maze != NULL);
    assert(matrix != NULL);
    assert(char_pointer != NULL);

    if (matrix == NULL) {
        fputs("couldn't allocate memory\n", stderr);
        maze_destroy(maze); maze = NULL;
        free(char_pointer); char_pointer = NULL;
        return false;
    }
    /* + 1 to add space for \n */
    matrix->cols = maze->line_length + 1 - maze->spare_cols;
    matrix->rows = maze->line_with_last_wall - maze->line_with_first_wall;
    matrix->array = malloc(sizeof(tile_t) * matrix->cols * matrix->rows);
    if (matrix->array == NULL) {
        fputs("couldn't allocate memory\n", stderr);
        maze_destroy(maze); maze = NULL;
        matrix_destroy(matrix); matrix = NULL;
        free(char_pointer); char_pointer = NULL;
        return false;
    }
    fill_maze_matrix(maze, matrix, char_pointer);
    free(char_pointer); char_pointer = NULL;
    return true;
}

static bool solve(char *path, maze_t *maze, matrix_t *matrix)
{
    assert(path != NULL);
    assert(maze != NULL);
    assert(matrix != NULL);

    position_t start = move(maze->fst_ent, maze->solve_start_dir);
    if (!solve_maze(maze, matrix, start)) {
        maze_destroy(maze); maze = NULL;
        matrix_destroy(matrix); matrix = NULL;
        return false;
    }
    FILE *output = fopen(path, "w");
    if (output == NULL) {
        perror("couldn't open the program for writing");
        maze_destroy(maze); maze = NULL;
        matrix_destroy(matrix); matrix = NULL;
        return false;
    }
    matrix_to_file(matrix, output);
    fclose(output); output = NULL;
    return true;
}

int main(int argc, char *argv[])
{
    bool with_solve;
    if (!check_arguments(argc, argv[1], &with_solve))
        return EXIT_FAILURE;

    maze_t *maze;
    char *char_pointer;
    if (!maze_create(argv[2], &maze, &char_pointer))
        return EXIT_FAILURE;

    matrix_t *matrix = malloc(sizeof(matrix_t));
    if (!matrix_create(maze, matrix, char_pointer))
        return EXIT_FAILURE;

    if (!check(maze, matrix))
        return EXIT_FAILURE;

    if (with_solve && !solve(argv[3], maze, matrix))
        return EXIT_FAILURE;

    maze_destroy(maze); maze = NULL;
    matrix_destroy(matrix); matrix = NULL;
    return EXIT_SUCCESS;
}
