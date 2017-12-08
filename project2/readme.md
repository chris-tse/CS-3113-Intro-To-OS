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
./project2 SORTING_ALGORITHM ALLOCATION_SIZE SCRIPT_FILE
```

Example:
```
./project2 BESTFIT 1024 script.txt
```

The above command line arguments can be the following:
- `SORTING_ALGORITHM`
    - `FIRSTFIT`
    - `NEXTFIT`
    - `BESTFIT`
    - `BUDDY`