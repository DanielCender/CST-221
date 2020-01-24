/*
 * Author: Daniel cender
 * Date: 01/22/2020
 * Basic program that utilizes pthreads and semaphores to
 * produce a program that prints out the entire alphabet using multiple threads
 * in sync.
 *
 * Build: gcc -o Sem Semaphores.c -lpthread
 * Run: ./Sem
 */

#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SEM_MUTEX_NAME "/sem-mutex"

sem_t* sem_mutex;
char alphabet[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
    'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
    's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };
int idx;

void* thread_f(void* arg)
{
    while (1) {
        if (idx < 25) {
            if (sem_wait(sem_mutex) == -1) {
                perror("sem_wait: mutex_sem");
                exit(1);
            }
            printf("\nThread %i, Entered...\n", pthread_self());
            printf("%c\n", alphabet[idx]);
            printf("Index at: %i", idx);
            ++idx; // increment index
            printf("\nJust Exiting...\n");
            // Release mutex sem: V (mutex_sem)
            if (sem_post(sem_mutex) == -1) {
                perror("sem_post: mutex_sem");
                exit(1);
            }
        } else {
            return NULL;
        }
    }
}

int main()
{
    idx = 0; // max of 25, total length of alphabet (26 chars)

    //  mutual exclusion semaphore, mutex_sem with an initial value 1.
    if ((sem_mutex = sem_open(SEM_MUTEX_NAME, O_CREAT, 0660, 1)) == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }
    pthread_t th1, th2;
    // Create threads
    pthread_create(&th1, NULL, thread_f, NULL);
    pthread_create(&th2, NULL, thread_f, NULL);
    // Join threads with the main thread
    pthread_join(th1, NULL);
    pthread_join(th2, NULL);
    if (sem_unlink(SEM_MUTEX_NAME) == -1) {
        perror("sem_unlink");
        exit(1);
    }
}
