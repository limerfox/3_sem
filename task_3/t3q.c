
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <string.h>
#include <semaphore.h>

#define sem_name "sm1"
#define BILLION 1000000000L
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
    struct timespec start_tm, stop_tm;
    double accum_time;
    int msqid, msqid_r;
    int msgflg = IPC_CREAT | 0666;
    key_t key;
    message_t sbuf;

    char data[MESSAGE_SIZE];
    char ret_data[MESSAGE_SIZE];
    int size = sizeof(data)-1;
    for (size_t i = 0; i < size; i++)
    {
        data[i] = (char)(i%26 + 65);
    }
    data[size] = '\0';
    
    key = k1;
    if ((msqid = msgget(key, msgflg )) < 0) {
        perror("msgget1");
        exit(1);
    }
    else 
        printf("msgget: msgget succeeded: msqid = %d\n", msqid);
    
    key = k2;
    if ((msqid_r = msgget(key, msgflg )) < 0) {
        perror("msgget1");
        exit(1);
    }
    else 
        printf("msgget: msgget succeeded: msqid_r = %d\n", msqid_r);

    sbuf.mtype = 1;

    if((sm1 = sem_open(sem_name, 0)) == SEM_FAILED)
    {
        perror("sem_open");
    }
    sem_post(sm1);
    if(clock_gettime(CLOCK_MONOTONIC, &start_tm)== -1)
    {
        perror("clock_gettime");
        return EXIT_FAILURE;
    }
    for(int i = 0; i < MESSAGE_SIZE; i += BUFFER_SIZE) {
        memcpy(sbuf.mtext, &data[i], BUFFER_SIZE);
        if(msgsnd(msqid, &sbuf, BUFFER_SIZE, 0) < 0) {
            perror("msgsnd1\n");
            return -1;
        }
    } 

    for(int i = 0; i < MESSAGE_SIZE; i += BUFFER_SIZE) {
        if(msgrcv(msqid_r, &sbuf, BUFFER_SIZE, 1, 0) < 0) {
            perror("msgrcv1\n");
            return -1;
        }
        memcpy(&ret_data[i], sbuf.mtext, BUFFER_SIZE);
    }

    if(clock_gettime(CLOCK_MONOTONIC, &stop_tm)== -1)
    {
        perror("clock_gettime");
        return EXIT_FAILURE;
    }

    accum_time = (stop_tm.tv_sec - start_tm.tv_sec) 
    + (double)(stop_tm.tv_nsec - start_tm.tv_nsec)/(double)BILLION;

    printf("%lf seconds\n", accum_time);
    //printf("%s\n", ret_data);

    return 0;
}