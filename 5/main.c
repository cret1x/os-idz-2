#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

// status: 0 - cutter free; 1 - cuttting
// buf: conatins ids of new client
// when cutter is cutting he will close sem
// when free, open it
// first client to access open sem will change status of 
// cutter to cutting and close sem

static volatile int keepRunning = 1;

typedef struct shared_memory {
  sem_t mutex;
  int current_client;
}shared_memory;


void intHandler(int dummy) {
    printf("[Cutter] SIGINT Detected!\n");
    keepRunning = 0;
}


void child(shared_memory *shmem)  {
  srand(time(NULL) ^ (getpid()<<16));
  printf("I am a client #%d!\n", getpid());
  sleep(rand() % 10 + 2);
  printf("[%d] In queue for cutter\n", getpid());
  sem_wait(&shmem->mutex);
  printf("[%d] Going to cutter\n", getpid());
  shmem->current_client = getpid();
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
  int* children = (int*) malloc(sizeof(int) * clients);
  char memn[] = "shared-memory"; //  имя объекта
  int mem_size = sizeof(shared_memory);
  int shm;
  sem_t mutex;

  // СОздать память
  if ((shm = shm_open(memn, O_CREAT | O_RDWR, 0666)) == -1) {
      printf("Object is already open\n");
      perror("shm_open");
      return 1;
  } else {
      printf("Object is open: name = %s, id = 0x%x\n", memn, shm);
  }
  if (ftruncate(shm, mem_size) == -1) {
      printf("Memory sizing error\n");
      perror("ftruncate");
      return 1;
  } else {
      printf("Memory size set and = %d\n", mem_size);
  }

  //получить доступ к памяти
  void* addr = mmap(0, mem_size, PROT_WRITE, MAP_SHARED, shm, 0);
  if (addr == (int * ) - 1) {
      printf("Error getting pointer to shared memory\n");
      return 1;
  }

  shared_memory* shmem = addr;
  sem_init(&shmem->mutex, 1, 1);
  shmem->current_client = 0;


  // create child processes
  for (int i = 0; i < clients; i++) {
    children[i] = fork();
    if (children[i] == 0) {
      child(shmem);
    }
  }

  printf("I am a Cutter\n");
  
  while (keepRunning) {
    int clid = shmem->current_client;
    if (clid == 0) {
      printf("[Cutter] No new clients\n");
      sleep(2);
    } else {
      printf("[Cutter] new client #%d\n", clid);
      sleep(5);
      printf("[Cutter] finished client #%d\n", clid);
      shmem->current_client = 0;
      sem_post(&shmem->mutex);
    }

  }
  for (int i = 0; i < clients; i++) {
    kill(children[i], SIGTERM);
    printf("[Cutter] Killing child #%d\n", children[i]);
  }
  free(children);
  sem_destroy(&shmem->mutex);
  close(shm);
  // удалить выделенную память
  if(shm_unlink(memn) == -1) {
    printf("Shared memory is absent\n");
    perror("shm_unlink");
    return 1;
  }
  return 0;
}