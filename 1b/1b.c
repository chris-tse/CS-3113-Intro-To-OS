#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024                     // max line buffer
#define MAX_ARGS 64                          // max num of args
#define SEPARATORS " \t\n"                   // token separators

extern char **environ;                   // environment array

int isIOOp(char *arg);
void printDir(char **args, int redirectout, char *outtarget);
void printEnv(int redirectout, char *outtarget);
void forkexec(char **argv, int redirectin, char *intarget, int redirectout, char *outtarget);

int main(int argc, char **argv)
{
    // initialize buffers and other arrays
    char buf[BUFFER_SIZE];                   // input buffer array
    char *args[MAX_ARGS];                    // pointer to tokenized input array
    char **arg;                              // pointer to loop through args
    char *prompt = "amadeus > ";             // prompt text
    char dir[BUFFER_SIZE];                   // buffer for dir command

    char *clr[] = {"clear"};                 // clear terminal before taking in input
    forkexec(clr, 0, NULL, 0, NULL);

    // continue reading input until quit command or redirected input source ends
    while(!feof(stdin))
    {

        fputs(prompt, stdout);               // prompt user for commands

        int redirectin = 0;                  // flag for stdin redirection
        int redirectout = 0;                 // flag for stdout redirection
        char *outtarget;                     // string for stdout redirection src
        char *intarget;                      // string for stdin redirection src

        int firstIO = -1;

        if (fgets(buf, BUFFER_SIZE, stdin))  // read in a line of command
        {

            arg = args;
            *arg++ = strtok(buf, SEPARATORS);                // tokenize and put into args array
            while( (*arg++ = strtok(NULL, SEPARATORS)) );    // fill the remaining space in args with NULL

            if (args[0])
            {
                arg = args;
                int i = 0;

                while(*arg)                                     // loop through args array and check for IO redirectors
                {
                    int res = isIOOp(*arg);                     // check whether current arg is IO operator

                    switch (res)
                    {
                        case 0:                                 // for each case
                            if (firstIO < 0)                    // if firstIO is -1, mark this arg as first IO operator
                                firstIO = i;
                            redirectin = 1;                     // set appropriate flag
                            intarget = *++arg;                  // set next arg as the target
                            arg++;                              // skip to next arg after target
                            break;

                        case 1:
                            if (firstIO < 0)
                                firstIO = i;
                            redirectout = 1;
                            outtarget = *++arg;
                            arg++;
                            break;
                        case 2:
                            if (firstIO < 0)
                                firstIO = i;
                            redirectout = 2;
                            outtarget = *++arg;
                            arg++;
                            break;
                        default:                                // if not an IO operator
                            arg++;                              // move to next arg
                            break;
                    }
                    i++;
                }

                if (!strcmp(args[0], "clr")) // clear command
                {
                    forkexec(clr, 0, NULL, 0, NULL);         // use clr command array from above
                    continue;
                }

                if (!strcmp(args[0], "cd"))              // cd command
                {
                    if (args[1] == NULL)                 // check if additional arguments are present
                    {
                        printf("%s\n", getenv("PWD"));   // print PWD if no
                    }
                    else
                    {
                        chdir(args[1]);                  // otherwise change process directory
                        char pathbuf[BUFFER_SIZE];       // and copy cwd into a buffer
                        getcwd(pathbuf, BUFFER_SIZE);
                        setenv("PWD", pathbuf, 1);       // set PWD to buffer containing cwd
                    }
                    continue;
                }

                if (!strcmp(args[0], "dir"))                   // dir command
                {
                    printDir(args, redirectout, outtarget);    // call printDir
                    continue;
                }

                if (!strcmp(args[0], "environ")) // environ command
                {
                    // use pointer arithmetic to step through
                    printEnv(redirectout, outtarget);
                    continue;

                }

                if (!strcmp(args[0], "quit")) // quit command
                    break;                    // break out of while input loop

                // if none of the above commands
                if (firstIO >= 0)           // first check for presence of IO operators
                {
                    // if yes, create new arg array and initialize all to NULL
                    char *newArgs[MAX_ARGS] = { NULL };
                    // copy contents of original args array to new array up to before first IO operator
                    for (int i = 0; i < firstIO; i++)
                    {
                        newArgs[i] = args[i];
                    }
                    // forkexec new args array
                    forkexec(newArgs, redirectin, intarget, redirectout, outtarget);
                }
                else  // if not IO simply forkexec original args
                {
                    forkexec(args, redirectin, intarget, redirectout, outtarget);
                }
            }
        }
    }
    exit(0);
}

