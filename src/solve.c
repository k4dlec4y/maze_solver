#include "solve.h"
#include "queue.h"

#define SPACE ' '
#define ENTRANCE 'X'
#define PATH 'o'

static void free_resources_solve(queue_t* bfs_queue, vertex* vertexes)
{
    queue_free(bfs_queue);
    free(bfs_queue); bfs_queue = NULL;
    free(vertexes); vertexes = NULL;
}

static void set_path(vertex* vertexes, matrix_t* matrix, size_t i)
{
    while (vertexes[i].index != 0)
    {
        m_get(matrix, vertexes[i].pos.y, vertexes[i].pos.x)->symbol = PATH;
        i = vertexes[i].prev;
    }
}

static void set_neighbours(dir_t* dirs, position_t* neigs, tile_t* tiles, dir_t d, position_t* pos, matrix_t* matrix)
{
    *dirs = turn_left(d);
    *(dirs + 1) = d;
    *(dirs + 2) = turn_right(d);

    *neigs = move(*pos, turn_left(d));
    *(neigs + 1) = move(*pos, d);
    *(neigs + 2) = move(*pos, turn_right(d));

    *tiles = get_tile(matrix, neigs[0]);
    *(tiles + 1) = get_tile(matrix, neigs[1]);
    *(tiles + 2) = get_tile(matrix, neigs[2]);
}

bool solve_maze(maze_t *maze, matrix_t* matrix, position_t start)
{
    queue_t* bfs_queue = malloc(sizeof(queue_t));
    if (bfs_queue == NULL)
    {
        perror("couldn't allocate memory for queue\n");
        return false;
    }
    queue_init(bfs_queue);

    size_t n = 0;
    size_t space_for_vertexes = 50;
    vertex* vertexes = malloc(sizeof(vertex) * 50);
    if (vertexes == NULL)
    {
        perror("couldn't allocate memory for vertexes\n");
        free(bfs_queue);
        return false;
    }

    vertexes[0] = (vertex){.pos=start, .d=maze->solve_start_dir, .prev=0, .index=0};

    queue_insert(bfs_queue, (node_t){NULL, 0});
    set_found(matrix, *(maze->fst_ent));

    while (!queue_is_empty(bfs_queue))
    {
        size_t index = queue_pop(bfs_queue).index;
        position_t pos = vertexes[index].pos;
        dir_t d = vertexes[index].d;

        if (pos_is_equal(pos, *(maze->snd_ent)))
        {
            set_path(vertexes, matrix, index);
            free_resources_solve(bfs_queue, vertexes);
            return true;
        }
        set_found(matrix, pos);

        dir_t dirs[3]; position_t neigs[3]; tile_t tiles[3];
        set_neighbours(dirs, neigs, tiles, d, &pos, matrix);

        for (int i = 0; i < 3; i++)
        {
            if (is_found(matrix, neigs[i]))
            {
                continue;
            }
            if (tiles[i].symbol == SPACE || tiles[i].symbol == ENTRANCE)
            {
                n++;
                if (n >= space_for_vertexes)
                {
                    space_for_vertexes += 50;
                    vertex* temp = (vertex*)realloc(vertexes, space_for_vertexes * sizeof(vertex));
                    if (temp == NULL)
                    {
                        perror("couldn't allocate memory for vertexes\n");
                        free_resources_solve(bfs_queue, vertexes);
                        return false;
                    }
                    vertexes = temp;
                }
                vertexes[n] = (vertex){.pos=neigs[i], .d=dirs[i], .prev=index, .index=n};
                queue_insert(bfs_queue, (node_t){NULL, n});
            }
        }
    }
    perror("no path found\n");
    free_resources_solve(bfs_queue, vertexes);
    return false;
}
