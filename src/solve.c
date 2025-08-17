#include <stdlib.h>
#include <stdio.h>

#include "../include/solve.h"
#include "../include/queue.h"

#define SPACE ' '
#define ENTRANCE 'X'
#define PATH 'o'

static bool allocate_resources(queue_t **bfs_queue, vertex_t **vertices,
    ssize_t vertices_size)
{
    *bfs_queue = malloc(sizeof(queue_t));
    if (*bfs_queue == NULL) {
        fputs("couldn't allocate memory for queue\n", stderr);
        return false;
    }
    queue_init(*bfs_queue);

    *vertices = malloc(sizeof(vertex_t) * vertices_size);
    if (*vertices == NULL) {
        fputs("couldn't allocate memory for vertices\n", stderr);
        free(*bfs_queue);
        return false;
    }
    return true;
}

static void free_resources(queue_t *bfs_queue, vertex_t *vertices)
{
    queue_free(bfs_queue);
    free(bfs_queue); bfs_queue = NULL;
    free(vertices); vertices = NULL;
}

static bool vertices_realloc(vertex_t **vertices, ssize_t *size, ssize_t n)
{
    if (n < *size)
        return true;

    *size *= 2;
    vertex_t *temp = realloc(*vertices, *size * sizeof(vertex_t));
    if (temp == NULL) {
        fputs("couldn't allocate memory for vertices\n", stderr);
        return false;
    }
    *vertices = temp;
    return true;
}

static void fill_in_path(vertex_t *vertices, matrix_t *matrix, ssize_t i,
    position_t fst_ent)
{
    /* start has parent_index = -1 */
    while (i > -1) {
        vertex_t vertex = vertices[i];
        m_get(matrix, vertex.position.y, vertex.position.x)->symbol = PATH;
        i = vertex.parent_index;
    }
    m_get(matrix, fst_ent.y, fst_ent.x)->symbol = PATH;
}

static void set_neighbours(matrix_t *matrix, position_t *p, dir_t d,
    tile_t *tiles, position_t *neigs, dir_t *dirs)
{
    dirs[0] = turn_left(d);
    dirs[1] = d;
    dirs[2] = turn_right(d);

    neigs[0] = move(*p, dirs[0]);
    neigs[1] = move(*p, dirs[1]);
    neigs[2] = move(*p, dirs[2]);

    tiles[0] = get_tile(matrix, neigs[0]);
    tiles[1] = get_tile(matrix, neigs[1]);
    tiles[2] = get_tile(matrix, neigs[2]);
}

bool solve_maze(maze_t *maze, matrix_t *matrix, position_t start)
{
    queue_t *bfs_queue;
    vertex_t *vertices;
    ssize_t vertices_size = 64;
    if (!allocate_resources(&bfs_queue, &vertices, vertices_size))
        return false;

    ssize_t n = 0;
    set_found(matrix, maze->fst_ent);
    vertices[n] = (vertex_t){start, maze->solve_start_dir, -1, n};
    queue_insert(bfs_queue, (node_t){NULL, n});
    ++n;

    while (!queue_is_empty(bfs_queue)) {
        ssize_t index = queue_pop(bfs_queue).index;
        position_t p = vertices[index].position;
        dir_t d = vertices[index].direction;

        if (pos_is_equal(p, maze->snd_ent)) {
            fill_in_path(vertices, matrix, index, maze->fst_ent);
            free_resources(bfs_queue, vertices);
            return true;
        }

        dir_t dirs[3]; position_t neigs[3]; tile_t tiles[3];
        set_neighbours(matrix, &p, d, tiles, neigs, dirs);
        for (int i = 0; i < 3; i++) {
            if (is_found(matrix, neigs[i]))
                continue;

            char box = tiles[i].symbol;
            if (box == SPACE || box == ENTRANCE) {
                if (!vertices_realloc(&vertices, &vertices_size, n)) {
                    free_resources(bfs_queue, vertices);
                    return false;
                }
                set_found(matrix, neigs[i]);
                vertices[n] = (vertex_t){neigs[i], dirs[i], index, n};
                queue_insert(bfs_queue, (node_t){NULL, n});
                ++n;
            }
        }
    }
    fputs("no path was found\n", stderr);
    free_resources(bfs_queue, vertices);
    return false;
}
