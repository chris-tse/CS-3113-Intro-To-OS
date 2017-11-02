# Project 1b: Roll Your Own Shell (Part 2)

An extension of Project 1a. This one expands upon the original shell. The main updates are:
1. Replace the use of `system()` with `fork()` and `exec()`
2. Implement ability to change directories with `cd <directory>` through the use of `chdir()`, `setenv()`, and `putenv()`
3. Implement IO redirection with the `<`, `>`, and `>>` operators

- `clr` - clear the screen
- `dir <directory>` - list the current directory contents
- `cd <directory>` - change to specified directory
- `environ` - list all environment strings
- `quit` - exits the program

