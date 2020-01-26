#include <stdio.h>
#include <stdlib.h> /*for system()*/

int main()
{

    int a[256] = { 1 };

    int idx = 0;

    if (fork() == 0) {
        while (idx < 100) {
            printf("a is: %i in one process\n", a[idx]);
            if (idx % 10 == 0)
                system("clear");
            ++idx;
        }
    } else {
        while (idx < 100) {
            printf("a is: %i in another\n", a[idx]);
            if (idx % 10 == 0)
                system("clear");
            ++idx;
        }
    }

    return 0;
}
