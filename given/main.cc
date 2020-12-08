/******************************************************************
 * The Main program with the two functions. A simple
 * example of creating and using a thread is provided.
 ******************************************************************/
# include <sys/sem.h>
#include "helper.h"





int main (int argc, char **argv){

  // check is the numebr of arguments is correct
  if (argc!=5){
    cerr<<"Incorrect number of argument, Expect 4 but get "<<argc-1<<endl;
    return 0;
  }


  // Assign input arguments and check if they are numebrs.

  int queue_size = check_arg(argv[1]);    // Assign the capacity of circular queue
  int num_job = check_arg(argv[2]);       // Assign the task numbers required for each producer to produce
  int num_producer = check_arg(argv[3]);  // Assign the producer number
  int num_consumer = check_arg(argv[4]);  // Assign the consumer number 

  // if any arguments are not number return error code
  if (queue_size==-1 || num_job==-1 || num_producer==-1 || num_consumer==-1){
    cerr<<"Input is not Number"<<endl;
    return -1;
  }
  
  // Initilize the circular queue --- buffer
  Queue *buffer = new Queue(queue_size);

  // Initilize the array of producer threads
  pthread_t * producerIds = new pthread_t[num_producer];

  // Initilize the array of consumer threads
  pthread_t * consumerIds = new pthread_t[num_consumer];


  // Initialize semaphore set 
  // Require 3 semaphores 1 for mutex, 1 to check if buffer is empty, adn another 1 to check is buffer is full
  
  // Initialize adn assign the key to the current execution file
  key_t key;
  key = ftok("./main", 'M');

  // Initiliza and assign the id for semaphore set 
  int semid = sem_create(key, 3);

  // Initilialize MUTEX semphore for mutex check
  if (sem_init(semid,0,1)==-1){
    perror("check mutex semaphore init failed\n");
  }

  // Initilialize EMPTY semphore for empty buffer check
  if (sem_init(semid,1,0)==-1){
    perror("check empty semaphore init failed\n");
  }

  // Initilialize FULL semphore for full buffer check
  if (sem_init(semid,2,queue_size)==-1){
    perror("check full semaphore init failed\n");
  }


  // Initialize temporary variables to store random duration and return value
  int ret, duration;

  // Initilize the array of input parameter for producers
  parameter * P = new parameter[num_producer];

  // Loop to access all producers
  for (int i =0; i < num_producer;i++){
    duration = rand() % 5 +1;                               // Assign random duration time between 1-5 s
    P[i] = (parameter) {i,num_job,duration,semid,buffer};   // Assenble the input parameter
    ret = pthread_create (&producerIds[i], NULL, producer, (void *) &P[i]);   // Create and execute the producer threads
    
    // Check for thread creation error
    if (ret!=0){
      perror("Producer pthread_create failed\n");
      exit(EXIT_FAILURE);
    }
  }

  // Initilize the array of input parameter for consumers
  parameter * C = new parameter[num_consumer];

  // Loop to access all producers
  for (int i =0; i < num_consumer;i++){
    C[i] = (parameter) {i,1,0,semid,buffer};    // Assenble the input parameter, in consumer case duration is default to 0
    ret = pthread_create (&consumerIds[i], NULL, consumer, (void *) &C[i]);   // Create and execute the consumer threads
    
    // Check for thread creation error
    if (ret!=0){
      perror("Consumer pthread_create failed\n");
      exit(EXIT_FAILURE);
    }
  }

  // Producer threads synchronisation
  for (int i =0; i < num_producer;i++){
    ret = pthread_join (producerIds[i], NULL);      // synchronisation of each producer threads

    // Check for Synchronisation failure
    if (ret!=0){
      perror("Producer pthread_join failed\n");
      exit(EXIT_FAILURE);
    }
  }

  // Consumer threads synchronisation
  for (int i =0; i < num_consumer;i++){
    ret = pthread_join (consumerIds[i], NULL);    // synchronisation of each consumer threads
    
    // Check for Synchronisation failure
    if (ret!=0){
      perror("Consumer pthread_join failed\n");
      exit(EXIT_FAILURE);
    }
  }

  // Finish execution
  // Clear semaphores
  sem_close(semid);
  

  // delete all allocated heap memory
  delete buffer;
  delete [] producerIds;
  delete [] consumerIds;
  delete [] P;
  delete [] C;

  // Clear threads
  pthread_exit(NULL);
  return 0;
}

