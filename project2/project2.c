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
    long size;
    long startPos;
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
void fill_array(char **arr, long size, char *str);
void print_array(char **arr, long maxsize);

long best_fit_allocate(char **mem, long size, char *label);
long first_fit_allocate(char **mem, long size, char *label);
long next_fit_allocate(char **mem, long size, char *label);
long buddy_allocate(char **mem, long size, char* label);
long* release(char **mem, char *label);
void find(char **mem, char *label);


long nextfitindex;
long memsize;
Node *root;

int main(int argc, char** argv)
{
    
    if (argc < 4)
    {
        printf("Error: Not enough arguments\n");
        printf("Usage: ./project2 [BESTFIT|FIRSTFIT|NEXTFIT] maxSize scriptfile\n");
        exit(EXIT_FAILURE);
    }
    
    char* eptr; // temp pointer
    memsize = strtol(argv[2], &eptr, 10); // parse memsize command line argument
    char **mem = malloc(sizeof(char *) * memsize + 1);
    fill_array(mem, memsize, "NULL");
    // print_array(mem, memsize);
    // Node *root = malloc(sizeof(Node));
    // root->label = "NULL";
    // root->size = memsize;
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
    {
        algmode = BUDDY; 
    }
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
    //printf("Start main loop\n");
    char buf[BUFFER_SIZE];
    char *tokens[MAX_TOKENS];
    char **token;
    nextfitindex = 0;
    long (*allocate)(char **mem, long size, char *label);
    
    if (algmode == FIRSTFIT)
        allocate = first_fit_allocate;
    else if (algmode == BESTFIT)
        allocate = best_fit_allocate;
    else if (algmode == NEXTFIT)
        allocate = next_fit_allocate;
    else
    {
        // allocate = buddy_allocate;
        printf("Sorry, buddy algorithm not implemented\n");
        exit(EXIT_FAILURE);
    }
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
                        
                        for (int i = 0; i < memsize; i++)
                        {
                            if (!strcmp(mem[i], "NULL"))
                            {
                                long currSize = 0;
                                long currPos = i;
                                while (i < memsize && !strcmp(mem[i], "NULL"))
                                {
                                    currSize++;
                                    i++;
                                }
                                printf("(%ld, %ld) ", currSize, currPos);
                            }
                        }  
                        putchar('\n'); 
                    }
                    else if (!strcmp(tokens[1], "ASSIGNED"))
                    {
                        int wentIn = 0;
                        for (int i = 0; i < memsize; i++)
                        {
                            if (wentIn) i--;
                            wentIn = 0;
                            if (strcmp(mem[i], "NULL"))
                            {
                                char *currLabel = malloc(sizeof(char) * 16);
                                strcpy(currLabel, mem[i]);
                                long currPos = i;
                                long size = 0;
                                while(strcmp(mem[i], "NULL") && !strcmp(mem[i], currLabel))
                                {
                                    wentIn = 1;
                                    size++;
                                    i++;
                                }
                                printf("(%s, %ld, %ld) ", mem[currPos], size, currPos);
                            }
                        }
                        putchar('\n');
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
                        long size = strtol(tokens[2], &eptr, 10);
                        //printf("Allocating label %s with size %ld\n", tokens[1], size);
                        long res = allocate(mem, size, tokens[1]);
                        if (res < 0)
                            printf("FAIL REQUEST %s %ld\n", tokens[1], size);
                        else 
                        {
                            printf("ALLOCATED %s %ld\n", tokens[1], res);
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
                        if (res[1] < 0)
                            printf("FAIL RELEASE %s\n", tokens[1]);
                        else
                            printf("FREE %s %ld %ld\n", tokens[1], res[0], res[1]);
                    }
                }
                else if(!strcmp(tokens[0], "PRINT"))
                {
                    print_array(mem, memsize);
                }
                else if (!strcmp(tokens[0], "FIND"))
                {
                    if (tokens[1])
                    {
                        find(mem, tokens[1]);
                    }
                }
            }
        }
    }
    
}

void fill_array(char **arr, long size, char *str)
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

void print_array(char **arr, long maxsize)
{
    char **ptr = arr;
    long i = 0;
    while(*ptr) 
        printf("%s, %ld\n", *ptr++, i++);      
}

long first_fit_allocate(char **mem, long size, char *label)
{
    for (int i = 0; i < memsize; i++)
    {
        if (!strcmp(mem[i], "NULL") && size + i < memsize)
        {
            long available = 0;
            for (int j = i; j < size + i; j++)
            {
                if (!strcmp(mem[i], "NULL")) {
                    available++;
                    if (available >= size) break;
                }
                else 
                    break;
            }
            if (available >= size)
            {
                for (int j = i; j < size + i; j++)
                    strcpy(mem[j], label);
                
                return i;
            }
        }
    }

    return -1;
    
}

