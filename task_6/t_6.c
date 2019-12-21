#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <errno.h>

#define BUF_SIZE 4096

char *concat(char *str1, char *str2) {
    char *res = (char *) calloc(strlen(str1) + strlen(str2) + 1, sizeof(char));
    strcat(res, str1);
    strcat(res, str2);
    return res;
}


char *time_to_str(time_t time) {
    char *str = asctime(localtime(&time));
    str[strlen(str) - 1] = ' ';
    return str;
}

void work(int fd, char *dir_path) {
    FILE *file = fdopen(fd, "r");

    struct dirent *dp;
    DIR *dir;
    if ((dir = opendir(dir_path)) == NULL) {
        perror("Error: cannot open director\n");
        exit(-1);
    }
    char *path;

    while ((dp = readdir(dir)) != NULL) {

        if (strcmp("backup", dp->d_name) == 0 || strcmp("..", dp->d_name) == 0 || strcmp(".", dp->d_name) == 0) {
            continue;
        }

        path = concat(dir_path, dp->d_name);
        char buff[BUF_SIZE];
        struct stat info;
        stat(path, &info);
        if (S_ISDIR(info.st_mode)) {
            sprintf(buff, "backup/%s", path);
            mkdir(buff, 0755);
            work(fd, path);
        } else {

            sprintf(buff, "file \"%s\"\n", path);
            system(buff);
            dprintf(STDERR_FILENO, "%s", buff);
            fgets(buff, BUF_SIZE, file);

            if (strstr(buff, "text") == NULL)
                continue;

            sprintf(buff, "diff -s -q -N \"backup/%s\" \"%s\"\n", &path[2], path);
            system(buff);
            dprintf(STDERR_FILENO, "%s", buff);
            fgets(buff, BUF_SIZE, file);
            dprintf(STDERR_FILENO, "%s", buff);

            if (strstr(buff, "identical"))
                continue;

            sprintf(buff, "cp \"%s\" \"backup/%s\"\n", path, &path[2]);
            system(buff);
            dprintf(STDERR_FILENO, "%s", buff);

            sprintf(buff, "cp \"backup/%s\" \"backup/%s-%s\"\n", &path[2], &path[2], time_to_str(time(0)));
            system(buff);
            dprintf(STDERR_FILENO, "%s", buff);
        }
    }
    dprintf(STDERR_FILENO, "\n\n");
}

int main() {
    int pid = fork();
    if (pid == -1) {
        printf("Error: cannot fork");
        exit(-1);
    }
    if (pid > 0) {
        return 0;
    }
    else
    {
        char log_name[] = "backup/log.txt";
        printf("Starting the daemon with PID: %d\n", getpid());

        mkdir("backup", 0755);
        errno = 0;

        setsid();
        sigset_t set;
        sigfillset(&set);
        sigprocmask(SIG_BLOCK, &set, NULL);

        int fd[2];
        pipe(fd);
        close(STDIN_FILENO);

        dup2(fd[1], STDOUT_FILENO);
        if (errno || (dup2(open(log_name, O_CREAT | O_WRONLY | O_APPEND, 0644), STDERR_FILENO) && errno)) {
            dprintf(STDERR_FILENO, "ERROR! errno == %d\n", errno);
            return 0;
        }

        while (1) {
            work(fd[0], "./");
            sleep(30);
        }
    }

}