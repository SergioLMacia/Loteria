#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h> 
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <fcntl.h>
#include <limits.h>

union semun {
  int val;
  struct semid_ds *buf;
  ushort *array;
};
/*
semid_ds
unsigned short sem_num;  // semaphore number
short          sem_op;   // semaphore operation
short          sem_flg;  // operation flags 
*/

void main(){

  key_t semkey;
  int semid, pfd, fv;
  struct sembuf sbuf;
  char *lgn;
  char filename[PATH_MAX+1];
  struct stat outstat;
  struct passwd *pw;

  /* Get unique key for semaphore. */
  if ((semkey = ftok("/tmp", 'a')) == (key_t) -1) {
      perror("IPC error: ftok"); exit(1);
  }


  /* Get semaphore ID associated with this key. */
  if ((semid = semget(semkey, 3, 0)) == -1) {
      /* Semaphore does not exist - Create. */
      if ((semid = semget(semkey, 3, IPC_CREAT | IPC_EXCL | 0664)) != -1){
          /* Initialize the semaphore. */
          int h;
          for (h = 0; h < 3; h++)
          {
            sbuf.sem_num = h;
            sbuf.sem_op = 0;  /* This is the number of runs without queuing. */
            sbuf.sem_flg = 0;
            if (semop(semid, &sbuf, 1) == -1) {
              perror("semop"); 
              exit(1);
            }
          }
          
      }else {
          perror("semget"); 
          exit(1);
      }
  }

printf("Semaforos creados\n");
int b;
for(b=0;b<3;b++){
  int pid = fork();
  if(pid == 0){
      printf("Hijo %d: duermo %d segundos\n", b, b+1);
      sleep(b+1);
      printf("Hijo %d: le doy senal a padre en mi array", b);
      sbuf.sem_num = b;
      sbuf.sem_op = 1;
      sbuf.sem_flg = 0;
      if (semop(semid, &sbuf, 1) == -1) {
          perror("IPC Error: semop"); exit(1);
      }
      printf("\t senal dada.\n");
    exit(0);
  }else{

  }
}
  int gh;
    for(gh=0;gh<3;gh++){
      printf("Padre intentando colgarse en el array de semaforos, en el hijo -> %d \n", gh);
      sbuf.sem_num = gh;
      sbuf.sem_op = -1;
      sbuf.sem_flg = 0;
      if (semop(semid, &sbuf, 1) == -1) {
          perror("IPC Error: semop"); exit(1);
      }
    }
    printf("el padre ha terminado\n");
  //sleep(5);
  semctl(semid, 0, IPC_RMID);
  return;
}
