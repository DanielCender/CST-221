/*
* Author:	Daniel cender
* Date:		01/23/2020
* Basic program that utilizes pthreads and semaphores to
* produce a program that prints out the entire alphabet using multiple threads in sync.
*
*
*/

#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

// Named semaphores a necessity on some
// openBSD-based systems (i.e. OSX)
#define FIRST_SEM "/first-block-sem"
#define SECOND_SEM "/second-block-sem"

#define ALPHA_LENGTH 26
#define MID 13

char alphabet[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };
int idx;

struct MONITOR_MUTEXES {
    sem_t* runFirstBlock;
    sem_t* runSecondBlock;
    int users; // count of the users on the resource
    int resourceIsBusy; // "mutex" that signifies we may test sem
    int readyForResource; // alert for current resource user
};

struct MONITOR_MUTEXES monitor;

/**
 * Func initializes MONITOR_MUTEXES struct and
 * opens named semaphores
 */
int monitor_init()
{
    monitor.readyForResource = 0;
    monitor.resourceIsBusy = 0;
    monitor.users = 0;
    // Check that both semaphores are initialized
    if (((monitor.runFirstBlock) = sem_open(FIRST_SEM, O_CREAT, 0660, 1)) != SEM_FAILED && ((monitor.runSecondBlock) = sem_open(SECOND_SEM, O_CREAT, 0660, 1)) != SEM_FAILED) {
        return 0;
    } else {
        printf("Unable to init semaphores for monitor construct...");
    }
    return 1;
}

/**
 * Func that unlinks the semaphores used in monitor struct
 */
void monitor_destroy()
{
    sem_unlink(FIRST_SEM);
    sem_unlink(SECOND_SEM);
}

/**
 * Func that starts a synchronous operation.
 * Blocks operations in 2nd synchronous block through mutual exclusion.
 */
void monitor_startSyncBlock1()
{
    printf("Waiting for block 1 to start");
    if (monitor.resourceIsBusy || monitor.users != 0) {
        sem_wait(monitor.runFirstBlock);
    }
    monitor.resourceIsBusy++; // 1st block may start now
}

/**
 * Func that ends a synchronous operation.
 * Releases operations in 2nd synchronous block.
 */
void monitor_endSyncBlock1()
{
    printf("Ending block 1");
    // set this so check in monitor_startSyncBlock2 will eval == true
    monitor.resourceIsBusy--;
    if (monitor.readyForResource) {
        sem_post(monitor.runSecondBlock);
    } else {
        sem_post(monitor.runFirstBlock);
    }
}

/**
 * Func that starts a synchronous operation.
 * Blocks operations in 1st synchronous block through mutual exclusion.
 */
void monitor_startSyncBlock2()
{
    printf("Waiting for block 2 to start");
    if (monitor.resourceIsBusy) {
        monitor.readyForResource++;
        sem_wait(monitor.runSecondBlock);
        monitor.readyForResource--;
    }
    monitor.users++;
    sem_post(monitor.runSecondBlock);
}

/**
 * Func that starts a synchronous operation.
 * Blocks operations in 1st synchronous block through mutual exclusion.
 */
void monitor_endSyncBlock2()
{
    printf("Ending block 2");
    monitor.users--;
    if (monitor.users == 0) {
        sem_post(monitor.runFirstBlock);
    }
}

// Below are thread functions copied over from Semaphores project,
// split into two separate functions which can process any index of the alphabet array when
// they lay claim to the semaphore

void* thread_1(void* arg)
{
    while (idx < ALPHA_LENGTH) {
        printf("\nThread 1 %i, Entered...\n", pthread_self());
        printf("%c\n", alphabet[idx]);
        printf("Index at: %i", idx);
        ++idx; // increment index
        printf("\nJust Exiting...\n");
        // sleep(1);
    }
    return NULL;
}

// This thread contains a loop that would potentially run
// at the same time as thread 1, without the monitor construct
void* thread_2(void* arg)
{
    while (idx < ALPHA_LENGTH) {
        printf("\nThread 2 %i, Entered...\n", pthread_self());
        printf("%c\n", alphabet[idx]);
        printf("Index at: %i", idx);
        ++idx; // increment index
        printf("\nJust Exiting...\n");
        // sleep(1);
    }
    return NULL;
}

int main()
{
    idx = 0; // max of 25, total length of alphabet
    pthread_t th1, th2;

    if (monitor_init() == 0) {
        // pthread_create(&th1, NULL, thread_1, NULL);
        // pthread_create(&th2, NULL, thread_2, NULL);

        monitor_startSyncBlock1();
        pthread_create(&th1, NULL, thread_1, NULL);
        // pthread_join(th1, NULL);
        monitor_endSyncBlock1();
        /* **** */
        monitor_startSyncBlock2();
        pthread_create(&th2, NULL, thread_2, NULL);
        // pthread_join(th2, NULL);
        monitor_endSyncBlock2();

        // Make sure both threads finish before destroying monitor
        pthread_join(th1, NULL);
        pthread_join(th2, NULL);
        printf("Gets to here...");

        monitor_destroy();
    } else {
        printf("Could not initialize monitor!");
    }
    return 0;
}
