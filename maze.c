#include "maze.h"

#include "queue.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void add_spaces(struct maze *maze)
{
    // fills the rest of the line with spaces   
    assert(maze != NULL);
    for (size_t y = 0; y < maze->height; y++) {
        int width = maze->width;
        int line_length = maze->line_lengths[y];
        if (line_length < width) {

            // allocating new line with correct width
            struct tile *new_line = (struct tile *) malloc(maze->width * sizeof(struct tile));
            if (new_line == NULL) {
                fprintf(stderr,"Memory allocation error\n");
                return;
            }
            // padding with spaces
            for (size_t x = 0; x < maze->line_lengths[y]; x++) {
                new_line[x].value = maze->tiles[y][x].value;
            }
            for (size_t x = line_length; x < maze->width; x++) {
                new_line[x].value = ' ';
            }
            free(maze->tiles[y]);
            maze->tiles[y] = new_line;
        }
    }
}

void count_Llength(struct maze *maze)
{
    // updates line length ignoring trailing spaces
    assert(maze != NULL);
    for (size_t i = 0; i < maze->height; i++) {
        size_t j = maze->width - 1;
        while (j > 0 && maze->tiles[i][j].value == ' ') {
            j--;
        }
        maze->line_lengths[i] = j + 1;
    }
}


bool maze_is_within_bounds(struct maze *maze, struct position pos)
{
    // checks if pos is inside map limits
    assert(maze != NULL);
    int width = maze->width;
    int height = maze->height;
    int line_length = maze->line_lengths[pos.y];
    return pos.x >= 0 && pos.x <= width - 1 && pos.y >= 0 && pos.y <= height - 1 && pos.x < line_length;
}

bool bounds_overall(struct maze *maze, struct position pos)
{
    assert(maze != NULL);
    int width = maze->width;
    int height = maze->height;
    return pos.x >= 0 && pos.x <= width - 1 && pos.y >= 0 && pos.y <= height - 1;
}

bool maze_is_correct_col(struct maze *maze, struct position pos)
{
    assert(maze != NULL);
    int width = maze->width;

    return pos.x >= 0 && pos.x <= width - 1;
}

bool maze_is_correct_line(struct maze *maze, struct position pos)
{
    assert(maze != NULL);
    int height = maze->height;
    return pos.y >= 0 && pos.y <= height - 1;
}

bool maze_get_tile(struct maze *maze, struct position pos, struct tile *tile)
{
    // gets tile value if coordinates are valid
    assert(maze != NULL);
    int width = maze->width;
    int height = maze->height;

    bool correct = pos.x >= 0 && pos.x <= width - 1 && pos.y >= 0 && pos.y <= height - 1;
    if (!correct) {
        return false;
    }
    *tile = maze->tiles[pos.y][pos.x];
    return true;
}

bool maze_set_tile(struct maze *maze, struct position pos, struct tile tile)
{
    assert(maze != NULL);
    if (!maze_is_within_bounds(maze, pos)){
        return false;
    }
    maze->tiles[pos.y][pos.x] = tile;
    return true;
}

void maze_get_adjacent_positions(struct position from, struct position adjacent_positions[4])
{
    // calculates coordinates for up, down, left, right
    assert(adjacent_positions != NULL);
    int dx[4] = { 0, 1, 0, -1 };
    int dy[4] = { -1, 0, 1, 0 };

    for (int i = 0; i < 4; i++) {
        adjacent_positions[i].x = from.x + dx[i];
        adjacent_positions[i].y = from.y + dy[i];
    }
}

bool is_valid_entrance(struct maze *maze)
{
    // checks neighbors to see if entrance is valid
    assert(maze != NULL);

    struct position left = { maze->entrance.x - 1, maze->entrance.y };
    struct position right = { maze->entrance.x + 1, maze->entrance.y };
    struct position up = { maze->entrance.x, maze->entrance.y - 1 };
    struct position down = { maze->entrance.x, maze->entrance.y + 1 };

    bool left_ = false;
    bool right_ = false;
    bool up_ = false;
    bool down_ = false;
    
    // checks walls around entrance
    if (maze_is_correct_col(maze, left) && maze->tiles[left.y][left.x].value == '#') {
        left_ = true;
    }
    if (maze_is_correct_col(maze, right) && maze->tiles[right.y][right.x].value == '#') {
        right_ = true;
    }
    if (maze_is_correct_line(maze, up) && maze->tiles[up.y][up.x].value == '#') {
        up_ = true;
    }
    if (maze_is_correct_line(maze, down) && maze->tiles[down.y][down.x].value == '#') {
        down_ = true;
    }

    return (((left_ && right_) && (!down_ && !up_)) || ((down_ && up_) && (!left_ && !right_)));
}