// Producer function
void *producer (void *P) 
{

  // Cast the input (void*) parameter into the original form
  parameter *param = (parameter *) P;

  // Initialize loop counter
  int counter =0;

  // Initilize temporary variable duration and return value
  int duration,ret;

  // Using loop to request producer to produce required number of jobs
  while (counter < param->num_job){

    // Sleep to represent production process
    sleep(param->duration);

    // FULL Sem value -1 (down) Semaphore waits for 20s to ensure buffer is not full when adding the job
    ret = sem_wait(param->semid, 2);

    // if timeout and the buffer is still full, then return error
    if (ret!=0){
      void * result;    // to satisfy output
      cerr<<"Full buffer cannot be consumed"<<endl;
      pthread_exit(0);
      return result;
    }

    // MUTEX Sem value -1 (down) Semaphore waits for 20s to ensure mutex requirement
    ret = sem_wait(param->semid, 0);

    // if timeout and the muxtex requriment still not satisfied, return error
    if (ret!=0){
      void * result;
      cerr<<"MUTEX fail"<<endl;
      pthread_exit(0);
      return result;
    }

    // Assign the random duration (1-10) for each jobs
    duration = rand() % 10 +1;

    // Append the job into the Circular Queue
    param->buffer->enQueue(duration);

    // Print info
    cout<<"Producer("<<param->id<<"): Job id "<<param->buffer->rear<<" duration "<<duration<<endl;

    // MUTEX Sem value +1 (up) update mutex semaphore to release lock
    sem_signal(param->semid, 0);

    // EMPTY Sem value +1 (up) to unlock empty semaphore
    sem_signal(param->semid, 1);
    counter++;
  }

  // If all jobs are produced, return adn print info
  cout<<"Producer("<<param->id<<"): No more jobs to generate."<<endl;

  // Clear threads
  pthread_exit(0);
}

// Consumer function
void *consumer (void *C)
{

  // Cast the input (void*) parameter into the original form
  parameter *param = (parameter *) C;

  // Initialize temporary variables which are used to store :
  // the id of consumed job
  // the duration to execute the job
  // the return value for error handling
  int jobid, duration, ret;

  // Infinite loop until No jobs left that can be consumed
  while (true){

    // Empty Sem value -1 (down) Semaphore waits for 20s to ensure buffer is not empty when popping out the job
    ret = sem_wait(param->semid, 1);

    // if timeout and the buffer is still full, then retrun and exit the program
    if (ret!=0){
      void * result;
      cerr<<"Consumer("<<param->id<<") No more jobs left."<<endl;
      pthread_exit(0);
      return result;
    }

    // MUTEX Sem value -1 (down) Semaphore waits for 20s to ensure mutex requirement
    ret = sem_wait(param->semid, 0);

    // if timeout and the muxtex requriment still not satisfied, return error
    if (ret!=0){
      void * result;
      cerr<<"MUTEX fail"<<endl;
      pthread_exit(0);
      return result;
    }

    // Poppong out the job from buffer and print info
    param->buffer->deQueue(jobid,duration);
    cout<<"Consumer("<<param->id<<"): Job id "<<jobid<<" executing sleep duration "<<duration<<endl;

    // MUTEX Sem value +1 (up) update mutex semaphore to release lock
    sem_signal(param->semid, 0);
    
    // EMPTY Sem value +1 (up) to unlock FULL semaphore
    sem_signal(param->semid, 2);

    // Sleep to represent execution of the consumed job
    sleep(duration);
    cout<<"Consumer("<<param->id<<"): Job id "<<jobid<<" completed"<<endl;
  }

}






