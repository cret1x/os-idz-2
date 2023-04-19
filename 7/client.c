#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


typedef struct shared_memory {
  int semid;
  int status;
  int current_client;
}shared_memory;

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

int main() {
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
  
  printf("I am a client #%d!\n", getpid());
  sem_close(semid);
  printf("[%d] Going to cutter\n", getpid());
  shared_mem_ptr->current_client = getpid();
  return 0;
}