bool is_valid_exit(struct maze *maze)
{
    assert(maze != NULL);
    struct position left = { maze->exit.x - 1, maze->exit.y };
    struct position right = { maze->exit.x + 1, maze->exit.y };
    struct position up = { maze->exit.x, maze->exit.y - 1 };
    struct position down = { maze->exit.x, maze->exit.y + 1 };

    bool left_ = false;
    bool right_ = false;
    bool up_ = false;
    bool down_ = false;

    
    if (maze_is_correct_col(maze, left) && maze->tiles[left.y][left.x].value == '#') {
        left_ = true;
    }
    if (maze_is_correct_col(maze, right) && maze->tiles[right.y][right.x].value == '#') {
        right_ = true;
    }
    if (maze_is_correct_line(maze, up) && maze->tiles[up.y][up.x].value == '#') {
        up_ = true;
    }
    if (maze_is_correct_line(maze, down) && maze->tiles[down.y][down.x].value == '#') {
        down_ = true;
    }

    return (((left_ && right_) && (!down_ && !up_)) || ((down_ && up_) && (!left_ && !right_)));
}

bool is_connected(struct maze *maze)
{
    // alloc visited array
    assert(maze != NULL);
    bool **visited = (bool **) malloc(maze->height * sizeof(bool *));
    if (visited == NULL)
    {
        fprintf(stderr, "memory allocation failed\n");
        return false;
    }
    for (size_t i = 0; i < maze->height; ++i) {
        visited[i] = (bool *) malloc(maze->width * sizeof(bool));
        if(visited[i] == NULL)
        {
            for(size_t j = 0; j < i; j++)
            {
                free(visited[j]);
            }
            fprintf(stderr,"memory allocation failed");
            free(visited);
            return false;
        }
        memset(visited[i], false, maze->width * sizeof(bool));
    }
    

    struct queue q;
    queue_init(&q); // init queue for BFS

    struct position start_pos = { 0, 0 };
    bool found_start_pos = false;
    // find first wall or X to start BFS
    for (size_t y = 0; y < maze->height; ++y) {
        for (size_t x = 0; x < maze->line_lengths[y]; ++x) {
            if (maze->tiles[y][x].value == '#' || maze->tiles[y][x].value == 'X') {
                start_pos.x = x;
                start_pos.y = y;
                found_start_pos = true;
                break;
            }
        }
        if (found_start_pos) {
            break;
        }
    }

    if (!found_start_pos) {
        for (size_t i = 0; i < maze->height; ++i) {
            free(visited[i]);
        }
        free(visited);
        return true;
    }

    struct position adjacent_positions[4];
    queue_insert(&q, (struct node){ .pos = start_pos, .next = NULL, .parent = NULL });
    visited[start_pos.y][start_pos.x] = true;

    while (!queue_is_empty(&q)) {
        // processing queue nodes
        struct node current_node = queue_pop(&q);
        struct position current_pos = current_node.pos;
        maze_get_adjacent_positions(current_pos, adjacent_positions);

        for (int i = 0; i < 4; ++i) {
            // check neighbors and add to queue
            struct position adjacent_pos = adjacent_positions[i];
            if (bounds_overall(maze, adjacent_pos)
                    && (maze->tiles[adjacent_pos.y][adjacent_pos.x].value == '#' 
                    || maze->tiles[adjacent_pos.y][adjacent_pos.x].value == 'X') 
                    && !visited[adjacent_pos.y][adjacent_pos.x]) {
                if (!queue_insert(&q, (struct node){ .pos = adjacent_pos, .next = NULL, .parent = NULL })) {
                    fprintf(stderr, "queue insert failed\n");
                    for (size_t i = 0; i < maze->height; ++i) {
                        free(visited[i]);
                    }
                    free(visited);
                    queue_free(&q);
                    return false; 
                }
                visited[adjacent_pos.y][adjacent_pos.x] = true;
            }
        }
    }

    // check if we visited all walls
    for (size_t y = 0; y < maze->height; ++y) {
        for (size_t x = 0; x < maze->line_lengths[y]; ++x) {
            if ((maze->tiles[y][x].value == '#' || maze->tiles[y][x].value == 'X') &&
                    !visited[y][x]) {
                for (size_t i = 0; i < maze->height; ++i) {
                    free(visited[i]);
                }
                free(visited);
                queue_free(&q);
                return false; // found unvisited wall
            }
        }
    }

    // clean up memory
    for (size_t i = 0; i < maze->height; ++i) {
        free(visited[i]);
    }
    free(visited);
    queue_free(&q);
    return true;
}

