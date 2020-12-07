/******************************************************************
 * The Main program with the two functions. A simple
 * example of creating and using a thread is provided.
 ******************************************************************/
# include <sys/sem.h>
#include "helper.h"

void *producer (void *id);
void *consumer (void *id);
void arg_check(int arg);







class Queue 
{
public:
  // Initialize front and rear 
  int front; 
  int rear;
  
  // Circular Queue 
  int size; 
  int *arr; 
  int *duration;
  
  Queue(int size) : front(-1), rear(-1), size(size), arr(new int[size]) ,duration(new int[size]){};
  ~Queue(){
    delete [] arr;
    delete [] duration;
  }
  void enQueue(int duration); 
  void deQueue(int& id, int& time); 
  void displayQueue(); 
  bool checkfull();
  bool checkempty();
};

struct parameter{
  int id;
  int num_job;
  int duration;
  int semid;
  Queue *buffer;
};



int main (int argc, char **argv){

  // TODO
  if (argc!=5){
    cerr<<"Incorrect number of argument, Expect 4 but get "<<argc-1<<endl;
    return 0;
  }

  int queue_size = check_arg(argv[1]);
  int num_job = check_arg(argv[2]);
  int num_producer = check_arg(argv[3]);
  int num_consumer = check_arg(argv[4]);

  if (queue_size==-1 || num_job==-1 || num_producer==-1 || num_consumer==-1){
    cerr<<"Input is not Number"<<endl;
    return -1;
  }
  
  Queue *buffer = new Queue(queue_size);
  pthread_t * producerIds = new pthread_t[num_producer];
  pthread_t * consumerIds = new pthread_t[num_consumer];


  key_t key;
  key = ftok("./main", 'A');
  int semid = sem_create(key, 3);

  if (sem_init(semid,0,1)==-1){
    perror("check mutex semaphore init failed\n");
  }
  if (sem_init(semid,1,0)==-1){
    perror("check empty semaphore init failed\n");
  }
  if (sem_init(semid,2,queue_size)==-1){
    perror("check full semaphore init failed\n");
  }

  int ret, duration;
  parameter * P = new parameter[num_producer];

  for (int i =0; i < num_producer;i++){
    duration = rand() % 5 +1;
    P[i] = (parameter) {i,num_job,duration,semid,buffer};
    ret = pthread_create (&producerIds[i], NULL, producer, (void *) &P[i]);
    if (ret!=0){
      perror("Producer pthread_create failed\n");
      exit(EXIT_FAILURE);
    }
  }

  parameter * C = new parameter[num_consumer];

  for (int i =0; i < num_consumer;i++){
    C[i] = (parameter) {i,1,0,semid,buffer};
    ret = pthread_create (&consumerIds[i], NULL, consumer, (void *) &C[i]);
    if (ret!=0){
      perror("Consumer pthread_create failed\n");
      exit(EXIT_FAILURE);
    }
  }

  for (int i =0; i < num_producer;i++){
    ret = pthread_join (producerIds[i], NULL);
    if (ret!=0){
      perror("Producer pthread_join failed\n");
      exit(EXIT_FAILURE);
    }
  }

  for (int i =0; i < num_consumer;i++){
    ret = pthread_join (consumerIds[i], NULL);
    if (ret!=0){
      perror("Consumer pthread_join failed\n");
      exit(EXIT_FAILURE);
    }
  }


  cout << "Doing some work after the join" << endl;

  sem_close(semid);
  pthread_exit(NULL);

  delete buffer;
  delete [] producerIds;
  delete [] consumerIds;
  delete [] P;
  delete [] C;
  return 0;
}


void *producer (void *P) 
{

  // TODO
  parameter *param = (parameter *) P;
  int counter =0;
  int duration,ret;


  while (counter < param->num_job){

    sleep(param->duration);

    ret = sem_wait(param->semid, 2);
    if (ret!=0){
      void * result;
      cerr<<"Full buffer cannot be consumed"<<endl;
      pthread_exit(0);
      return result;
    }



    sem_wait(param->semid, 0);

    
    duration = rand() % 10 +1;
    param->buffer->enQueue(duration);
    cout<<"Producer("<<param->id<<"): Job id "<<param->buffer->rear<<" duration "<<param->duration<<endl;

    sem_signal(param->semid, 0);

    sem_signal(param->semid, 1);
    counter++;
  }

  cout<<"Producer("<<param->id<<"): No more jobs to generate."<<endl;
  pthread_exit(0);
}

void *consumer (void *C)
{
  int jobid, duration, ret;
  parameter *param = (parameter *) C;

  while (true){

    ret = sem_wait(param->semid, 1);
    if (ret!=0){
      void * result;
      cerr<<"Consumer(3): No more jobs left."<<endl;
      pthread_exit(0);
      return result;
    }

    sem_wait(param->semid, 0);

    // TODO 
    param->buffer->deQueue(jobid,duration);
    cout<<"Consumer("<<param->id<<"): Job id "<<jobid<<" executing sleep duration "<<duration<<endl;


    sem_signal(param->semid, 0);

    sleep(duration);
    cout<<"Consumer("<<param->id<<"): Job id "<<jobid<<" completed"<<endl;
    
    sem_signal(param->semid, 2);



  }

}





/* Function to create Circular queue */
void Queue::enQueue(int timecost) { 
  if (checkfull()){
    return;
  }
  
  else if (front == -1){ 
    front = rear = 0; 
    arr[rear] = rear;
    duration[rear] = timecost;
  } 
  
  else if (rear == size-1 && front != 0) { 
    rear = 0; 
    arr[rear] = rear;
    duration[rear] = timecost;
  } 
  
  else{ 
    rear++; 
    arr[rear] = rear;
    duration[rear] = timecost;
  } 
}

// Function to delete element from Circular Queue 
void Queue::deQueue(int& id, int& time) { 
  if (checkempty()){
    return;
  }

  id = arr[front]; 
  time = duration[front];

  arr[front] = -1;
  duration[front] = -1;

  if (front == rear) { 
    front = -1; 
    rear = -1; 
  } 
  else if (front == size-1) {
    front = 0; 
  }
  else {
    front++; 
  }
} 


bool Queue::checkfull(){
  if ((front == 0 && rear == size-1) || (rear == (front-1)%(size-1))){ 
    return true; 
  } 
  return false;
}

bool Queue::checkempty(){
  if (front == -1){ 
    return true; 
  } 
  return false;
}

void Queue::displayQueue() { 
  if (front == -1) { 
    cout<<"Empty Queue";
    return; 
  } 
  cout<<"Elements in Circular Queue are: "<<endl;
  if (rear >= front) { 
    for (int i = front; i <= rear; i++) {
      cout<<"id="<<arr[i]<<" |";
    }
  } 

  else{ 
    for (int i = front; i < size; i++) {
      cout<<"id= "<<arr[i]<<" |";
    }
  
    for (int i = 0; i <= rear; i++) {
      cout<<"id= "<<arr[i]<<" |";
    }
  }
  cout<<endl;
} 



  //struct timespec Time[] = { {20,0} };
  //semtimedop(semid, op, 1, Time);
  // pthread_join (producerIds[i], NULL);
  // pthread_join (consumerIds[i], NULL);

  /*pthread_t producerid;
  int parameter = 5;

  pthread_create (&producerid, NULL, producer, (void *) &parameter);

  pthread_join (producerid, NULL);*/