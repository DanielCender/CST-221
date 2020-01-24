/* Filename: customer.c */
/*
Code from:
https://www.thegeekstuff.com/2011/12/c-socket-programming/?utm_source=feedburner

To get the socket connections to fail and overrun the available resources,
comment out the sem_wait() and sem_post() calls and change the network
retry count to a low number (like 2). To change it, do the following
on the command line:


To see what retry count is set to:
cat /proc/sys/net/ipv4/tcp_syn_retries

To change retry count, enter root and then change the retry count:
sudo su
echo 2 > /proc/sys/net/ipv4/tcp_syn_retries

Ubuntu was set to 5 by default

*/

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>  /* Semaphore */

#define FALSE 0
#define TRUE 1

// 5 customers contacting customer service through 2 ports
#define NUM_CUSTOMERS 10
int ports[] = {5000,5001};

int ContactRep (char *addr); // Prototype needed because function is called before it is defined

/*
    semaphores are declared global so they can be accessed
    in main() and in thread routine,
*/
sem_t csrChatPort;


/*
    This code originally from:
    https://stackoverflow.com/questions/7684359/how-to-use-nanosleep-in-c-what-are-tim-tv-sec-and-tim-tv-nsec

    It was modifed to be named msleep for milliseconds and mispelling was
    fixed. This function sleeps using the nanosecond sleep function in
    the library.
*/
int msleep(long milliseconds)
{
   struct timespec req, rem;

   if(milliseconds > 999)
   {
        req.tv_sec = (int)(milliseconds / 1000);                            /* Must be Non-Negative */
        req.tv_nsec = (milliseconds - ((long)req.tv_sec * 1000)) * 1000000; /* Must be in range of 0 to 999999999 */
   }
   else
   {
        req.tv_sec = 0;                         /* Must be Non-Negative */
        req.tv_nsec = milliseconds * 1000000;    /* Must be in range of 0 to 999999999 */
   }

   return nanosleep(&req , &rem);
}


/**
 * Thread_1 function.
 *
 * @param a Not used.
 * @return Always null.
 */
void *Thread_1(void *a)
{
    for (int i = 0; i < NUM_CUSTOMERS; ++i)
    {
        //printf("\nIn Thread_1 to chat with CSR\n");
        ContactRep("127.0.0.1");
        //msleep(1);
    }
    return NULL;
}

/**
 * Thread_2 function.
 *
 * @param a Not used.
 * @return Always null.
 */
void *Thread_2(void *a)
{
    for (int i = 0; i < NUM_CUSTOMERS; ++i)
    {
        //printf("\nIn Thread_2 to chat with CSR\n");
        ContactRep("127.0.0.1");
        //msleep(150);
    }
    return NULL;
}

/**
 * Thread_3 function.
 *
 * @param a Not used.
 * @return Always null.
 */
void *Thread_3(void *a)
{
    for (int i = 0; i < NUM_CUSTOMERS; ++i)
    {
        //printf("\nIn Thread_3 to chat with CSR\n");
        ContactRep("127.0.0.1");
        //msleep(150);
    }
    return NULL;
}

/**
 * Thread_4 function.
 *
 * @param a Not used.
 * @return Always null.
 */
void *Thread_4(void *a)
{
    for (int i = 0; i < NUM_CUSTOMERS; ++i)
    {
        //printf("\nIn Thread_4 to chat with CSR\n");
        ContactRep("127.0.0.1");
        //msleep(150);
    }
    return NULL;
}

char portActive[2] = {FALSE, FALSE};
int ContactRep (char *addr)
{
    int sockfd = 0, n = 0;
    struct sockaddr_in serv_addr;
    char recvBuff[1024];
    int port;

    sem_wait(&csrChatPort); // Wait for the resource
    // You are now in the critical section

    if (portActive[0] == TRUE) // First port is in use, use the second one
    {
        port = ports[1];
        portActive[1] = TRUE;
    }
    else // Second port is in use, use the first one
    {
        port = ports[0];
        portActive[0] = TRUE;
    }

    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if(inet_pton(AF_INET, &addr[0], &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    }

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    }

    while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
    {
        recvBuff[n] = 0;
        if(fputs(recvBuff, stdout) == EOF)
        {
            printf("\n Error : Fputs error\n");
        }
    }

    if(n < 0)
    {
        printf("\n Read error \n");
    }

    close(sockfd); // Not required in the example, but good practice to clean up after yourself

    if (port == ports[0]) // First port was used
    {
        portActive[0] = FALSE;
    }
    else // Second port was used
    {
        portActive[1] = FALSE;
    }

    sem_post(&csrChatPort);   /* free the resource */
    // Out of critical section

    return 0;
}

int main(int argc, char *argv[])
{
    pthread_t tid1, tid2, tid3, tid4;

    sem_init(&csrChatPort, 1, 2); /* initialize mutex to 2 resources*/
                                  /* second param = 1 - semaphore is shared between threads */

    // Create 2 threads
    if(pthread_create(&tid1, NULL, Thread_1, NULL))
    {
        printf("\n ERROR creating Thread_1");
        exit(1);
    }
    if(pthread_create(&tid2, NULL, Thread_2, NULL))
    {
        printf("\n ERROR creating Thread_2");
        exit(1);
    }

    if(pthread_create(&tid3, NULL, Thread_3, NULL))
    {
        printf("\n ERROR creating Thread_3");
        exit(1);
    }

    if(pthread_create(&tid4, NULL, Thread_4, NULL))
    {
        printf("\n ERROR creating Thread_4");
        exit(1);
    }

    // Wait for both threads to finish
    if(pthread_join(tid1, NULL))
    {
        printf("\n ERROR joining Thread_1");
        exit(1);
    }
    if(pthread_join(tid2, NULL))
    {
        printf("\n ERROR joining Thread_2");
        exit(1);
    }
    if(pthread_join(tid3, NULL))
    {
        printf("\n ERROR joining Thread_3");
        exit(1);
    }

    if(pthread_join(tid4, NULL))
    {
        printf("\n ERROR joining Thread_4");
        exit(1);
    }

    sem_destroy(&csrChatPort); /* destroy semaphore */

    // Thread creation cleanup
    pthread_exit(NULL);
    printf("\nNo more customers trying to chat with CSRs\n");
}