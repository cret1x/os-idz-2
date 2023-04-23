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

static volatile int keepRunning = 1;
static char *memn = "shared-memory-10";
static char *sem_name = "sem-mutex-10";
static int pipe_desc;

typedef struct shared_memory {
    sem_t mutex;
    int value;
} shared_memory;

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


sem_t* create_sem(char sem_name[]) {
    sem_t *sem;
    if ((sem = sem_open(sem_name, O_CREAT, 0666, 1)) < 0) {
        printf("Error creating semaphore!\n");
        exit(1);
    }
    return sem;
}

void delete_sem(char sem_name[]) {
    if (sem_unlink(sem_name) == -1) {
        perror("sem_unlink"); 
        exit(1);
    }
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
    int buffer[] = {0, 0};
    int size = sizeof(int) * 2;
    write(desc, buffer, size);
}

void recv(int desc, int buffer[2]) {
    int size = sizeof(int) * 2;
    read(desc, buffer, size);
}

void intHandler(int dummy) {
    printf("[Cutter] SIGINT Detected!\n");
    keepRunning = 0;
    delete_sem(sem_name);
    close(pipe_desc);
    if(shm_unlink(memn) == -1) {
        printf("Shared memory is absent\n");
        perror("shm_unlink");
    }
    kill(getpid(), SIGTERM);
}


int main(int argc, char ** argv) {
    signal(SIGINT, intHandler);
    char *memn = "shared-memory-10";
    sem_t mutex;
    int buffer[2];


    pipe_desc = open_pipe("pipe.fifo");
    shared_memory* shmem = open_shmem(memn);
    shmem->mutex = *create_sem("sem-mutex-10");
    shmem->value = 228;

    while(keepRunning) {
        recv(pipe_desc, buffer);
        int client_id = buffer[1];
        printf("[Cutter] New client with id: %d\n", client_id);
        sleep(5);
        printf("[Cutter] Finished client with id: %d\n", client_id);
        send(pipe_desc);  
        sleep(2);  
    }
    return 0;
}