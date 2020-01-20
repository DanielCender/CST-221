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
int idx;

/**
 * Func that prints out 2 chars of the alphabet array
 */
void* printAlphabet(void* arg) { //function which act like thread
	while(idx <= 25) {
   sem_wait(&mutex); //wait state on semaphore
	 if(idx > 25) break;
	 printf("\nEntered..\n");
	 printf("%c\n", alphabet[idx]);
	 printf("Index at: %i", idx);
	 ++idx; // increment index
	 printf("\nJust Exiting...\n");
   sem_post(&mutex); //send message to free up mutex
	}
	pthread_exit(0);
}


main() {
	idx = 0; // max of 25, total length of alphabet

	// init semaphore to 0, for only this process
   sem_init(&mutex, 0, 1);
   pthread_t th1,th2;
	 //Create threards
   pthread_create(&th1,NULL,printAlphabet,NULL);
   pthread_create(&th2,NULL,printAlphabet,NULL);
   //Join threads with the main thread
   pthread_join(th1,NULL);
   pthread_join(th2,NULL);
   sem_destroy(&mutex);
}