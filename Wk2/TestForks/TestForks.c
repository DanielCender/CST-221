#include <stdio.h>
main()
{
	int example;
	int process = fork();

	if (process > 0)
	{
		example = 1; // In parent process
	}
	else if (process == 0)
	{
		example = 3; // In child process
	}
	else
	{
		example = process; // Failed to create process
	}

	printf("Here is my variable: %i", example);
}
