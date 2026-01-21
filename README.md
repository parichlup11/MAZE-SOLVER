=========================MAZE SOLVER===========================

CLI tool written in C for analyzing, solving, and rendering 
ASCII-based mazes. The application implements the Breadth-First Search (BFS) 
algorithm to guarantee the shortest path between entry and exit points.

CORE FEATURES:
- Pathfinding: Calculates the shortest path using queue-based BFS.
- Validation: Performs structural integrity checks to ensure the maze is 
  fully enclosed and has valid entry/exit points.
- Visualization: Includes a custom BMP renderer to export mazes and solutions 
  as high-resolution bitmap images with customizable colors and scaling.
- Input/Output: Handles text-based maze definitions and generates solved 
  ASCII maps or binary image files.

BUILDING:
$ make

USAGE:
1. Validate a maze file:
   $ ./maze check input_example.txt

2. Solve the maze (ASCII output):
   $ ./maze solve input_example.txt output.txt
