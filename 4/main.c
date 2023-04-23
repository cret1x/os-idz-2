#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>



static volatile int keepRunning = 1;


// Struct for memory
typedef struct shared_memory {
  int current_client;
}shared_memory;


// Function to handle SIGINT
void intHandler(int dummy) {
    printf("[Cutter] SIGINT Detected!\n");
    keepRunning = 0;
}

// Function to -1 semaphore
void sem_close(int semid) {
 struct sembuf parent_buf = {
    .sem_num = 0,
    .sem_op = -1,
    .sem_flg = 0
  };
  if (semop(semid, & parent_buf, 1) < 0) {
    printf("Can\'t sub 1 from semaphor\n");
    exit(-1);
  }
}

// Function to +1 semaphore
void sem_open(int semid) {
 struct sembuf parent_buf = {
    .sem_num = 0,
    .sem_op = 1,
    .sem_flg = 0
  };
  if (semop(semid, & parent_buf, 1) < 0) {
    printf("Can\'t add 1 to semaphor\n");
    exit(-1);
  }  
}


// Function for client
void child(int semid, shared_memory* shmptr)  {
  srand(time(NULL) ^ (getpid()<<16));
  printf("I am a client #%d!\n", getpid());
  sleep(rand() % 10 + 2);
  printf("[%d] In queue for cutter\n", getpid());
  sem_close(semid);
  printf("[%d] Going to cutter\n", getpid());
  shmptr->current_client = getpid();
  exit(0);
}



int main(int argc, char ** argv) {
  if (argc < 2) {
    printf("usage: ./main <clients count>");
    return -1;
  }
  signal(SIGINT, intHandler);
  int clients = atoi(argv[1]);
  if (clients < 1) {
    printf("Invalid clients count!\n");
    return -1;
  }
  key_t key = ftok(argv[0], 0);
  shared_memory *shared_mem_ptr;
  int semid;
  int shmid;

  int* children = (int*) malloc(sizeof(int) * clients);
  
  // Create memory and semaphore
  if ((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0) {
    printf("Can\'t create semaphore\n");
    return -1;
  }

  if ((shmid = shmget(key, sizeof(shared_memory), 0666 | IPC_CREAT)) < 0) {
    printf("Can\'t create shmem\n");
    return -1;
  }
  if ((shared_mem_ptr = (shared_memory*) shmat(shmid, NULL, 0)) == (shared_memory *) -1) {
    printf("Cant shmat!\n");
    return -1;
  } 
  semctl(semid, 0, SETVAL, 1);

  shared_mem_ptr->current_client = 0;

  // create child processes
  for (int i = 0; i < clients; i++) {
    children[i] = fork();
    if (children[i] == 0) {
      child(semid, shared_mem_ptr);
    }
  }

  printf("I am a Cutter\n");
  
  while (keepRunning) {
    int clid = shared_mem_ptr->current_client;
    if (clid == 0) {
      printf("[Cutter] No new clients\n");
      sleep(2);
    } else {
      printf("[Cutter] new client #%d\n", clid);
      sleep(5);
      printf("[Cutter] finished client #%d\n", clid);
      shared_mem_ptr->current_client = 0;
      sem_open(semid);
    }

  }

  // Kill children if alive
  for (int i = 0; i < clients; i++) {
    kill(children[i], SIGTERM);
    printf("[Cutter] Killing child #%d\n", children[i]);
  }

  // Clean up memory and delete shmem and sem
  free(children);

  if (semctl(semid, 0, IPC_RMID, 0) < 0) {
	printf("Can\'t delete semaphore\n");
  	return -1;
  }
  shmdt(shared_mem_ptr);
  shmctl(shmid, IPC_RMID, NULL);
  return 0;
}