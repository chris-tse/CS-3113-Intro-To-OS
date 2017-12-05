#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 100                      // max line buffer
#define MAX_TOKENS 3                         // max line buffer
#define SEPARATORS " \t\n"                   // token separators

// tree node struct for buddy system
typedef struct Node {
    char *label;
    long int size;
    struct Node *left, *right;
} Node;

typedef struct Queue {
    Node *treenode;
    struct Queue *next; 
} Queue;
Queue *front = NULL;
Queue *rear = NULL; 

typedef enum {BESTFIT, FIRSTFIT, NEXTFIT, BUDDY} mode;

void mainLoop(mode algmode, char **mem, FILE *script);
void fill_array(char **arr, long int size, char *str);
void print_array(char **arr, long int maxsize);

long best_fit_allocate(char **mem, long int size, char *label);
long first_fit_allocate(char **mem, long int size, char *label);
long next_fit_allocate(char **mem, long int size, char *label);
long buddy_allocate(char **mem, long int size, char* label);

long* release(char **mem, char *label);


char **nextfitptr;

int main(int argc, char** argv)
{
    
    if (argc < 4)
    {
        printf("Error: Not enough arguments\n");
        printf("Usage: ./project2 [BESTFIT|FIRSTFIT|NEXTFIT] maxSize scriptfile\n");
        exit(EXIT_FAILURE);
    }
    
    char* eptr; // temp pointer
    long int maxsize = strtol(argv[2], &eptr, 10); // parse maxsize command line argument
    char **mem = malloc(sizeof(char *) * maxsize + 1);
    fill_array(mem, maxsize, "NULL");
    print_array(mem, maxsize);
    // Node *root = malloc(sizeof(Node));
    // root->label = "NULL";
    // root->size = maxsize;
    // root->left = NULL;
    // root->right = NULL;
    
    mode algmode;
    
    if (!strcmp(argv[1], "BESTFIT"))
        algmode = BESTFIT;
    else if (!strcmp(argv[1], "FIRSTFIT"))
        algmode = FIRSTFIT;
    else if (!strcmp(argv[1], "NEXTFIT"))
        algmode = NEXTFIT;
    else if (!strcmp(argv[1], "BUDDY"))
        algmode = BUDDY;
    else 
    {
        printf("Invalid allocation algorithm\n");
        exit(EXIT_FAILURE);
    }
    
    // check script file for existence and read permission
    // if no, exit program
    FILE* script;
    if (access(argv[3], F_OK) == -1)
    {
        printf("Error: %s does not exist\n", argv[3]);
        exit(EXIT_FAILURE);
    }
    else if (access(argv[3], R_OK) == -1)
    {
        printf("Error: cannot read from file %s\n", argv[3]);
        exit(EXIT_FAILURE);
    }
    else
    {
        script = fopen(argv[3], "r");            
    }
    
    mainLoop(algmode, mem, script);
    free(mem);
    exit(EXIT_SUCCESS);
}

void mainLoop(mode algmode, char **mem, FILE *script)
{
    printf("Start main loop\n");
    char buf[BUFFER_SIZE];
    char *tokens[MAX_TOKENS];
    char **token;
    
    long (*allocate)(char **mem, long int size, char *label);
    
    if (algmode == FIRSTFIT)
        allocate = first_fit_allocate;
    // else if (algmode == FIRSTFIT)
    //     allocate = first_fit_allocate;
    // else if (algmode == NEXTFIT)
    //     allocate = next_fit_allocate;
    // else
    //     allocate = buddy_allocate;
    while(!feof(script))
    {
        if (fgets(buf, BUFFER_SIZE, script))
        {
            token = tokens;
            *token++ = strtok(buf, SEPARATORS);
            while ((*token++ = strtok(NULL,SEPARATORS)));
            
            char **currBlockLoc = mem;
            if (tokens[0])
            {
                if (!strcmp(tokens[0], "LIST"))
                {
                    if (!strcmp(tokens[1], "AVAILABLE"))
                    {
                        
                        
                        char *currLabel = NULL;
                        char *prevLabel = "NOTHING";
                        
                        currLabel = *currBlockLoc;
                        
                    }
                    else if (!strcmp(tokens[1], "ASSIGNED"))
                    {
                        while(*currBlockLoc)
                        {
                            printf("Current block label: %s, location: %ld\n", *currBlockLoc, currBlockLoc - mem);
                            currBlockLoc++;
                        }
                    }
                    else
                    {
                        printf("Invalid command\n");
                    }
                }
                
                else if (!strcmp(tokens[0], "REQUEST"))
                {
                    if (tokens[1] && tokens[2])
                    {
                        char *eptr;
                        long int size = strtol(tokens[2], &eptr, 10);
                        //printf("Allocating label %s with size %ld\n", tokens[1], size);
                        long res = allocate(mem, size, tokens[1]);
                        if (res < 0)
                            printf("FAIL REQUEST %s %ld\n", tokens[1], size);
                        else 
                        {
                            printf("ALLOCATED %s %ld\n", tokens[1], (char **)res + size - mem);
                        }
                    }
                    else
                        printf("INVALID REQUEST\n");
                }
                else if (!strcmp(tokens[0], "RELEASE"))
                {
                    if (tokens[1])
                    {
                        long *res = release(mem, tokens[1]);
                        if (res[0] > 0)
                            printf("FREE %s %ld %ld\n", tokens[1], res[0], res[1]);
                        else
                            printf("FAIL RELEASE %s\n", tokens[1]);
                    }
                }
            }
        }
    }
    
}

void fill_array(char **arr, long int size, char *str)
{
    
    for (int i=0; i<size; i++) {
        arr[i] = malloc(sizeof(char) * 16);
        if (str)
        {
            arr[i] = strdup(str);
        }
        else
        {
            arr[i] = "\0";
        }
    }
    arr[size] = NULL;
}

void print_array(char **arr, long int maxsize)
{
    char **ptr = arr;
    long int i = 0;
    while(*ptr) 
        printf("%s, %ld\n", *ptr++, i++);      
}

long first_fit_allocate(char **mem, long int size, char *label)
{
    char **currBlock = mem;
    char **freeSearch;
    int broke = 0;
    long ret = -1;
    
    while(*currBlock)
    {
        if (!strcmp(*currBlock, "NULL"))
        {
            freeSearch = currBlock;
            long int i = 0;
            while(i < size)
            {
                // go through and search if all "NULL"
                //printf("Checking block with label %s\n", *freeSearch);
                if (!(*freeSearch) || strcmp(*freeSearch, "NULL"))
                {
                    broke = 1;
                    break;
                }
                i++;
                freeSearch++;
            }
            if (!broke)
            {
                
                ret = (long) currBlock;
                //printf("Inserting %s at location %ld\n", label, ret - (long) mem);
                for (long int i = 0; i < size; i++, currBlock++)
                {
                    strcpy(*currBlock, label);
                }
                return ret;
            }
            else
                currBlock++;
        }
        else
            currBlock++;
    }
    return ret;
    
}

long* release(char **mem, char *label)
{
    char **currBlock = mem;
    long int *res = malloc(sizeof(long int) * 2);
    res[0] = 0;
    res[1] = -1;
    while(*currBlock)
    {
        if (!strcmp(*currBlock, label))
        {
            res[1] = res[1] < 0 ? currBlock - mem : res[1];
            res[0]++;
            strcpy(*currBlock, "NULL");
        }
        currBlock++;
    }
    return res;
}