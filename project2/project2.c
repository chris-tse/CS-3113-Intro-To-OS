#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// tree node struct for buddy system
typedef struct {
    int value;
    int size;
    struct node *left;
    struct node *right;
} node;

int main(int argc, char** argv)
{
    enum mode {ARRAY, TREE};
    if (argc < 4)
    {
        printf("Error: Not enough arguments\n");
        printf("Usage: ./project2 [BESTFIT|FIRSTFIT|NEXTFIT] maxSize scriptfile\n");
        exit(EXIT_FAILURE);
    }
    
    char* eptr; // temp pointer
    long int maxsize = strtol(argv[2], &eptr, 10); // parse maxsize command line argument
    int *arrMem; // array pointer for fit algorithms
    node *root; // root node for buddy system
    enum mode memmode;
    
    if (!strcmp(argv[1], "BESTFIT"))
    {
        // use best fit
        arrMem = malloc(sizeof(int) * maxsize);
        memmode = ARRAY;
    }
    else if (!strcmp(argv[1], "FIRSTFIT"))
    {
        // use first fit
        arrMem = malloc(sizeof(int) * maxsize);
        memmode = ARRAY;
    }
    else if (!strcmp(argv[1], "NEXTFIT"))
    {
        // use next fit
        arrMem = malloc(sizeof(int) * maxsize);
        memmode = ARRAY;
    }
    else if (!strcmp(argv[1], "BUDDY"))
    {
        // initialize root node values
        root->size = maxsize;
        root->value = 1;
        root->left = NULL;
        root->right = NULL;
        
        memmode = TREE;
    }
    else 
    {
        printf("Invalid allocation algorithm\n");
        exit(EXIT_FAILURE);
    }
}