# Project 2
Christopher Tse

## Description
This program simulates four different memory allocation algorithms discussed in class: first-fit, next-fit, best-fit, and buddy system. 

## Getting Started

### Installation

1. Clone source code from repo if not on current machine  
`git clone https://github.com/chris-tse/CS-3113-Intro-To-OS/`

2. Navigate to project directory  
`cd project2`

3. Compile the project using make  
`make all`

4. Run the program

### Usage

To use this memory allocation simulator, run the compiled binary file with the appropriate command line arguments.

```
Usage: ./project2 SORTING_ALGORITHM ALLOCATION_SIZE SCRIPT_FILE
```

Example:
```
./project2 BESTFIT 1024 script.txt
```

## Command Options
For the choice of sorting algorithm, the following options are available:
- `FIRSTFIT`
- `NEXTFIT`
- `BESTFIT`
- `BUDDY`

When creating the script to simulate memory allocation, there are 5 commands that can be used:
- `REQUEST A n` - Allocates a process with the label A of size n
- `RELEASE A` - Finds and frees the memory allocated for a process with the label A
- `LIST AVAILABLE` - Prints a space separated list of pairs of space available `n` and the relative start location `x` in the form of `(n1, x1) (n2, x2) ...`
- `LIST ASSIGNED` - Prints a space separated list of tuples of process label `A`, size allocated `n`, and relative start position `x` in the form of `(A1, n1, x1)`
- `FIND A` - Prints a tuple of process label `A`, allocated size `n`, and relative start location `n`

A process label should not exceed 16 characters.

Here is an example of a script that allocates a process A with size 16, process B with size 10, and process C with size 6, then frees the space allocated for process B:

```
REQUEST A 16
REQUEST B 10
REQUEST C 6
RELEASE B
```
Output
```
ALLOCATE A 0
ALLOCATE B 16
ALLOCATE C 25
FREE B 10 16
```