#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

// status: 0 - cutter free; 1 - cuttting
// buf: conatins ids of new client
// when cutter is cutting he will close sem
// when free, open it
// first client to access open sem will change status of 
// cutter to cutting and close sem

static volatile int keepRunning = 1;

typedef struct shared_memory {
  int semid;
  int status;
  int current_client;
}shared_memory;


void intHandler(int dummy) {
    printf("[Cutter] SIGINT Detected!\n");
    keepRunning = 0;
}

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


int main(int argc, char ** argv) {
  signal(SIGINT, intHandler);
  key_t key = ftok("cutter.c", 0);
  shared_memory *shared_mem_ptr;
  int semid;
  int shmid;
  
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

  shared_mem_ptr->status = 0;
  shared_mem_ptr->current_client = 0;
  shared_mem_ptr->semid = semid;

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

  if (semctl(semid, 0, IPC_RMID, 0) < 0) {
	printf("Can\'t delete semaphore\n");
  	return -1;
  }
  shmdt(shared_mem_ptr);
  shmctl(shmid, IPC_RMID, NULL);
  return 0;
}