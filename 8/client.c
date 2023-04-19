#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

typedef struct shared_memory {
  sem_t mutex;
  int current_client;
}shared_memory;

int main() {
  char memn[] = "shared-memory"; //  имя объекта
  int mem_size = sizeof(shared_memory);
  int shm;
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
  printf("I am a client #%d!\n", getpid());
  sem_wait(&shmem->mutex);
  printf("[%d] Going to cutter\n", getpid());
  shmem->current_client = getpid();
  return 0;
}