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

// sem_t mutex;
char alphabet[] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
int idx;

pthread_t monitor_ts[2]; // Maybe hold couple of pthreads to handle/wait based on sync blocks?

struct MONITOR {
	sem_t startSync;
	sem_t endSync;
	int firstAction;
	int secondAction;
}

struct MONITOR monitor;

int monitor_init() {
    // Initialize the structure
   	monitor.firstAction = 0;
	 	monitor.secondAction = 0;

    // initialize the semaphores
    if(sem_init(&(monitor_data.startSync), 0, 1) == 0 &&
       sem_init(&(monitor_data.endSync), 0, 1) == 0){
				// Init and join threads here maybe??
        return 0;
    } else {
      printf("Unable to initialize semaphores\n");
    }
		return 1;
}

void monitor_destroy() {
	sem_destroy(&(monitor.startSync));
	sem_destroy(&(monitor.endSync));
}

/**
 * Func that prints out 2 chars of the alphabet array, shouldn't need
 * to wait or post to semaphore if working in monitor structure
 */
void* printAlphabet(void* arg) { //function which act like thread
	while(idx <= 25) {
	 if(idx > 25) break;
	 printf("\nEntered..\n");
	 printf("%c\n", alphabet[idx]);
	 printf("Index at: %i", idx);
	 ++idx; // increment index
	 printf("\nJust Exiting...\n");
	}
	pthread_exit(0);
}


main() {
	idx = 0; // max of 25, total length of alphabet

	if(monitor_init() == 0) {
		// Ready to define synchronized program logic
		monitor_startSync(); // TODO
		// Process 1
		wait(3); // Would ordinarily cause second thread to run first
		print("Would print out some stuff here, to print first....");
		monitor_endSync(); // TODO
	/* **** */
		monitor_startSync(); // TODO
		// Process 2
		print("Would print out some stuff here, to print second....");
		monitor_endSync(); // TODO

	};
	monitor_destroy();

}