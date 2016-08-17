
/*----------- CS590------------ */
/*--------- Final project------ */
/*Author: Poornima Byre Gowda-- */
/*----------------------------- */
/*In this Assignment pthread library to write a C program that creates a number of producer and consumer threads. The producer threads
 will generate random numbers to be stored in a circular queue. The consumer threads will remove elements from the queue. */
/* producer and consumer concept can be implemented using the circular queue using data structures */

// Headers

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define Queue_size 100 // max queue size of the queue
unsigned int seed = 1000;// giving the seed value


int Queue_size_count = 0; // variable that keep count of the number of slots inserted to queue
int index_number = 0; // holds the slot number of the queue


/* THREAD INITIALIZATION */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t producer = PTHREAD_COND_INITIALIZER;
pthread_cond_t consumer = PTHREAD_COND_INITIALIZER;

/*--------- QUEUE structure----------*/
/*----------------------------*/


typedef struct Queue
        {
        int  random_value; // saves random number
        int  queue_index; // queue slot value
        struct  Queue  *next; // holding the next pointer value
        } Queue;

        Queue  *head; // Head of the queue.
        Queue  *tail;  // tail of the queue.

/* --------------------------*/

Queue_initialisation() // constructor
{
        head = tail = NULL;  //avoiding bad pointer segmentation faults.

}
/* -----------------------------*/


/* Enqueuing */
/* Enqueuing */
Enqueue()
{

        Queue *temp; // creating the temporary value
        temp = (Queue *) malloc(sizeof(Queue)); // dynamic allocation of queue

        /*randomising the value */

        int num = (rand_r(& seed)%1000) + 1;
        seed = rand ();
        temp->random_value = num;
        index_number = index_number % (Queue_size ) ; // saving the index or slot number
        temp->queue_index = index_number; // slot number is allotted.


        index_number = index_number + 1; // increament in index number
        temp->next = NULL;

        if(head == NULL) // if head is null
                head = tail = temp; // head and tail hold temporary value
        else
        {
               // tail is joined to head to make circular queue
                tail->next = temp;
                tail = temp;
                tail->next = head;


        }
                Queue_size_count = Queue_size_count + 1; // queue_size value increment.

                /*To print thread ID */
        pthread_t tid1;
        tid1 = pthread_self();
        printf(" The producer tid:  %lu add  %d to slot %d \n ",(unsigned long)tid1,tail-> random_value,tail->queue_index );
                //printf("%d", Queue_size_count);
}

/* --------------DEQUEUE------------------------*/
Dequeue()
{
        Queue *temp;


        if (head == NULL) // if the queue is empty
        {
                printf("\n Queue is Empty , consumer have to wait for certain time \n ");
                pthread_cond_wait( &consumer, &mutex);
        }
        else
        {
                temp = head;
                head = head->next;

                                /* Printing Thread ID */
                                pthread_t tid2;
                                tid2 = pthread_self();
 printf(" The Consumer tid:  %lu removed  %d from slot %d \n ",(unsigned long)tid2,temp-> random_value,temp->queue_index );

                if(head == tail)
                {
                  tail->next = NULL;
                }
                else
                                {
                tail-> next = head;
                }
                free(temp); // freeing the alloted space
                Queue_size_count = Queue_size_count - 1;// queue size count
               //printf("%d", Queue_size_count);

        }
}



/*---------Producer----------*/
/*---------------------------*/

void *producer_func(void * arg)
{
            while(1)
                {

                        pthread_mutex_lock(&mutex); // locking the mutex
                                                // if the Thread is full
                        while(  Queue_size_count == 100)
                        {
                                printf(" \n Queue is full , Producer have to wait for certain time \n");
                                pthread_cond_wait(&producer, &mutex );
                        }

            //printf(" The producer " );

            Enqueue(); // calling enqueue function

                        pthread_cond_signal(&consumer); // signalling the consumer
                        pthread_mutex_unlock(&mutex); // removing the lock

                        // random sleep valueue
                        srand(seed);
                        int r =  rand() % 4;
                        sleep(r);
                }

          return((void *)1);
}
/* --------------------------*/


/* --------Consumer--------- */
/* --------------------------*/
void *consumer_func(void *arg)
{
        while(1)
        {

        pthread_mutex_lock(&mutex);// locking the mutex


                Dequeue(); // Dequeing the queue

                pthread_cond_signal(&producer); // signalling the producer
        pthread_mutex_unlock(&mutex); // unlocking the mutex

        // creating the random sleep value
        srand(seed);
        int r =  rand() % 4;
        sleep(r);

        }

        return((void *)2);
}
/* --------------------------*/




/*       Main function             */
int main(int argc, char **argv)
{
        int err, i;
        void *tret;
        int num_producer;
        int num_consumer;
        char buffer[10];

        Queue_initialisation();// initialising the queue

        /*       Number of producer          */

        printf("Number of producers : ");
        fgets (buffer, 10, stdin);
                num_producer = atoi (buffer);
                //printf("%d",num_producer );

        /*       Number of Consumer          */

        printf("Number of Consumers : ");
        fgets (buffer, 10, stdin);
                num_consumer = atoi (buffer);
                //printf("%d",num_consumer );


        /*   Creating the array of Producer and consumer thraeds     */
                pthread_t  producer_thread[num_producer];
        pthread_t  consumer_thread[num_consumer];

		  // creating the producer thread
        for (i=1; i<= num_producer; i++)
        {
                err = pthread_create(&producer_thread[i], NULL, producer_func, (void *)1);
                if (err != 0)
            printf( "can’t create producer thread number %d ", i);
        }
                // creating the consumer thread
        for ( i=1; i<= num_consumer; i++)
        {
                err = pthread_create(&consumer_thread[i], NULL, consumer_func, (void *)1);
                if (err != 0)
                printf( "can’t create consumer thread number %d ", i);
        }
         // receiving the return value of the thread
        for ( i=1; i<= num_producer; i++)
        {
                err = pthread_join(producer_thread[i], &tret);
                if (err != 0)
                perror( "can’t join with producer thread ");
                printf(" producer thread %d exit code %ld\n", i, (long)tret);
    }


        for (i=1; i<= num_consumer; i++)
        {
                err = pthread_join(consumer_thread[i], &tret);
                if (err != 0)
                perror( "can’t join with consumer thread ");
                printf("thread 2 exit code %ld\n", (long)tret);
        }
    // Destroying the mutex
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&producer);
    pthread_cond_destroy(&consumer);

}
