/**
 *
 * Author: Daniel cender
 * Date: 01/23/2020
 *
 * Description:
 *
 *
 *
 *
 */
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h> /*for system()*/

/* The LONG_TIMEOUT will be used in the first thread process,
 then the first thread to encounter a currently locked mutex
 will timout for the duration of SHORT_TIMEOUT before trying again.
 In this way, it'll test whether the critical resource is available,
 but not with a busy-waiting lock.
*/
#define LONG_TIMEOUT 10 // this thread should hypothetically never go without the lock
#define SHORT_TIMEOUT 4 // threads will "try" twice while longer thread runs
#define NUM_THREADS 10 // set num of threads here
#define THREAD_ITERATIONS 5 // set iterations to run in each thread created

pthread_mutex_t* mutex = PTHREAD_MUTEX_INITIALIZER;
// uint64_t runsLeft[NUM_THREADS] = { THREAD_ITERATIONS };
pthread_t threads[NUM_THREADS] = { 0 };

uint64_t resource = 12345678910; // resource being "fought over"

/**
 * Basic implementation of Array.indexOf for int arrays
 */
int64_t indexOf(uint64_t* arr, uint64_t* el)
{
    for (uint64_t i = 0; i < NUM_THREADS; ++i) {
        if (arr[i] == el) {
            return i;
        }
    }
    return -1;
}

void* thread_func(void* args)
{
    uint64_t runsLeft = THREAD_ITERATIONS;
    uint64_t pid = pthread_self();
    int64_t pidIndex = indexOf(threads, pid);

    while (1 && runsLeft != 0) {
        if (pthread_mutex_trylock(mutex) == 0) {
            // critical region
            printf("Thread %i on my %i iteration:\n", pidIndex, runsLeft - THREAD_ITERATIONS);
            printf("Printing resource: %i\n\n###\n\n", resource);
            if (pid == threads[0]) {
                // First thread runs longer timeout while keeping mutex locked
                sleep(LONG_TIMEOUT);
            } else {
                // Might not need this actually
                sleep(SHORT_TIMEOUT);
            }
        } else {
            // set timeout for shorter processes
            sleep(SHORT_TIMEOUT);
        }
    }
    return NULL;
}

// TODO: Need thread 1 func - will take 15 sec
// void* thread_1(void* args)
// {
//     while (1) {
//         if (pthread_mutex_trylock(mutex) == 0) {
//             // critical region

//         } else {
//             // set timeout...
//             sleep(LONG_TIMEOUT);
//         }
//     }

//     sleep(1);
//     // Check mutex
// }

// TODO: Need thread 2 func - will take 3 sec
// void* thread_2(void* args)
// {
//     // check mutex
//     // If locked, set timeout
// }

// void* manage_threads(void* args)
// {
//     // In constant loop, check if any active threads
// }

// TODO: Need logger func - to keep the thread func's clean

int main()
{
    // Create threads
    for (int idx = 0; idx < NUM_THREADS; ++idx) {
        pthread_create(threads[idx], NULL, thread_func, NULL);
    }

    // Manage the threads - prune them and restart them as needed
    // manage_threads(); // TODO

    // Join threads to main
    for (int idx = 0; idx < NUM_THREADS; ++idx) {
        pthread_join(threads[idx], NULL);
    }

    return 0;
}
