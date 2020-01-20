/*
* Author:	Daniel cender
* Date:		01/19/2020
* Basic program that utilizes pthreads to produce and consume elements
*	to/from a buffer in synchronized order.
*
*	The buffer construct is circular, so the producer and consumer should
* follow around the array of elements like a singly-linked circular list.
*
*
*
*
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>

#define MAX 10

pthread_mutex_t mutex;
pthread_cond_t condp, condc;

struct BUFFER
{
	int count;
	int top;
	int lo;
	int buffer[MAX];
};

struct BUFFER *buff = NULL;

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
* @arg el Item to put in back of buffer array
*/
void put(int el)
{
	// Since this is a circular struct,
	// Set top to 0 if index would overflow array length
	printf("Putting %i in buffer index %i\t", el, buff->top);
	buff->buffer[buff->top] = el;
	buff->top = (buff->count + 1 != MAX) ? buff->top + 1 : 0;
	++buff->count;
}

/**
 * Func to get item from static buffer
 * @return First item in buffer array
 */
int get()
{
	int elem = buff->buffer[buff->lo];
	// Since this is a circular buffer, we
	// dont want to overflow the set array length
	buff->lo = (buff->lo + 1 == MAX) ? 0 : buff->lo + 1;
	--buff->count;
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
		// We want to wait on the consumer to chew through
		// at least 1 more item, since the buffer is full
		printf("PRODUCER: MAX equals: %i\n", MAX);
		while (buff->count == MAX)
			pthread_cond_wait(&condp, &mutex);

		int elem = produce();
		printf("PRODUCER: Placing %i in buffer...\n", elem);
		put(elem);
		// Wake up the consumer thread
		pthread_cond_signal(&condc);
		// Release the mutex to be locked by the consumer thread
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
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
		// If there are NO ITEMS to get from buffer,
		// go ahead and wait until the producer adds at least
		// 1 more
		printf("CONSUMER: MAX equals: %i\n", MAX);
		printf("CONSUMER: buffer count equals: %i\n", buff->count);
		while (buff->count == 0)
			pthread_cond_wait(&condc, &mutex);

		int item = get();
		printf("CONSUMER: Got element %i from buffer...\n", item);
		// Signal Producer thread to wake up
		pthread_cond_signal(&condp);
		// Free mutex so Producer can lock and get exclusive access to buffer
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
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
	buff = (struct BUFFER *)mmap(0, sizeof(buff), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	buff->count = 0;
	buff->lo = 0;
	buff->top = 0;

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