/*
    Returns 0 if <
            1 if >
            2 if >>
    else returns -1
*/
int isIOOp(char *arg)
{
    if (arg == NULL)
        return -1;
    if (!strcmp(arg, "<"))
        return 0;
    if (!strcmp(arg, ">"))
        return 1;
    if (!strcmp(arg, ">>"))
        return 2;
    return -1;
}

/*
    Takes in original args array and appends argument for dir command
    to "ls -al " string to be tokenzied and passed into execvp
*/
void printDir(char **args, int redirectout, char *outtarget)
{
    char lsbuf[BUFFER_SIZE];                   // ls buffer for command string
    char *lsargs[MAX_ARGS];                    // pointer to tokenized ls command array
    char **lsarg;                              // pointer to loop through lsargs

    char ls[] = "ls -al ";                     // initialize default ls command string

    lsarg = lsargs;
    strcpy(lsbuf, ls);


    if (args[1] != NULL && isIOOp(args[1]) < 0)
        strcat(lsbuf, args[1]);                // append argument after dir to ls command string if any
    else
        strcat(lsbuf, ".");                    // else append . for current directory

    *lsarg++ = strtok(lsbuf, SEPARATORS);                // tokenize ls command string
    while( (*lsarg++ = strtok(NULL, SEPARATORS)) );      // fill rest with NULL
    forkexec(lsargs, 0, NULL, redirectout, outtarget);   // forkexec ls command array
}

/*
    Forks and executes the original environ command code in the child.
*/
void printEnv(int redirectout, char *outtarget)
{
    pid_t childPid;

    switch (childPid = fork())
    {
        case -1:
            perror("Fork error");
            break;
        case 0:                                                                   // in child
            if (access(outtarget, F_OK) == -1)                                    // check whether file exists, create if doesn't
            {
                FILE* out = fopen(outtarget, "w");
                fclose(out);
            }
            if (redirectout == 1 || redirectout == 2)                            // if redirectout flag is on
            {
                if (access(outtarget, W_OK) == 0)                                // check for write access then set desired mode
                    freopen(outtarget, redirectout == 1 ? "w" : "a", stdout);    // according to flag
                else
                {
                    fprintf(stderr, "%s: cannot write to file\n", outtarget);   // else print to stderr
                    return;
                }
            }

            // use pointer arithmetic to loop through environ
            char **env = environ;
            while (*env)
                printf("%s\n", *env++);
            exit(EXIT_SUCCESS);
        default:
            waitpid(childPid, NULL, WUNTRACED);
    }

}


/*
    Performs a fork and execvp with the original args array passed in
*/
void forkexec(char **argv, int redirectin, char *intarget, int redirectout, char *outtarget)
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
            if (redirectin)                                                     // if redirect in flag on
            {
                if (access(intarget, F_OK | R_OK) == 0)                         // if file exists and can be read from, redirect stdin
                    freopen(intarget, "r", stdin);
                else if (access(intarget, F_OK) == -1)                         // if file doesn't exist, print to stderr and return
                {
                    fprintf(stderr, "%s: file does not exist\n", intarget);
                    return;
                }
                else                                                           // if can't read from file, print to stderr and return
                {
                    fprintf(stderr, "%s: cannot read from file\n", intarget);
                    return;
                }
            }
            if (redirectout == 1 || redirectout == 2)                             // if redirectout flag triggered
            {
                if (access(outtarget, F_OK) == -1)                               // check if file exists, create if not
                {
                    FILE* out = fopen(outtarget, "w");
                    fclose(out);
                }

                if (access(outtarget, W_OK) == 0)                               // if write access OK, redirect stdout
                    freopen(outtarget, redirectout == 1 ? "w" : "a", stdout);   // set mode according to flags
                else                                                            // else print to stderr and return
                {
                    fprintf(stderr, "%s: cannot write to file\n", outtarget);
                    return;
                }
            }

            execvp(argv[0], argv);
            exit(EXIT_SUCCESS);
        default:
            // parent process will wait for child process to exit
            // before returning to input reading loop
            waitpid(childPid, NULL, WUNTRACED);
    }
}
