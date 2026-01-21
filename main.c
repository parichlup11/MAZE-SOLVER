#include "maze.h"
#include "queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UNUSED(X) ((void) (X))

/*
 * Finds the shortest path from entrance to exit using BFS.
 * Marks the path with 'o' characters in the maze structure.
 * Returns true if a path is found, false otherwise.
 */
bool solve_maze(struct maze *maze)
{
    assert(maze != NULL);
    struct queue queue_struct;
    queue_init(&queue_struct);

    // Arrays to keep track of visited nodes and path reconstruction
    bool visited[maze->height][maze->width];
    struct position predecessors[maze->height][maze->width];

    // Mark entrance as part of the path initially
    maze->tiles[maze->entrance.y][maze->entrance.x].value = 'o';

    // Initialize visited array and predecessors
    for (size_t i = 0; i < maze->height; i++) {
        for (size_t j = 0; j < maze->width; j++) {
            visited[i][j] = false;
            predecessors[i][j].x = -1;
            predecessors[i][j].y = -1;
        }
    }

    visited[maze->entrance.y][maze->entrance.x] = true;
    queue_insert(&queue_struct, (struct node){ .pos = maze->entrance, .parent = NULL });

    // Direction vectors for moving Up, Right, Down, Left
    int dx[4] = { 0, 1, 0, -1 };
    int dy[4] = { -1, 0, 1, 0 };

    while (!queue_is_empty(&queue_struct)) {
        struct node current = queue_pop(&queue_struct);

        // Check if we reached the exit
        if (current.pos.x == maze->exit.x && current.pos.y == maze->exit.y) {
            struct position pos = current.pos;
            
            // Backtrack from exit to entrance to mark the path
            while (pos.x != maze->entrance.x || pos.y != maze->entrance.y) {
                maze->tiles[pos.y][pos.x].value = 'o';
                pos = predecessors[pos.y][pos.x];
            }
            queue_free(&queue_struct);
            return true;
        }

        // Explore neighbors
        for (int i = 0; i < 4; i++) {
            struct position next = { current.pos.x + dx[i], current.pos.y + dy[i] };
            
            if (maze_is_within_bounds(maze, next) && 
                maze->tiles[next.y][next.x].value != '#' && 
                !visited[next.y][next.x]) {
                
                visited[next.y][next.x] = true;
                predecessors[next.y][next.x] = current.pos;
                queue_insert(&queue_struct, (struct node){ .pos = next, .parent = &current });
            }
        }
    }
    
    queue_free(&queue_struct);
    return false;
}

/*
 * Prints the solved maze to the specified output file.
 * Handles trimming of leading empty columns to match the assignment format.
 */
void maze_print(struct maze *maze, FILE *output_file)
{
    assert(maze != NULL);
    assert(output_file != NULL);
    
    // Find the leftmost column that contains actual maze content (not just spaces)
    size_t leftmost_non_space_col = maze->width;

    for (size_t i = 0; i < maze->height; i++) {
        for (size_t j = 0; j < maze->width; j++) {
            if (maze->tiles[i][j].value != ' ') {
                if (j < leftmost_non_space_col) {
                    leftmost_non_space_col = j;
                }
                break;
            }
        }
    }
    
    // Update line lengths to ensure correct trimming from the right
    count_Llength(maze);

    for (size_t i = 0; i < maze->height; i++) {
        // Start printing from the first useful column
        for (size_t j = leftmost_non_space_col; j < maze->line_lengths[i]; j++) {
            if (maze->tiles[i][j].value == '\n') {
                continue;
            }
            fprintf(output_file, "%c", maze->tiles[i][j].value);
        }
        fprintf(output_file, "\n");
    }
}

/*
 * Main entry point. Handles arguments and switches between 'check' and 'solve' modes.
 */
int main(int argc, char *argv[])
{
    // Argument count check (minimal check, logic mostly relies on argv[1])
    if (argc < 3) {
        fprintf(stderr, "Usage: ./maze check INPUT_FILE\n");
        fprintf(stderr, "       ./maze solve INPUT_FILE OUTPUT_FILE\n");
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "check") == 0) {
        /* --- CHECK MODE --- */
        FILE *file = fopen(argv[2], "r");
        if (file == NULL) {
            fprintf(stderr, "Error: Cannot open input file.\n");
            return EXIT_FAILURE;
        }

        struct maze maze;
        if (!maze_create(&maze, file)) {
            fprintf(stderr, "Error: Invalid maze.\n");
            fclose(file);
            maze_destroy(&maze);
            return EXIT_FAILURE;
        }

        fprintf(stdout,"Maze is OK.\n");
        fclose(file);
        maze_destroy(&maze);
        
    } else if (strcmp(argv[1], "solve") == 0) {
        /* --- SOLVE MODE --- */
        if (argc < 4) {
             fprintf(stderr, "Error: Missing output file argument.\n");
             return EXIT_FAILURE;
        }

        FILE *input_file = fopen(argv[2], "r");
        if (!input_file) {
            fprintf(stderr, "Error: Cannot open input file.\n");
            return EXIT_FAILURE;
        }

        struct maze maze;
        if (!maze_create(&maze, input_file)) {
            fprintf(stderr, "Error: Invalid maze.\n");
            fclose(input_file);
            maze_destroy(&maze); 
            return EXIT_FAILURE;
        }
        fclose(input_file);

        if (solve_maze(&maze)) {
            FILE *output_file = fopen(argv[3], "w");
            if (!output_file) {
                fprintf(stderr, "Error: Cannot create output file.\n");
                maze_destroy(&maze);
                return EXIT_FAILURE;
            }
            maze_print(&maze, output_file);
            fclose(output_file);
        } else {
            fprintf(stderr, "Error: No solution found.\n");
            maze_destroy(&maze);
            return EXIT_FAILURE; // Should technically exit with failure if not solvable?
        }

        maze_destroy(&maze);
        
    } else {
        /* --- INVALID COMMAND --- */
        fprintf(stderr, "Usage: ./maze check INPUT_FILE\n");
        fprintf(stderr, "       ./maze solve INPUT_FILE OUTPUT_FILE\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}