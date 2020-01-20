#include <stdio.h>

main()
{

	int turn = 1;

	fork();

	// turn = 1;
	printf("Printing in process: %i", turn);

	if (fork() >= 0)
	{
		int turn = 0;
		printf("Var turn: %i", turn);
	}
	printf("Var turn outside if: %i", turn);
}
