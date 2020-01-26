/**
 *
 * Author: Daniel cender
 * Date: 01/25/2020
 *
 * Description: This program illustrates how using a timeout to restart threads
 *  to avoid deadlock is a dangerous way to potentially have your program forever
 *  awaiting conditions that will never be met.
 *
 * In this program, we're setting up the scenario where the process at
 * threads[0] runs for longer than the other threads we're spinning up.
 *
 * The short and long timeouts have been set to reflect a particular scenario.
 * With SHORT_TIMEOUT == 4 and the LONG_TIMEOUT == 10, we get a situation where
 * our shorter-running threads will run twice before the longer thread finishes it's first
 * run. It'll then start another 10second run before our shorter threads can lock the mutex.
 *
 * So, in this case, our shorter threads never get access to their critical region of code, and their timeouts aren't intelligent enough to know they are futile.
 *
 * Improvements: If I were to improve upon this so that all the threads could
 *  gain access to the coveted resource, I would set a backoff for the sleep timeout.
 *  If the timeout increased by 1 second every time, there'd be an eventual overlap
 *  with the gap between the longer running thread's runs, so the starved thread could
 *  hopefully lock the mutex in that time. Better yet, I'd implement a busy wait
 *  instead of the current check, even though that requires more processing power.
 *  A spinning lock would knab the resource at some point, but would likely still
 *  distribute resource access unfairly.
 *
 * Compiled and ran with:
 *  ~ gcc -o Starvation Starvation.c -lpthread
 *  ~ ./Starvation
 *
 * Logs found in: /log.md
 *
 */
#include <limits.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t threads[NUM_THREADS] = { 0 };
uint64_t resource = 12345; // resource being "fought over"
FILE* fptr;

char* cwd[PATH_MAX];

/**
 * Basic implementation of Array.indexOf for int arrays
 */
int64_t indexOf(pthread_t arr[], pthread_t el)
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
    uint64_t nbrTimesAccessingResource = 0;
    pthread_t pid = pthread_self();
    int64_t pidIndex = indexOf(threads, pid);

    while (runsLeft > 0) {
        if (pthread_mutex_trylock(&mutex) == 0) {
            if (pid == threads[0]) {
                printf("Longer running thread now in control...\n\n");
                fprintf(fptr, "_Longer running thread now in control..._\n\n");
            }
            // critical region
            printf("Thread %lli on its %llu iteration:\n", pidIndex + 1, (THREAD_ITERATIONS - runsLeft) + 1);
            fprintf(fptr, "_Thread %lli on its %llu iteration:_\n\n", pidIndex + 1, (THREAD_ITERATIONS - runsLeft) + 1);
            printf("Printing resource: %llu\n", resource);
            fprintf(fptr, "- __Printing resource: %llu__\n\n", resource);

            // decrement remaining runs
            --runsLeft;
            // add to count of times this thread got to access the critical region
            ++nbrTimesAccessingResource;

            if (pid == threads[0]) {
                // First thread runs longer timeout while keeping mutex locked
                sleep(LONG_TIMEOUT);
            } else {
                // Might not need this actually
                sleep(SHORT_TIMEOUT);
            }

            // give up mutex lock
            if (pthread_mutex_unlock(&mutex) != 0) {
                perror("Couldn't unlock mutex!\n");
                pthread_exit(&pid);
            }
        } else {
            printf("Thread %lli putting itself to sleep on its %i run\n", pidIndex + 1, (THREAD_ITERATIONS - runsLeft) + 1);
            fprintf(fptr, "_Thread %lli putting itself to sleep on its %i run_\n\n", pidIndex + 1, (THREAD_ITERATIONS - runsLeft) + 1);
            // Also, decrement here: some threads need to starve so others can eat
            --runsLeft;
            // set timeout for shorter processes
            sleep(SHORT_TIMEOUT);
        }
    }


    // log all final values
    printf("\n---\n### Thread %lli final results:\n\n__Thread %lli got to run %llu times.__\n\n__Thread %lli got to access RESOURCE %llu times__\n\n---\n", pidIndex + 1, pidIndex + 1, THREAD_ITERATIONS - runsLeft, pidIndex + 1, nbrTimesAccessingResource);
    fprintf(fptr, "\n---\n### Thread %lli final results:\n\n__Thread %lli got to run %llu times.__\n\n__Thread %lli got to access RESOURCE %llu times__\n\n---\n", pidIndex + 1, pidIndex + 1, THREAD_ITERATIONS - runsLeft, pidIndex + 1, nbrTimesAccessingResource);

    return NULL;
}

int main()
{
    // Get current working directory
    // Ref: https://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd() error");
        return 0;
    }

    // Open/create new log file - will re-write file on every run
    if ((fptr = fopen(strcat(cwd, "/log.md"), "w")) == NULL) {
        perror("File could not be opened or created!");
        return 0;
    }

    fprintf(fptr, "## Thread Starvation Program Log File\n\n");

    // Create threads
    for (uint64_t idx = 0; idx < NUM_THREADS; ++idx) {
        pthread_create(&(threads[idx]), NULL, thread_func, NULL);
    }

    // Join threads to main
    for (uint64_t idx = 0; idx < NUM_THREADS; ++idx) {
        pthread_join(threads[idx], NULL);
    }

    return 0;
}
