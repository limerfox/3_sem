#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <semaphore.h>

#define sem_name "sm1"
// #define MESSAGE_SIZE 256
// #define MESSAGE_SIZE 4096
#define MESSAGE_SIZE 16384
#define BUFFER_SIZE 1024

typedef struct mymsgbuf{
	long mtype;
	char mtext[BUFFER_SIZE];
} message_t;

int main(int argc, char * argv[])
{
    int k1 = atoi(argv[1]);
    int k2 = atoi(argv[2]);

    sem_t * sm1;
    int msqid, msqid_r;
    int msgflg = IPC_CREAT | 0666;
    key_t key, key_r;
    message_t  rbuf;
    char data[MESSAGE_SIZE];
    if((sm1 = sem_open(sem_name, O_CREAT, 0777, 0)) == SEM_FAILED)
    {
        perror("sem_open");
    }
    sem_wait(sm1);
    key = k1;
    if ((msqid = msgget(key, 0666)) < 0) {
        perror("msgget2");
        exit(1);
    }
    key = k2;
    if ((msqid_r = msgget(key, 0666)) < 0) {
        perror("msgget2");
        exit(1);
    }

    for(int i = 0; i < MESSAGE_SIZE; i += BUFFER_SIZE) {
        if(msgrcv(msqid, &rbuf, BUFFER_SIZE, 1, 0) < 0) {
            perror("msgrcv2\n");
            return -1;
        }
        memcpy(&data[i], rbuf.mtext, BUFFER_SIZE);
    }

    for(int i = 0; i < MESSAGE_SIZE; i += BUFFER_SIZE) {
        memcpy(rbuf.mtext, &data[i], BUFFER_SIZE);
        if(msgsnd(msqid_r, &rbuf, BUFFER_SIZE, 0) < 0) {
            perror("msgsnd2\n");
            return -1;
        }
    }
    return 0;
}