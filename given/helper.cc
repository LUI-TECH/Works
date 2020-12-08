/******************************************************************
 * The helper file that contains the following helper functions:
 * check_arg - Checks if command line input is a number and returns it
 * sem_create - Create number of sempahores required in a semaphore array
 * sem_init - Initialise particular semaphore in semaphore array
 * sem_wait - Waits on a semaphore (akin to down ()) in the semaphore array
 * sem_signal - Signals a semaphore (akin to up ()) in the semaphore array
 * sem_close - Destroy the semaphore array
 ******************************************************************/

# include "helper.h"

int check_arg (char *buffer)
{
  int i, num = 0, temp = 0;
  if (strlen (buffer) == 0)
    return -1;
  for (i=0; i < (int) strlen (buffer); i++)
  {
    temp = 0 + buffer[i];
    if (temp > 57 || temp < 48)
      return -1;
    num += pow (10, strlen (buffer)-i-1) * (buffer[i] - 48);
  }
  return num;
}

int sem_create (key_t key, int num)
{
  int id;
  if ((id = semget (key, num,  0666 | IPC_CREAT | IPC_EXCL)) < 0)
    return -1;
  return id;
}

int sem_init (int id, int num, int value)
{
  union semun semctl_arg;
  semctl_arg.val = value;
  if (semctl (id, num, SETVAL, semctl_arg) < 0)
    return -1;
  return 0;
}

// use int function to output error info
int sem_wait (int id, short unsigned int num)
{
  struct sembuf op[] = {
    {num, -1, SEM_UNDO}
  };
  // use semtimedop to wait for 20s
  struct timespec Time[] = { {20,0} };
  return semtimedop(id, op, 1, Time);
}

void sem_signal (int id, short unsigned int num)
{
  struct sembuf op[] = {
    {num, 1, SEM_UNDO}
  };
  semop (id, op, 1);
}

int sem_close (int id)
{
  if (semctl (id, 0, IPC_RMID, 0) < 0)
    return -1;
  return 0;
}


/* Function to append Circular queue */
void Queue::enQueue(int timecost) { 
  // if queue is full stop adding elements
  if (checkfull()){
    return;
  }

  // when queue is empty 
  else if (front == -1){ 

    // if it is an initialized queue, append to the begining
    if (rear == -1){
      front = rear =0;
    }

    // if empty is achieved by deleting, then append to the next slot
    else{
      front = rear ;//= 0; 
    }

    arr[rear] = rear;
    duration[rear] = timecost;
  } 
  
  // if queue has already reached the end, start from begining
  else if (rear == size-1 && front != 0) { 
    rear = 0; 
    arr[rear] = rear;
    duration[rear] = timecost;
  } 
  
  // normal case simply push the rear indicator forward
  else{ 
    rear++; 
    arr[rear] = rear;
    duration[rear] = timecost;
  } 
}

// Function to delete element from Circular Queue 
void Queue::deQueue(int& id, int& time) { 

  // if the queue is empty, then do nothing
  if (checkempty()){
    return;
  }

  // return the element at the top of the queue
  id = arr[front]; 
  time = duration[front];

  // reinitialize the deleted element
  arr[front] = -1;
  duration[front] = -1;

  // if it deleted element is the last one
  // reinitialize the front indicator 
  // and the rear points to the next slot, to indicate the empty status is not initialized
  if (front == rear) { 
    front = -1;
    rear = (rear+1)%size;
  }

  // if the front inidicator reached the end of queue
  // then star from begining
  else if (front == size-1) {
    front = 0; 
  }

  // Other normal case, simply add the front indicator
  else {
    front++; 
  }
} 


// to check if queue is full
bool Queue::checkfull(){
  // if all the slots are within the range between the 2 indicator, the queue is full
  if ((front == 0 && rear == size-1) || (rear == (front-1)%(size-1))){ 
    return true; 
  } 
  return false;
}

// to check if queue is empty
bool Queue::checkempty(){
  // if the front indicator is not within the slot range, then the buffer is empty
  if (front == -1){ 
    return true; 
  } 
  return false;
}




