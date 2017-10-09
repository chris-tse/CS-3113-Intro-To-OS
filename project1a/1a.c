#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUFFER_SIZE 80
#define MAX_ARGS 64
#define SEPARATORS " \t\n"

int main(void)
{
    // initialize buffers and other arrays
    char buf[BUFFER_SIZE];                // input buffer array
    char *args[MAX_ARGS];                 // pointer to tokenized input array
    char **arg;                           // pointer to loop through args
    char *prompt = "1ash > ";             // prompt text

    while(!feof(stdin))
    {
        fputs(prompt, stdout);

        if (fgets(buf, BUFFER_SIZE, stdin))
        {
            arg = args;
            *arg++ = strtok(buf, SEPARATORS);

            while( (*arg++ = strtok(NULL, SEPARATORS)) );

            if (args[0])
            {
                if (!strcmp(args[0], "clr")) // clear command
                {
                    system("clear");
                    continue;
                }

                if (!strcmp(args[0], "quit")) // quit command
                    break;

                // if neither, print command
                arg = args;
                while(*arg) fputs(*arg++, stdout);
                fputs("\n", stdout);
            }
        }
    }
    exit(0);
}