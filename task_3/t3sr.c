#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <string.h>
#include <semaphore.h>

#define sem_name "sm1"
// #define MESSAGE_SIZE 256
// #define MESSAGE_SIZE 4096
#define MESSAGE_SIZE 16384
#define BUFFER_SIZE 1024


int main(int argc, char * argv[])
{
    int k1 = atoi(argv[1]);
    int k2 = atoi(argv[2]);

    sem_t * sm1;
    int shmid, shmid_r;
    char * shm_ptr, * shm_ptr_r;
    int msgflg = IPC_CREAT | 0666;

    char data[MESSAGE_SIZE];
    if((sm1 = sem_open(sem_name, O_CREAT, 0777, 0)) == SEM_FAILED)
    {
        perror("sem_open");
    }
    sem_wait(sm1);

    key_t key;
    key = k1;
    if ((shmid = shmget(key, 1, msgflg)) < 0) {
        perror("shmget1");
        exit(1);
    }
    else 
        printf("shmid: shmget succeeded: shmget = %d\n", shmid);
    shm_ptr = shmat(shmid, NULL, 0);

    key = k2;
    if ((shmid_r = shmget(key, 1, msgflg)) < 0) {
        perror("shmget1");
        exit(1);
    }
    else 
        printf("shmid_r: shmget succeeded: shmget = %d\n", shmid_r);
    shm_ptr_r = shmat(shmid_r, NULL, 0);


    for(int i = 0; i < MESSAGE_SIZE; i += BUFFER_SIZE) {
        while(!shm_ptr[BUFFER_SIZE]);
        memcpy(&data[i], shm_ptr, BUFFER_SIZE);
        shm_ptr[BUFFER_SIZE] = 0;
    }
    
    for(int i = 0; i < MESSAGE_SIZE; i += BUFFER_SIZE) {
        memcpy(shm_ptr_r, &data[i], BUFFER_SIZE);
        shm_ptr_r[BUFFER_SIZE] = 1;
        while(shm_ptr_r[BUFFER_SIZE]);
    } 
   
    return 0;
}