bool col_alone_wall(struct maze *maze)
{
    assert(maze != NULL);
    for (size_t x = 0; x < maze->width; x++) 
    {
        int32_t counter_hash = 0;
        int32_t counter_X = 0;
        for (size_t y = 0; y < maze->height; y++) 
        {
            if (maze->tiles[y][x].value == '#') {
                counter_hash += 1;
            }
            if (maze->tiles[y][x].value == 'X') {
                counter_X += 1;
            }
        }
        if (counter_hash == 1 && counter_X != 1) {
            fprintf(stderr,"alone col\n");
            return false;
        }
    }
    return true;
}

bool is_valid(struct maze *maze)
{
    // counts walls globally
    assert(maze != NULL);
    int32_t hash_count_overall = 0;
    for (size_t y = 0; y < maze->height; y++) {
        int32_t hash_count = 0;

        int32_t x_count = 0;
        for (size_t x = 0; x < maze->width; x++) {
            
            struct position current_pos = { x, y };
            struct tile current_tile;

            

            if (!maze_get_tile(maze, current_pos, &current_tile)) {
                fprintf(stderr,"tile reading error");
                return false;
            }

            if (current_tile.value == '#') {
                hash_count += 1;
                hash_count_overall += 1;
                // checks surroundings of the wall
                struct position left = { current_pos.x - 1, current_pos.y };
                struct position right = { current_pos.x + 1, current_pos.y };
                struct position up = { current_pos.x, current_pos.y - 1 };
                struct position down = { current_pos.x, current_pos.y + 1 };
                bool left_check = false;
                bool right_check = false;
                bool up_check = false;
                bool down_check = false;
                
                if (maze_is_correct_col(maze, left) && maze->tiles[left.y][left.x].value == '#') {
                    left_check = true;
                }
                if (maze_is_correct_col(maze, right) && maze->tiles[right.y][right.x].value == '#') {
                    right_check = true;
                }
                if (maze_is_correct_line(maze, up) && maze->tiles[up.y][up.x].value == '#') {
                    up_check = true;
                }
                if (maze_is_correct_line(maze, down) && maze->tiles[down.y][down.x].value == '#') {
                    down_check = true;
                }
                if (!left_check && !right_check && !up_check && !down_check) {
                    return false;
                }
            }
            if (current_tile.value == 'X') { // count entrances
                x_count += 1;
            }
        }
        maze->num_walls = hash_count_overall;
        if (hash_count == 1 && x_count != 1) {
            fprintf(stderr, "only one line#\n");
            return false;
        }
    }

    // running all specific checks
    add_spaces(maze);
    count_Llength(maze);
    if (!is_valid_entrance(maze)) {
        fprintf(stderr,"invalid entrance\n");
        return false;
    }
    if (!is_valid_exit(maze)) {
        fprintf(stderr,"invalid exit\n");
        return false;
    }
    if (!is_connected(maze)) {
        fprintf(stderr,"not connected\n");
        return false;
    };
    if (!col_alone_wall(maze)) {
        fprintf(stderr,"alone col\n");
        return false;
    }
    return true;
}

