#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main()
{
    typedef struct {
        int txd[2]; 
        int rxd[2]; 
    } dpipe_t;

    dpipe_t fdd;
    char buf[4096]; 
    int size;
    //char buf1[] = "From child";

    while(1){

        size = read(0, buf, sizeof(buf)-1);
        buf[size] = 0;

        if (pipe(fdd.txd) < 0) {
            printf("Pipe creation is failed!\n");
            return -1;
        }

        if (pipe(fdd.rxd) < 0) {
            printf("Pipe creation is failed!\n");
            return -1;
        }

        const pid_t pid = fork();

        if (pid < 0) {
            printf("fork failed!\n");
            return -1;
        }
    
        if (pid) {
            close(fdd.txd[0]);
            close(fdd.rxd[1]);
                
            // size = read(0, buf, sizeof(buf)-1);
            // buf[size] = 0;
            write(fdd.txd[1], buf, size);
            printf("Send from parent: %s\n", buf);

            if (strncmp(buf, "exit", 4) == 0)
            {
                return 0;
            }

            int status;
            waitpid(pid, &status, 0);

            size = read(fdd.rxd[0], buf, sizeof(buf)-1);
            buf[size] = '\0';
            printf("Received from child: %s\n", buf);

            close(fdd.rxd[0]);
            close(fdd.txd[1]);

        }

        else {
            close(fdd.txd[1]);
            close(fdd.rxd[0]);

            size = read(fdd.txd[0], buf, sizeof(buf)-1);
            buf[size] = '\0';
            printf("Received from parent: %s\n", buf);

            if (strncmp(buf, "exit", 4) == 0)
            {
                close(fdd.rxd[1]);
                close(fdd.txd[0]);
                close(fdd.rxd[0]);
                close(fdd.txd[1]);
                return 0;
            }

            //// If want to change a message
            // int size1 = sizeof(buf1);
            // for (int i = 0; i < size1; i++)
            // {
            //     buf[size + i - 1] = buf1[i];
            // }
            
            write(fdd.rxd[1], buf, sizeof(buf));
            printf("Send from child: %s\n", buf);
            close(fdd.rxd[1]);
            close(fdd.txd[0]);
            exit(0);
        }
    }
  return 0;
}