long next_fit_allocate(char **mem, long size, char *label)
{
    //printf("Start next fit\n");
    int startedAt = nextfitindex;
    // printf("Start from current nextfitindex %ld to end\n", nextfitindex);
    for (;nextfitindex < memsize; nextfitindex++)
    {
        // printf("current nextfitindex %ld is %s\n", nextfitindex, mem[nextfitindex]);
        // printf("size + nextfitindex < memsize: %d\n", size + nextfitindex <= memsize);
        if (!strcmp(mem[nextfitindex], "NULL") && size + nextfitindex <= memsize)
        {
            // printf("Found \"NULL\"\n");
            long available = 0;
            for (int i = nextfitindex; i <= size + nextfitindex; i++)
            { 
                // printf("size + nextfitindex: %ld\n", size + nextfitindex);  
                //printf("Comparing %s\n", mem[i]);
                if (!strcmp(mem[i], "NULL")) {
                    available++;
                    if (available >= size) break;
                }
                else {
                    printf("BROKE\n");
                    break;
                }
            }
            if (available >= size)
            {
                for (int j = nextfitindex; j < size + nextfitindex; j++)
                    strcpy(mem[j], label);
                
                return nextfitindex;
            }
        }
    }
    //printf("Start from 0 to startedAt\n");
    for (nextfitindex = 0; nextfitindex < startedAt; nextfitindex++)
    {
        //printf("current nextfitindex %ld is %s\n", nextfitindex, mem[nextfitindex]);
        //printf("size + nextfitindex < memsize: %d\n", size + nextfitindex <= memsize);
        if (!strcmp(mem[nextfitindex], "NULL") && size + nextfitindex <= memsize)
        {
            //printf("Found \"NULL\"\n");
            long available = 0;
            for (int i = nextfitindex; i <= size + nextfitindex; i++)
            {
                if (!strcmp(mem[i], "NULL")) available++;
                else break;
            }
            // printf("Available found: %ld\n", available);
            if (available >= size)
            {
                for (int j = nextfitindex; j < size + nextfitindex; j++)
                    strcpy(mem[j], label);

                return nextfitindex;
            }
            else return -1;
        }
    }
    printf("Finished both loops\n");
    return -1;
}

long best_fit_allocate(char **mem, long size, char *label)
{   
    long smallestBlockIndex = -1, smallestBlockSize = 0;
    for (int i = 0; i < memsize; i++)
    {
        if (!strcmp(mem[i], "NULL"))
        {
            long currBlockIndex = i, currBlockSize = 0;

            while (!strcmp(mem[i], "NULL"))
            {
                currBlockSize++;
                if (i == memsize -1) break;
                i++;
            }
            if (smallestBlockIndex < 0 || smallestBlockSize > currBlockSize) 
            {
                smallestBlockIndex = currBlockIndex;
                smallestBlockSize = currBlockSize;
            }
        }
    }

    for (int i = smallestBlockIndex; i < size + smallestBlockIndex; i++)
        strcpy(mem[i], label);
    
    return smallestBlockIndex;
}

// long buddy_allocate (char** mem, long size, char *label)
// {
//     if (root->left == NULL && size <= root->size/2)
//     {
//         root->left = malloc(sizeof(Node));
//         root->right = malloc(sizeof(Node));
//         root->left->label = "NULL";
//         root->left->size = root->size/2;
//         root->left->startPos = root->startPos;
//         root->left->label = "NULL";
        
//     }
// }


long* release(char **mem, char *label)
{
    long *res = malloc(sizeof(long) * 2);
    res[0] = 0;
    res[1] = -1;
    for (int i = 0; i < memsize; i++)
    {
        if (!strcmp(mem[i], label))
        {
            res[1] = i;
            for (int j = i; j < memsize; j++)
            {
                if (strcmp(mem[j], label)) break;
                res[0]++;
                strcpy(mem[j], "NULL");
            }
            //printf("Returning res: [%ld, %ld]\n", res[0], res[1]);
            return res;
        }
    }
    //printf("Exited loop\n");
    //printf("Returning res: [%ld, %ld]\n", res[0], res[1]);
    return res;


    // while(*currBlock)
    // {
    //     if (!strcmp(*currBlock, label))
    //     {
    //         res[1] = res[1] < 0 ? currBlock - mem : res[1];
    //         res[0]++;
    //         strcpy(*currBlock, "NULL");
    //     }
    //     currBlock++;
    // }
    // return res;
}

void find(char **mem, char *label)
{
    long size = 0;
    long startPos = -1;
    
    for (int i = 0; i < memsize; i++)
    {
        if (!strcmp(mem[i], label))
        {
            if (startPos < 0) startPos = i;
            size++;
        }
    }
    
    if (startPos >= 0) 
        printf("(%s, %ld, %ld)\n", label, size, startPos);
    else 
        printf("Could not find process %s\n", label);
        
    return;
}