#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFFER_SIZE 1024                     // max line buffer
#define MAX_ARGS 64                          // max num of args
#define SEPARATORS " \t\n"                   // token separators

extern char **environ;                   // environment array

int main(int argc, char **argv)
{
    // initialize buffers and other arrays
    char buf[BUFFER_SIZE];                   // input buffer array
    char *args[MAX_ARGS];                    // pointer to tokenized input array
    char **arg;                              // pointer to loop through args
    char *prompt = "amadeus > ";             // prompt text
    char dir[BUFFER_SIZE];                   // buffer for dir command

    system("clear");                         // clear terminal before taking in input

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
                    system("clear");         // use system "clear" command
                    continue;                // skip to next loop iteration
                }

                if (!strcmp(args[0], "dir")) // dir command
                {
                    if (args[1] == NULL)
                    {
                        system("ls -al");
                    }
                    else
                    {
                        strcpy(dir, "ls -al ");
                        strcat(dir, args[1]);
                        system(dir);
                    }
                    continue;
                }

                if (!strcmp(args[0], "environ"))
                {
                    // use pointer arithmetic to step through
                    char **env = environ;
                    while (*env)
                        printf("%s\n", *env++);

                    continue;
                }

                if (!strcmp(args[0], "quit")) // quit command
                    break;                    // break out of while input loop

                // if neither, print command
                arg = args;
                while(*arg)
                {
                    fputs(*arg++, stdout);
                    fputs(" ", stdout);
                }
                fputs("\n", stdout);
            }
        }
    }
    exit(0);
}