/*
* Author:	Daniel cender
* Date:		01/23/2020
* Basic program that utilizes pthreads, pthread condition variables, and a mutex to
* produce a program that prints out the entire alphabet using multiple threads in sync.
*
* This implementation of a monitor is not what we might traditionally expect.
* The actual "monitor" construct in CS that utilizes condition variales to determine
* which process will run next in the program.
*
* In the case of this one, the external state being evaluated is the "users" variable.
*
* "Users" should always be 1 or 0, since we are running 2 threads.
* The practical use of a "monitor" check has us running a continual "while(users != 0)" check
* in our thread functions.
*
* Check out this paper for a great look into using monitor checks in C/C++
* http://pages.cs.wisc.edu/~remzi/OSTEP/threads-monitors.pdf
*
* Compile and run this program with:
* ~ gcc -o Monitors Monitors.c -lpthread
* ~ ./Monitors
*/

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#define ALPHA_LENGTH 26

char alphabet[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };
int idx;

struct MONITOR {
    int users; // count of the users on the resource
    pthread_mutex_t monitor; // monitor lock | mutex
    pthread_cond_t firstBlockSig; // conditional variables, signal threads to check
    pthread_cond_t secondBlockSig;
};

struct MONITOR monitor;

// Below are thread functions copied over from Semaphores project,
// split into two separate functions which can process any index of the alphabet array when
// they lay claim to the semaphore

void* thread_1(void* arg)
{
    while (idx < ALPHA_LENGTH) {
        // "monitor"-type check against mutex and conditional variables
        pthread_mutex_lock(&(monitor.monitor));
        while (monitor.users != 0) {
            pthread_cond_wait(&(monitor.firstBlockSig), &(monitor.monitor));
        }
        monitor.users++;
        /***/
        printf("\nThread 1 %d, Entered...\n", pthread_self());
        printf("%c\n", alphabet[idx]);
        printf("Index at: %i", idx);
        ++idx; // increment index
        printf("\nJust Exiting...\n");
        /***/
        // Decrementing this int condition is invaluable to keeping the other
        // thread waiting until the current one is truly finished and releases the lock
        monitor.users--;
        pthread_cond_signal(&(monitor.secondBlockSig));
        pthread_mutex_unlock(&(monitor.monitor));
    }
    return NULL;
}

// This thread contains a loop that would potentially run
// at the same time as thread 1, without the monitor construct
void* thread_2(void* arg)
{
    while (idx < ALPHA_LENGTH) {
        // "monitor"-type check against mutex and conditional variables
        pthread_mutex_lock(&(monitor.monitor));
        while (monitor.users != 0) {
            pthread_cond_wait(&(monitor.secondBlockSig), &(monitor.monitor));
        }
        monitor.users++;
        /***/
        printf("\nThread 2 %d, Entered...\n", pthread_self());
        printf("%c\n", alphabet[idx]);
        printf("Index at: %i", idx);
        ++idx; // increment index
        printf("\nJust Exiting...\n");
        /***/
        // Phase out of the critical section
        monitor.users--;
        pthread_cond_signal(&(monitor.firstBlockSig));
        pthread_mutex_unlock(&(monitor.monitor));
    }
    return NULL;
}

int main()
{
    idx = 0;
    pthread_t th1, th2;
    pthread_create(&th1, NULL, thread_1, NULL);
    pthread_create(&th2, NULL, thread_2, NULL);
    pthread_join(th1, NULL);
    pthread_join(th2, NULL);

    return 0;
}
