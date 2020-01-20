#include <stdio.h>
#include <stdlib.h>

int main()
{
	fork();
	printf("This is printed after the 1st fork\n");
	fork();
	printf("This is printed after the 2nd fork\n");
	exit(0);
}
