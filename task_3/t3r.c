#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define FIFO_F "fifo_forward"
#define FIFO_R "fifo_reverse"
#define MESSAGE_SIZE 256
// #define MESSAGE_SIZE 4096
// #define MESSAGE_SIZE 16384

int main()
{
    mknod(FIFO_F, __S_IFIFO | 0666, 0);
    int fd = open(FIFO_F, O_RDONLY);
    printf("Second waiting for a writer\n");    
    mknod(FIFO_R, __S_IFIFO | 0666, 0);
    int rd = open(FIFO_R, O_WRONLY);
    printf("Second waiting for a reader\n");    

    char buff[MESSAGE_SIZE];
    int size = read(fd, buff, sizeof(buff)-1); 
    buff[size] = '\0';
    write(rd, buff, size);

    //printf("Recieved: %s", buff);
    // printf("Retur: %s", buff);

    return 0;
}