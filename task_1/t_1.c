#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <limits.h>

int main() {
	int arg_max= sysconf(_SC_ARG_MAX);
	//int arg_max= 20;
	char* p;
	while(1)
	{
		int del_cnt =0;
		char delim [] = " ,\n";
		printf("Put the command\n");

		int arg_cnt = 0;
		char s [arg_max];
		char *ps;
		ps = s;
		while(arg_cnt++ < arg_max)
		{
			*ps=getchar ();
			if(*ps == '\n')
			{
				*ps = '\0';
				break;			
			}
			char *fnd = strchr(delim, *ps);
			if (fnd != NULL)
			{
				del_cnt++;
			}
			ps++;
		}

		if (strcmp(s, "exit") == 0)
		{
			return 0;
		}

		char *arr[del_cnt + 2];
		int i = 0;
		for (p = strtok(s, delim); p != NULL; p = strtok(NULL, delim))
		{
			arr[i++] = p;
		}
		arr[del_cnt + 1] = '\0';

		const pid_t pid = fork();
		if (pid < 0) {
			printf("fork() error\n");
			return -1;
		}
		if (pid) {
			int status;
			waitpid(pid, &status, 0);
			printf("Ret code: %d\n", WEXITSTATUS(status));
		} else {
			int ret = execvp(arr[0], arr);
			exit(ret);
		}
	}
	return 0;
}