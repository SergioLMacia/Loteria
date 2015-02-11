#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "func.h"



int main(int argc, char *argv[])
{

  // Control de parametros
  if(argc != 3){
    return printerr(1);
  }else{
    int arg1= atoi(argv[1]);
    int arg2= atoi(argv[2]);
    if( arg1 < 1 || arg1 > 15){
      return printerr(2);
    }
    else if( arg2 < 1 || arg2 > 10){
      return printerr(3);
    }
  }

	int rootBet[6]; 
	HIJO *child; 
	int results = 0; 
	int i=0, j=0;
	int nhijos = atoi(argv[2]);
  pid_t pid, wpid; 
  int semid1, semid2;
  struct sembuf sinc1, sinc2;

//Se intenta crear los semaforos. Si fallan, sale del programa
  if(!createsem(&sinc1, &semid1, getpid(), nhijos, 0) || !createsem(&sinc2, &semid2, getpid()+1, nhijos, 0) ){
      return EXIT_FAILURE;
  }

  child = (HIJO*) malloc( atoi(argv[2])*sizeof(HIJO) );
  for(i=0;i<atoi(argv[2]);i++){
    pipe(child[i].pipe);
  }

// Bucle for para crear los hijos mediante fork
  for(i=0;i<nhijos; i++){
    pid = fork();
    //Error en fork
    if(pid<0) return printerr(4);

    if(pid == 0){ 

// ****** Empieza el codigo del hijo

      close(child[i].pipe[1]); //cierra escritura ya que solo leeremos
      int betCounter = 0;
      
      // Se intenta generar la combinacion ganadora, si hay error, el hijo termina devolviendo 0 aciertos...
      if(!randbet(getpid(),child[i].childBet)){
          printerr(5);
          exit(0);
      }
      orderby(ASC,child[i].childBet);
      child[i].maxnum=child[i].childBet[5];

      /*SINCRONIZACION 1: Notifica al padre para que sepa que está listo*/
      if(!semsignal(&sinc1, semid1, i)){ // Se intenta lanzar el SIGNAL
        printerr(7);
        exit(0);
      }

      /*SINCRONIZACION 2: Espera a que el padre le deje leer*/
      if(!semwait(&sinc2, semid2, i)){ // Se intenta lanzar el WAIT
        printerr(7);
        exit(0);
      }
      betCounter = pipe_read(child[i]);
      close(child[i].pipe[0]);
      close(child[i].pipe[1]);
      exit(betCounter); 
// ****** El codigo del hijo termina aqui

    }else{ 
// ****** Codigo del padre, guardamos datos del hijo
      child[i].pid=pid;
      child[i].num=i;
    }
  }

/******* A este codigo solo llega el padre *******/

// SINCRONIZACION 1: Se intenta realizar un wait en todo el array. Si falla, sale del programa...
  if(!sem_allset(S_WAIT, nhijos, &sinc1, semid1)){
    return printerr(7);
  }
  randbet(getpid(),rootBet);
  orderby(ASC, rootBet);

  // Vuelca la combinación ganadora en los pipes
  pipe_bet(nhijos,child,rootBet);

// SINCRONIZACION 2: Se intenta realizar el signal a todo el array. Si falla, sale del programa...
  if(!sem_allset(S_SIGNAL, nhijos, &sinc2, semid2)){ 
    return printerr(7);
  }

//Bucle para esperar la salida de los hijos con sus aciertos y guardar el premio
  for(i=0;i<nhijos;i++){
    wpid=wait(&results);
    for(j=0;j<nhijos;j++){
      if(child[j].pid == wpid){
        child[j].premio = (long) getmoney ((results / 256));
      }
    }
  }

// Se intenta realizar el archivo SnR
  if(!makesnr(child, atoi(argv[1]), atoi(argv[2]))) return printerr(6);

// Liberamos los semaforos
  semctl(semid1, 0, IPC_RMID);
  semctl(semid2, 0, IPC_RMID);

  printf("Hecho\n");
  return EXIT_SUCCESS;

}
