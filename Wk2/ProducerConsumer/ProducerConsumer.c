/*
* Author:	Daniel cender
* Date:		01/19/2020
* Basic program that utilizes pthreads to produce and consume elements
*	to/from a buffer in synchronized order.
*
*	The buffer construct just a single work, so the producer and consumer
* should be taking even turns without deadlocking or reading a zero.
*
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>

pthread_mutex_t mutex;
pthread_cond_t condp, condc;
int buffer;

/* *** BEGIN Utility Functions *** */
/**
 * Func to produce random int value
 * @return Random int value
 */
int produce()
{
	return rand();
}

/**
* Func to put item in static buffer
* @arg el Item to put in buffer
*/
void put(int el)
{
	buffer = el;
}

/**
 * Func to get item from static buffer
 * @return buffer item
 */
int get()
{
	int elem = buffer;
	buffer = 0; // reset static buffer
	return elem;
}
/* *** END Utility Functions *** */

/**
 * Producer
 *
 * @param a Not used.
 * @return Always null.
 */
void *
Producer(void *a)
{

	// Aside from checking the mutex,
	// this process should be prepared to grind through
	// all its items
	while (1)
	{
		// Gets exclusive access to buffer, so our read of buff->count
		// is time-accurate
		pthread_mutex_lock(&mutex);
		// If buffer is not empty,
		// wait for consumer to grab item
		while (buffer != 0)
			pthread_cond_wait(&condp, &mutex);

		int elem = produce();
		put(elem);
		printf("PRODUCER: Placed element %i in buffer...\n", elem);
		// Wake up the consumer thread
		pthread_cond_signal(&condc);
		// Release the mutex to be locked by the consumer thread
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(0);
}

/**
 * Consumer
 *
 * @param a Not used.
 * @return Always null.
 */
void *Consumer(void *a)
{
	// Aside from checking the mutex,
	// this process should be prepared to grind through
	// the buffer forever
	while (1)
	{
		// Lock up mutex
		pthread_mutex_lock(&mutex);
		// If the buffer is empty,
		// go ahead and wait until the producer adds item
		while (buffer == 0)
			pthread_cond_wait(&condc, &mutex);

		int item = get();
		printf("CONSUMER: Got element %i from buffer...\n", item);
		// Signal Producer thread to wake up
		pthread_cond_signal(&condp);
		// Free mutex so Producer can lock and get exclusive access to buffer
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(0);
}

/**
 * Main application entry point to create some threads.
 *
 * @return 1 if error or 0 if OK returned to code the caller.
 */
int main()
{
	pthread_t producer, consumer;

	// Create shared memory for the Circular Buffer to be shared between the Parent and Child  Processes
	buffer = 0;

	pthread_mutex_init(&mutex, 0);
	pthread_cond_init(&condp, 0);
	pthread_cond_init(&condc, 0);

	// Create 2 threads
	if (pthread_create(&producer, NULL, Producer, NULL))
	{
		printf("\n ERROR creating Producer");
		exit(1);
	}
	if (pthread_create(&consumer, NULL, Consumer, NULL))
	{
		printf("\n ERROR creating Consumer");
		exit(1);
	}

	// Wait for both threads to finish
	if (pthread_join(producer, NULL))
	{
		printf("\n ERROR joining Thread_1");
		exit(1);
	}
	if (pthread_join(consumer, NULL))
	{
		printf("\n ERROR joining Thread_1");
		exit(1);
	}

	// Thread creation cleanup
	pthread_exit(NULL);
	// Cond variable cleanup
	pthread_cond_destroy(&condp);
	pthread_cond_destroy(&condc);
	// Mutex cleanup
	pthread_mutex_destroy(&mutex);

	return 0;
}
