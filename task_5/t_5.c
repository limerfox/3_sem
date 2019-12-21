#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

int out_char = 0, counter = 128;
pid_t pid;

// Объявляем функции выполняемые по сигналам

// SIGCHLD
void childexit(int signo) {
  exit(EXIT_SUCCESS);
}

// SIGALRM
void parentexit(int signo) { 
  exit(EXIT_SUCCESS);
}

// Nothing
void empty(int signo) {
}

// SIGUSR1
void one(int signo) {
  out_char += counter;
  counter /= 2;	
  kill(pid, SIGUSR1);
}

// SIGUSR2
void zero(int signo) { 
  counter/=2;	
  kill(pid, SIGUSR1);
}


int main(int argc, char ** argv){
  if (argc != 3) {
    fprintf(stderr, "Use: %s [source] [destin]\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  pid_t ppid = getpid(); // Запонимаем пид родителя, то есть приёмника

  sigset_t set;

  // Изменяем набор блокированых сигналов

  // SIGUSR1 - one()
  struct sigaction act_one;
  memset(&act_one, 0, sizeof(act_one));
  act_one.sa_handler = one;
  sigfillset(&act_one.sa_mask);
  sigaction(SIGUSR1, &act_one, NULL);

  // SIGUSR2 - zero()
  struct sigaction act_zero;
  memset(&act_zero, 0, sizeof(act_zero));
  act_zero.sa_handler = zero;
  sigfillset(&act_zero.sa_mask);    
  sigaction(SIGUSR2, &act_zero, NULL);
 
  // при SIGCHLD - выходим
  struct sigaction act_exit;
  memset(&act_exit, 0, sizeof(act_exit));
  act_exit.sa_handler = childexit; 
  sigfillset(&act_exit.sa_mask); 
  sigaction(SIGCHLD, &act_exit, NULL); 


  sigemptyset(&set);

  // Добавляем блокировки
  sigaddset(&set, SIGUSR1);
  sigaddset(&set, SIGUSR2);
  sigaddset(&set, SIGCHLD);
  sigprocmask(SIG_BLOCK, &set, NULL );
  sigemptyset(&set);

  pid = fork();

  if(pid < 0)
  {
    perror("fork");
    exit(EXIT_FAILURE);
  }
  // Ребёнок (Передатчик)
  if (pid) 
  {
    errno = 0;
    unsigned int fout = open(argv[2], O_CREAT | O_RDWR, __S_IREAD, __S_IWRITE);
    do {	
        if(counter == 0){       // Whole byte
            write(fout, &out_char, 1);  //        
            counter=128;
            out_char = 0;
        }
        sigsuspend(&set); // Ждём сигнал от ребёнка
    } while (1);

    exit(EXIT_SUCCESS);
  }
  else 
  {
    unsigned int fin = 0;
    char c = 0;
    int i;
    
    sigemptyset(&set); // очищает набор сигналов
    // SIGUSR1 - empty()
    struct sigaction act_empty;                    
    memset(&act_empty, 0, sizeof(act_empty));
    act_empty.sa_handler = empty;
    sigfillset(&act_empty.sa_mask);    
    sigaction(SIGUSR1, &act_empty, NULL);

    // SIGALRM - parentexit()
    struct sigaction act_alarm;
    memset(&act_alarm, 0, sizeof(act_alarm));
    act_alarm.sa_handler = parentexit;
    sigfillset(&act_alarm.sa_mask);
    sigaction(SIGALRM, &act_alarm, NULL);

    fin = open(argv[1], O_RDONLY);
    if (fin  < 0 ){
      perror("Can't open file");
      exit(EXIT_FAILURE);
    }

    while (read(fin, &c, 1) > 0){	
      // SIGALRM Будет получен если родитель не успеет ответить за секунду
      alarm(1);
      // Побитовые операции
      for ( i = 128; i >= 1; i /= 2){
        if ( i & c )              // 1 
          kill(ppid, SIGUSR1);
        else                      // 0 
          kill(ppid, SIGUSR2);
        // Ждём подтверждения от родителя	
        // приостанавливает процесс до получения сигнала
        sigsuspend(&set); 
      } 
    }
    close(fin);
    exit(EXIT_SUCCESS);
  }


}
