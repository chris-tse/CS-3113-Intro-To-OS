# AMADEUS Shell
v1.3

This is a C shell that mimics features from the popular bash shell, including the ability to execute batch files, redirect IO, and will fall back to the underlying system shell in the case where it does not recognize a command.

## Release History

- 0.3.0
    - Implemented ability to execute batch files with `./1c.out <filename>`
    - Implemented background execution of processes with the `&` indicator
    - Implemented `echo <string>` for echoing to the screen
    - I

- 0.2.0
    - Replaces all usage of `system()` to execute unknown commands to using `fork()` and `exec()`
    - Implemented `cd` command to check current directory and `cd <directory>` to change directories
    - Implemented IO redirection with `<`, `>`, and `>>` operators

- 0.1.0
    - Initial release with 4 simple commands: `clr`, `dir`, `environ`, and `quit`