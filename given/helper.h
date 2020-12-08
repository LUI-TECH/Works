/******************************************************************
 * Header file for the helper functions. This file includes the
 * required header files, as well as the function signatures and
 * the semaphore values (which are to be changed as needed).
 ******************************************************************/


# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/ipc.h>
# include <sys/shm.h>
# include <sys/sem.h>
# include <sys/time.h>
# include <math.h>
# include <errno.h>
# include <string.h>
# include <pthread.h>
# include <ctype.h>
# include <iostream>

using namespace std;

# define SEM_KEY 0x50 // Change this number as needed


union semun {
    int val;               /* used for SETVAL only */
    struct semid_ds *buf;  /* used for IPC_STAT and IPC_SET */
    ushort *array;         /* used for GETALL and SETALL */
};

int check_arg (char *);
int sem_create (key_t, int);
int sem_init (int, int, int);
int sem_wait (int, short unsigned int);	// int type to return err info
void sem_signal (int, short unsigned int);
int sem_close (int);

// Function to execute producer tasks
void *producer (void *id);

// Function to execute consumer tasks
void *consumer (void *id);

// CircularQueue Class
class Queue 
{
public:
  // Initialize front and rear positional indicators 
  int front; 
  int rear;
  
  // Initialize the maximum capacity of the circular queue
  int size; 

  // Initilize the circularqueue's slots
  int *arr; 
  int *duration;
  
  // Constructor to initialize the front, rear indicator, capacity of queue and the slots
  Queue(int size) : front(-1), rear(-1), size(size), arr(new int[size]) ,duration(new int[size]){};

  // Destructor to delete heap memory allocated for elements' id and duation.
  ~Queue(){
    delete [] arr;
    delete [] duration;
  }

  // Append item at the end of the circular queue
  void enQueue(int duration); 

  // Pop item from the top of the circular queue
  void deQueue(int& id, int& time);

  // Check if the queue is full
  bool checkfull();

  // Check if the queue is empty
  bool checkempty();
};

// Input parameter structure for producer and consumer;
struct parameter{
  int id;         // Initialize the job id
  int num_job;    // Initialize the number of jobs required for each producer to produce
  int duration;   // Initialize the duration of production time for each producer
  int semid;      // Initialize the id of semaphore set
  Queue *buffer;  // Initialize the circular queue to store jobs
};