bool initialize_maze_buffers(struct maze *maze, size_t buffer_size)
{
    maze->tiles = (struct tile **) malloc(buffer_size * sizeof(struct tile *));
    if (maze->tiles == NULL) {
        return false;
    }

    maze->line_lengths = (size_t *) malloc(buffer_size * sizeof(size_t));
    if (maze->line_lengths == NULL) {
        free(maze->tiles);
        return false;
    }
    return true;
}

bool maze_create(struct maze *maze, FILE *file)
{
    // buffer for reading lines
    assert(maze != NULL);
    assert(file != NULL);
    char *buffer = NULL;
    size_t buffer_size = 128;
    size_t line_length;
    size_t leftmost_wall = 0;
    size_t rightmost_wall = 0;

    buffer = (char *) malloc(buffer_size);
    if (buffer == NULL) {
        return false;
    }

    // alloc maze arrays
     if (!initialize_maze_buffers(maze, buffer_size)) {
        free(buffer);
        return false;
    }

    maze->width = 0;
    maze->height = 0;
    int entrance_count = 0;

    // reading file line by line
    for (size_t y = 0; fgets(buffer, buffer_size, file) != NULL; y++) {
        line_length = strlen(buffer);

        // realloc if line is too long
        while (line_length == buffer_size - 1 && buffer[line_length - 1] != '\n') {
            buffer_size *= 2;
            char *buffer_new = (char *) realloc(buffer, buffer_size);
            if (buffer_new == NULL) {
                free(buffer);
                free(maze->tiles);
                free(maze->line_lengths);
                return false;
            }
            buffer = buffer_new;
            if (fgets(buffer + line_length, buffer_size - line_length, file) == NULL) {
                free(buffer);
                free(maze->tiles);
                free(maze->line_lengths);
                return false;
            }

            line_length = strlen(buffer);
        }

        // alloc row for tiles
        maze->tiles[y] = (struct tile *) malloc(line_length * sizeof(struct tile));
        if (maze->tiles[y] == NULL) {
            for (size_t x = 0; x < y; x++){
                free(maze->tiles[x]);
            }
            free(buffer);
            free(maze->tiles);
            free(maze->line_lengths);
            return false;
        }

        // validating allowed chars
        for (size_t x = 0; x < line_length; x++) {
            if (buffer[x] != '#' && buffer[x] != 'X' && buffer[x] != ' ' && buffer[x] != '\n') {
                free(buffer);
                free(maze->tiles);
                free(maze->line_lengths);
                return false;
            }
            maze->tiles[y][x].value = buffer[x];

            if (buffer[x] == '#') {
                if (x < leftmost_wall) {
                    leftmost_wall = x;
                }

                if (x > rightmost_wall) {
                    rightmost_wall = x;
                }
            }

            if (buffer[x] == 'X') {
                // save start and end positions
                if (entrance_count == 0) {
                    maze->entrance.x = x; //column
                    maze->entrance.y = y; //line
                } else if (entrance_count == 1) {
                    maze->exit.x = x;
                    maze->exit.y = y;
                }
                entrance_count++;
            }
        }
        
        // update max width
        if (line_length > maze->width) {
            maze->width = line_length;
        }
        maze->line_lengths[y] = line_length;
        maze->height++;
    }

    maze->width = rightmost_wall - leftmost_wall + 1;
    free(buffer);
    buffer = NULL;
    if (entrance_count != 2) {
        fprintf(stderr, "invalid amount of entrances");
        return false;
    }

    // final validity check
    if (!is_valid(maze)) {
        fprintf(stderr, "false valid\n");
        return false;
    }
    return true;
}

void maze_destroy(struct maze *maze)
{
    assert(maze != NULL);
    if (maze->tiles != NULL) {
        for (size_t y = 0; y <= maze->height - 1; y++) {
            free(maze->tiles[y]);
        }
    }
    
    // free all allocated memory
    free(maze->tiles);
    free(maze->line_lengths);
    maze->width = 0;
    maze->height = 0;
    maze->entrance.x = 0;
    maze->entrance.y = 0;
    maze->exit.x = 0;
    maze->exit.y = 0;
    maze->num_walls = 0;
    maze->tiles = NULL;
    maze = NULL;
}


