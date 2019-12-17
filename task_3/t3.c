#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BILLION 1000000000L
#define FIFO_F "fifo_forward"
#define FIFO_R "fifo_reverse"
#define MESSAGE_SIZE 256
// #define MESSAGE_SIZE 4096
// #define MESSAGE_SIZE 16384

int main()
{
    struct timespec start_tm, stop_tm;
    double accum_time;

    mknod(FIFO_F, __S_IFIFO | 0666, 0);
    int fd = open(FIFO_F, O_WRONLY);
    printf("First waiting for a reader\n");    
    mknod(FIFO_R, __S_IFIFO | 0666, 0);
    int rd = open(FIFO_R, O_RDONLY);
    printf("First waiting for a writer\n");    

    char buff[MESSAGE_SIZE];
    char bufr[MESSAGE_SIZE];
    int size = sizeof(buff)-1;
    for (size_t i = 0; i < size; i++)
    {
        buff[i] = (char)(i%26 + 65);
    }
    buff[size] = '\0';
    //printf("Sended: %s", buff);
    if(clock_gettime(CLOCK_MONOTONIC, &start_tm)== -1)
    {
        perror("clock_gettime");
        return EXIT_FAILURE;
    }

    write(fd, buff, size);
    size = read(rd, bufr, sizeof(bufr)-1); 
    
    if(clock_gettime(CLOCK_MONOTONIC, &stop_tm)== -1)
    {
        perror("clock_gettime");
        return EXIT_FAILURE;
    }
    bufr[size] = '\0';
    accum_time = (stop_tm.tv_sec - start_tm.tv_sec) 
    + (double)(stop_tm.tv_nsec - start_tm.tv_nsec)/(double)BILLION;

    printf("%lf seconds\n", accum_time);
    //printf("Returned: %s", bufr);

    return 0;
}