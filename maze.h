#ifndef MAZE_H
#define MAZE_H

#include "maze.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

struct tile
{
    char value;
};

struct position
{
    int x, y;
};

struct maze
{
    size_t width;
    size_t height;
    struct position entrance;
    struct position exit;
    size_t num_walls;
    struct tile **tiles;
    size_t *line_lengths;
    size_t num_outer_walls;
};
bool maze_create(struct maze *maze, FILE *file);
void maze_destroy(struct maze *maze);
bool maze_is_within_bounds(struct maze *maze, struct position pos);
bool maze_get_tile(struct maze *maze, struct position pos, struct tile *tile);
bool maze_set_tile(struct maze *maze, struct position pos, struct tile tile);
void maze_get_adjacent_positions(struct position from, struct position adjacent_positions[4]);
bool maze_is_correct_col(struct maze *maze, struct position pos);
bool maze_is_correct_line(struct maze *maze, struct position pos);
void add_spaces(struct maze *maze);
void count_Llength(struct maze *maze);
bool is_valid(struct maze *maze);
bool bounds_overall(struct maze *maze, struct position pos);
bool is_connected(struct maze *maze);
#endif // MAZE_H
