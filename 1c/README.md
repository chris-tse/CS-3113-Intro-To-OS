# AMADEUS Shell
v1.3

This is a C shell that mimics features from the popular bash shell, including the ability to execute batch files, redirect IO, and falls back to the underlying system shell in the case where it does not recognize a command.

## Requirements

The project was developed on:

- `make` >= 3.81
- `gcc` >= 6.3

Problems may occur if versions used are older than stated above.


## Installation

1. Clone the repository<br>
`git clone https://github.com/chris-tse/CS-3113-Intro-To-OS`

2. Go into the directory of the most recent version<br>
`cd 1c`

3. Compile the project with `make`<br>
`make`

## Usage

### Getting Started
To run the shell, simply execute the compiled binary.
```
$ ./1c
```
The shell can also execute batch files which can be passed in as a command line argument.
```
$ ./1c batchfile
```
### Commands
Here is a list of all currently implemented commands. Any commands not on this list are passed to the system shell.

- `help` - displays a readme with commands and usage help
- `clr` - clears the screen
- `dir` - prints the current directory
- `dir <directory>` - lists the contents of specified directory. If no directory is given, this command prints the contents of the current directory
- `cd <directory>` - navigates to the specified directory. If no directory is given, this command prints the current working directory
- `environ` - prints out the current environment variables
- `echo <text>` - prints out the given text onto the screen
- `pause` - displays the text "Press Enter to continue..." and waits for user to press Enter before continuing execution
- `quit` - exits the shell

### Background Execution
A process can be executed in the background using the `&` operator. Instead of waiting for execution of current command to finish before returning to prompt, it immediately returns to prompt without execution finishing.

Example:
```
amadeus> sleep 2 &
amadeus>          // did not wait 2 seconds before appearing
```

### Input/Output Redirection

These operators change the behaviour of the input and output of the shell.

#### Redirecting Input
To redirect input from a separate file, use the `<` operator.

Example:
```
amadeus> cat < greetings.txt
Welcome user
```

#### Redirecting Output (Overwrite)
To redirect output and overwrite contents at destination, use the `>` operator.

Example:
```
amadeus> cat textfile
This is some text
amadeus> dir > textfile
amadeus> cat textfile
total 136
drwxr-xr-x  14 christophertse  staff    476 Nov 27 23:17 .
drwxr-xr-x  12 christophertse  staff    408 Nov 16 14:26 ..
... // rest of dir output
```

#### Redirecting Output (Append)
To redirect output and append to the destination file, use the `>>` operator.

Example:
```
amadeus> cat greetings.txt >> newgreet
amadeus> cat greetings.txt >> newgreet
amadeus> cat newgreet
Welcome user
Welcome user
```

*Note that the `echo` command in the current implementation of the AMADEUS shell does not redirect output, and will instead print the operator to the screen as text

## Release History

- 0.3.0
    - Implemented ability to execute batch files with `./1c.out <filename>`
    - Implemented background execution of processes with the `&` indicator
    - Implemented `echo <string>` for echoing to the screen
    - Implemented `pause` which prompts for user to press `Enter` before continuing execution (useful for batch file execution)


- 0.2.0
    - Replaces all usage of `system()` to execute unknown commands to using `fork()` and `exec()`
    - Implemented `cd` command to check current directory and `cd <directory>` to change directories
    - Implemented IO redirection with `<`, `>`, and `>>` operators

- 0.1.0
    - Initial release with 4 simple commands: `clr`, `dir`, `environ`, and `quit`