#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

typedef struct shared_memory {
  sem_t mutex;
  int value;
}shared_memory;

shared_memory* open_shmem(char *memn) {
    int mem_size = sizeof(shared_memory);
    int shm;
    if ((shm = shm_open(memn, O_CREAT | O_RDWR, 0666)) == -1) {
        printf("Object is already open\n");
        perror("shm_open");
        exit(1);
    } else {
        printf("Object is open: name = %s, id = 0x%x\n", memn, shm);
    }
    if (ftruncate(shm, mem_size) == -1) {
        printf("Memory sizing error\n");
        perror("ftruncate");
        exit(1);
    } else {
        printf("Memory size set and = %d\n", mem_size);
    }
    void* addr = mmap(0, mem_size, PROT_WRITE, MAP_SHARED, shm, 0);
    if (addr == (int * ) - 1) {
        printf("Error getting pointer to shared memory\n");
        exit(1);
    }
    shared_memory* shmem = addr;
    return shmem;
}

int open_pipe(char* name) {
    mknod(name, S_IFIFO | 0666, 0);
    int desc;
    if((desc = open(name, O_RDWR)) < 0){
        printf("Can\'t open FIFO\n");
        exit(1);
    }
    return desc;
}

void send(int desc) {
    int buffer[] = {0, getpid()};
    int size = sizeof(int) * 2;
    write(desc, buffer, size);
}

void recv(int desc) {
    int buffer[2];
    int size = sizeof(int) * 2;
    read(desc, buffer, size);
}

int main() {
    sem_t *mutex;
    int pipe_desc;


    
    
    shared_memory* shmem = open_shmem("shared-memory-10");
    pipe_desc = open_pipe("pipe.fifo");

   
    
    printf("[%d] I am new client, waiting in queue\n", getpid());
    sem_wait(&shmem->mutex);
    printf("[%d] Got to cutter\n", getpid());
    send(pipe_desc);
    sleep(2);
    recv(pipe_desc);
    printf("[%d] Leaving cutter\n", getpid());
    sem_post(&shmem->mutex);
    close(pipe_desc);
    return 0;
}