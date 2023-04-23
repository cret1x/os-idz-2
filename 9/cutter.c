#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>


#define SEND_KEY 451
#define RECV_KEY 452
#define RECV_TYPE 1
#define MSGSZ 1
#define RMSGSZ sizeof(int) + sizeof(ssize_t)

typedef struct msgbuf
{
    long mtype;
    ssize_t msize;
    int mtext[MSGSZ];
} msgbuf;

static volatile int keepRunning = 1;

// Function to handle SIGINT
void intHandler(int dummy) {
    printf("[Cutter] SIGINT Detected!\n");
    keepRunning = 0;
}

void sem_close(int semid, int semnum) {
	struct sembuf parent_buf = {
    	.sem_num = semnum,
    	.sem_op = -1,
    	.sem_flg = 0
  	};
  	if (semop(semid, & parent_buf, 1) < 0) {
		printf("Can\'t sub 1 from semaphor\n");
    	exit(1);
  	}
}

// Function to +1 semaphore
void sem_open(int semid, int semnum) {
 	struct sembuf parent_buf = {
    	.sem_num = semnum,
    	.sem_op = 1,
    	.sem_flg = 0
  	};
  	if (semop(semid, & parent_buf, 1) < 0) {
    	printf("Can\'t add 1 to semaphor\n");
    	exit(1);
  	}  
}

int create_sem() {
	key_t key = ftok("cutter.c", 0);
	int semid;
  	if ((semid = semget(key, 1, 0666 | IPC_CREAT)) < 0) {
    	printf("Can\'t create semaphore\n");
    	exit(1);
  	}
  	semctl(semid, 0, SETVAL, 1);
  	return semid;
}

void delete_sem(int semid) {
	if (semctl(semid, 0, IPC_RMID, 0) < 0) {
		printf("Can\'t delete semaphore\n");
  		exit(1);
  	}
}

int create_queue(int key) {
	int qid;
	if ((qid = msgget(key, IPC_CREAT | 0666)) < 0) {
		printf("Error creating quque with key: %d\n", key);
		exit(1);
	} else {
		printf("Created queue with id: %d\n", qid);
	}
	return qid;
}

void recv(int qid, msgbuf *buffer) {
	if (msgrcv(qid, buffer, RMSGSZ, RECV_TYPE, 0) < 0) {
        printf("Error recieving message!\n");
        exit(1);
    }
}

void send(int qid, msgbuf *buffer) {
	if (msgsnd(qid, buffer, RMSGSZ, IPC_NOWAIT) < 0) {
    	printf("Error sending message!\n");
    	exit(1);
    }
}

int main() {
	signal(SIGINT, intHandler);
	int send_qid;
	int recv_qid;
	int sem_id;
	size_t buffer_len;
	ssize_t read_bytes;
	ssize_t write_bytes;
	struct msqid_ds qinfo;
	msgbuf send_buffer;
    msgbuf recv_buffer;

    int msg[] = {-1};

	send_qid = create_queue(SEND_KEY);
	recv_qid = create_queue(RECV_KEY);
	sem_id = create_sem();


	while (keepRunning) {
		recv(recv_qid, &recv_buffer);
		int client_id = recv_buffer.mtext[0];
		printf("[Cutter] New client with id: %d\n", client_id);
		sleep(5);
		printf("[Cutter] Finished client with id: %d\n", client_id);
		send_buffer.mtype = client_id;
		send_buffer.msize = 1;
		memcpy(send_buffer.mtext, msg, sizeof(int));
		send(send_qid, &send_buffer);
	}

	delete_sem(sem_id);
	return 0;
}