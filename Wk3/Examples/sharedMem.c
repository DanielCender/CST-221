#include <stdio.h>

int main()
{

int a[256] = {1};

if(fork() == 0) {
	printf("a is: %i in one process\n", a[0]);
} else {
	printf("a is: %i in another\n", a[0]);
}

return 0;
}