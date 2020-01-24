/*
* Author:	Daniel cender
* Date:		01/19/2020
* Basic program that utilizes pthreads and semaphores to
* produce a program that prints out the entire alphabet using multiple threads in sync.
*
*
*/

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t mutex;
char alphabet[] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};

/**
 * Func that prints out 2 chars of the alphabet array
 */
void* thread_func(void* arg) {
	sem_wait(&mutex); //wait state on semaphore
	for(int idx = 0; idx < 26; ++idx) { // infinite loop until returned from

	 	printf("\nThread %d, Entered...\n", pthread_self());
		if(idx % 4) {
			sem_post(&mutex); // Release semaphore
			sleep(1); // If  you don't do this, the semaphore will likely unlock immediately
			sem_wait(&mutex); //wait state on semaphore
		}
		printf("%c\n", alphabet[idx]);
		printf("Index at: %i", idx);
	}
	printf("\nJust Exiting...\n");

	sem_post(&mutex); // Release semaphore

	return NULL;
}

main() {

	// init semaphore to 0, for only this process
   sem_init(&mutex, 0, 1);
   pthread_t th1,th2;
	 //Create threads
   pthread_create(&th1,NULL,thread_func,NULL);
   pthread_create(&th2,NULL,thread_func,NULL);
   //Join threads with the main thread
   pthread_join(th1,NULL);
   pthread_join(th2,NULL);
   sem_destroy(&mutex);
}