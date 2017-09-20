#include <stdio.h>
#include <stdlib.h>

void myprint(int row, int size, char k);

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		printf("Argc is %d\n", argc);
		fprintf(stderr, "Usage: diamond [size] [char]\n");
		return 1;
	}

	int num = atoi(argv[1]);

	if (num < 1 || num > 15)
	{
		fprintf(stderr, "Error: Number must be between 1 and 15\n");
		exit(-1);
	}

	if (num % 2 == 0)
	{
		fprintf(stderr, "Error: Number must be odd\n");
		exit(-1);
	}

	int size = num * 2 - 1;
	int length = num - 1;

	char *k = argv[2];;

	for (int row = 0; row < size/2; row++)
	{
		myprint(row, size, *k);
		printf("\n");
	}

	for (int i = 0; i < size; i++) {
		printf("%c", *k);
	}
	printf("\n");

	for (int row = size/2 - 1; row >= 0; row--)
	{
		myprint(row, size, *k);
		printf("\n");
	}
	return 0;
}

void myprint(int row, int size, char k)
{
	for (int col = 0; col < size; col++)
	{
		if (row + col < size/2 || col - row > size/2)
		{
			printf(" ");
		}
		else
		{
			printf("%c", k);
		}
	}
}