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
void printDir(char **args, int redirectin, char *intarget, int redirectout, char *outtarget);
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
        int save_out = dup(fileno(stdout));
        int out;
        int redirectin = 0;
        int redirectout = 0;
        char *outtarget;
        char *intarget;
        FILE *tmpout;
        FILE *tmpin;
        if (fgets(buf, BUFFER_SIZE, stdin))  // read in a line of command
        {

            arg = args;
            *arg++ = strtok(buf, SEPARATORS);                // tokenize and put into args array

            while( (*arg++ = strtok(NULL, SEPARATORS)) );    // fill the remaining space in args with NULL
			if (args[0])
            {
				arg = args;
                // check for redirection operators
				while(*arg)
				{
					int res = isIOOp(*arg);

					switch (res)
					{
						case 0:
							redirectin = 1;
							intarget = *++arg;
							arg--;
							break;

						case 1:
							redirectout = 1;
							outtarget = *++arg;
							arg--;
							break;
						case 2:
							redirectout = 2;
							outtarget = *++arg;
							arg--;
							break;
						default:
							break;
					}	
					arg++;
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

                if (!strcmp(args[0], "dir")) // dir command
                {
                    printDir(args, redirectin, intarget, redirectout, outtarget);
                    continue;
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
                forkexec(args, 0, NULL, 0, NULL);
            }
        }
        fflush(stdout);
        close(out);
        dup2(save_out, fileno(stdout));
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
void printDir(char **args, int redirectin, char *intarget, int redirectout, char *outtarget)
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

    *lsarg++ = strtok(lsbuf, SEPARATORS);
    while( (*lsarg++ = strtok(NULL, SEPARATORS)) );
    forkexec(lsargs, redirectin, intarget, redirectout, outtarget);
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
            if (redirectin)
                freopen(intarget, "r", stdin);
            if (redirectout == 1)
                freopen(outtarget, "w", stdout);
            if (redirectout == 2)
                freopen(outtarget, "a", stdout);
            execvp(argv[0], argv);
            exit(EXIT_SUCCESS);
        default:
            // parent process will wait for child process to exit
            // before returning to input reading loop
            waitpid(childPid, NULL, WUNTRACED);
    }
}
