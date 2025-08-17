# maze_solver

## Table of Contents
- [Introduction](#introduction)
- [Overview](#overview)
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)
- [Tests](#tests)
- [License](#license)
- [Author](#author)

## Introduction
This is a refactored version of one of my advanced C projects I made at
my university one year ago. The goal of this refactoring is to apply knowledge
I've learned in the past year and to get better at reading and understanding
"unfamiliar" code (one year is a long time).

## Overview
This is a command-line program which checks the maze format and then finds
the shortest path in it using
[BFS graph algorithm](https://en.wikipedia.org/wiki/Breadth-first_search).
Maze is made of `#` (walls), exactly 2 `X` (entrances) and ` ` (empty path).
It is provided to the program through a 'input' file. The path is then filled
in with `o` characters and the whole maze is stored into 'output' file. It also
removes excessive whitespace which is present in the input file.

Example:

For the following input file:
```txt
   
      
    ####X####
    #       #  
    # # ## ##
    # #  # #### 
    # ## # #  #       
    #  # # #  #
    ##   # # ##
    #  # # # ##
    # ##   # ##    
    # ###### X
    #  #  #  #
    #        #  
    ##########
      
                

```
it generates this output file:
```txt
####o####
#  oo   #
# #o## ##
# #oo# ####
# ##o# #  #
#  #o# #  #
##ooo# # ##
#oo# # # ##
#o##   # ##
#o######oo
#oo#  #oo#
# oooooo #
##########
```

## Maze format

The input file may only contain `#`, `X`, ` ` and `\n` A maze is defined as
the largest closed area bounded by # characters (the “fence”).
- The outer wall must form a continuous path of # symbols, containing exactly
two X symbols. Neighboring wall symbols of the fence must touch horizontally
or vertically (no gaps).
- Think of the fence as a snake from the old mobile game: the snake must form
a loop and bite its own tail.
- The two X symbols serve as entry/exit points and must be placed on the outer
wall, each aligned with exactly two opposite # symbols
(to ensure they’re passable).
- Only one maze is allowed in the file. No stray X or # symbols may exist
outside the maze. Spaces are ignored.

## Requirements

- GCC with C99 support
- make
- Tested on Linux (Fedora 42)

## Installation

```bash
git clone https://github.com/k4dlec4y/maze_solver
cd maze_solver
make
```

## Usage

To only check maze format, run the program as:
```bash
./build/maze check INPUT_FILE
```
If the maze has valid format, nothing is printed and the program
returns zero. Otherwise, an error message is printed on `STDERR` and
non-zero value is returned.

To check and find the path as well, run:
```bash
./build/maze solve INPUT_FILE OUTPUT_FILE
```
If the shortest path was successfully found, the maze with
marked path is saved into `OUTPUT_FILE`, nothing is printed on the console and
the program returns zero. If the maze has invalid format or no valid path is
found, an error message is printed on `STDERR` and non-zero value is returned.
`OUTPUT_FILE` will not be created in this case. 

## Tests
To run cli tests, execute
```bash
./tests.sh
```

## License

This project is licensed under the MIT License – see the LICENSE file for details.

## Author

Created by Matúš Kadlecay - https://github.com/k4dlec4y
