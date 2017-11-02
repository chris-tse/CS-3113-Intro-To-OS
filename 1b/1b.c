#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#define BUFFER_SIZE 1024                     // max line buffer
#define MAX_ARGS 64                          // max num of args
#define SEPARATORS " \t\n"                   // token separators

extern char **environ;                   // environment array

int isIOOp(char *arg);
void printDir(char **args);
void forkexec(char **argv);

int main(int argc, char **argv)
{
    // initialize buffers and other arrays
    char buf[BUFFER_SIZE];                   // input buffer array
    char *args[MAX_ARGS];                    // pointer to tokenized input array
    char **arg;                              // pointer to loop through args
    char *prompt = "amadeus > ";             // prompt text
    char dir[BUFFER_SIZE];                   // buffer for dir command

    char *clr[] = {"clear"};                 // clear terminal before taking in input
    forkexec(clr);

    // continue reading input until quit command or redirected input source ends
    while(!feof(stdin))
    {

        fputs(prompt, stdout);               // prompt user for commands

        if (fgets(buf, BUFFER_SIZE, stdin))  // read in a line of command
        {

            arg = args;
            *arg++ = strtok(buf, SEPARATORS);                // tokenize and put into args array

            while( (*arg++ = strtok(NULL, SEPARATORS)) );    // fill the remaining space in args with NULL

            if (args[0])
            {
                if (!strcmp(args[0], "clr")) // clear command
                {
                    forkexec(clr);         // use system "clear" command
                    continue;                // skip to next loop iteration
                }

                if (!strcmp(args[0], "dir")) // dir command
                {
                    printDir(args);
                    continue;
                }

                if (!strcmp(args[0], "cd"))
                {
                    if (args[1] == NULL)
                    {
                        printf("%s\n", getenv("PWD"));
                        continue;
                    }

                    else
                    {
                        chdir(args[1]);
                        setenv("PWD", args[1], 1);
                    }
                }

                if (!strcmp(args[0], "environ")) // environ command
                {
                    // use pointer arithmetic to step through
                    char **env = environ;
                    while (*env)
                        printf("%s\n", *env++);

                    continue;
                }

                if (!strcmp(args[0], "quit")) // quit command
                    break;                    // break out of while input loop

                // if none of the above, fork and exec the command
                forkexec(args);
            }
        }
    }
    exit(0);
}

/*
    Returns 1 if arg is any of "<", ">", or ">>"
    else returns 0
*/
int isIOOp(char* arg)
{
    return !strcmp(arg, "<") || !strcmp(arg, ">") || !strcmp(arg, ">>");
}

/*
    Takes in original args array and appends argument for dir command
    to "ls -al " string to be tokenzied and passed into execvp
*/
void printDir(char **args)
{
    char lsbuf[BUFFER_SIZE];                   // ls buffer for command string
    char *lsargs[MAX_ARGS];                    // pointer to tokenized ls command array
    char **lsarg;                              // pointer to loop through lsargs

    char ls[] = "ls -al ";                     // initialize default ls command string

    lsarg = lsargs;
    strcpy(lsbuf, ls);


    if (args[1] != NULL)
        strcat(lsbuf, args[1]);                // append argument after dir to ls command string if any
    else
        strcat(lsbuf, ".");                    // else append . for current directory

    *lsarg++ = strtok(lsbuf, SEPARATORS);
    while( (*lsarg++ = strtok(NULL, SEPARATORS)) );
    forkexec(lsargs);
}


/*
    Performs a fork and execvp with the original args array passed in
*/
void forkexec(char **argv)
{
    pid_t childPid;

    switch(childPid = fork())
    {
        case -1:
            // If fork fails for any reason, print an error message
            // and break, returning to the input reading loop
            perror("Fork error");
            break;
        case 0:
            // child process executes other commands with execvp
            // then exits
            execvp(argv[0], argv);
            exit(EXIT_SUCCESS);
        default:
            // parent process will wait for child process to exit
            // before returning to input reading loop
            waitpid(childPid, NULL, WUNTRACED);
    }
}