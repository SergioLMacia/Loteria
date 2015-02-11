#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include "func.h"
unsigned int scount = 0;
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

  pid_t pid, wpid; 
  int rootBet[6]; 
  HIJO *child; 
  int results = 0;
  int i=0, j=0;
  int nhijos = atoi(argv[2]);

// Se crea el manejador de señales
  sa.sa_sigaction = sighandler;
  sa.sa_flags = SA_SIGINFO;
  sigaction(SIGRTMIN, &sa, NULL);

  child = (HIJO*) malloc( atoi(argv[2])*sizeof(HIJO) );
  for(i=0;i<atoi(argv[2]);i++){
    pipe(child[i].pipe);
  }

  for(i=0;i<nhijos; i++){
    pid = fork();
    //Error en fork
    if(pid<0) return printerr(4);

    if(pid == 0){ 
//****** Codigo del hijo

      close(child[i].pipe[1]); //cerramos escritura ya que solo leeremos
      int betCounter = 0;
      // Se intenta generar la apuesta
      if(!randbet(getpid(),child[i].childBet)){
          printerr(5);
          exit(0);
      }
      orderby(ASC,child[i].childBet);
      child[i].maxnum=child[i].childBet[5];

      // Envia señal de que ha creado combinacion para desbloquear al padre del pause
      v.sival_int=getpid();
      sigqueue(getppid(), SIGRTMIN, v);      
      
      // Intenta leer del pipe
      betCounter = pipe_read(child[i]);

      close(child[i].pipe[0]);
      close(child[i].pipe[1]);
      exit(betCounter); 
//****** El codigo del hijo termina aqui
    }else{ 
//****** Codigo del padre, guardamos datos del hijo
      child[i].pid=pid;
      child[i].num=i;
    }
  }


/******* A este codigo solo llega el padre *******/

//SINCRONIZACION 1: Espera a que terminen los hijos de generar las apuestas
while(scount<nhijos){
  pause();
}
randbet(getpid(),rootBet);
orderby(ASC, rootBet);
//Vuelca la combinación ganadora en el pipe
pipe_bet(nhijos, child,rootBet);

for(i=0;i<nhijos;i++){
// SINCRONIZACION 2: Espera que terminen de leer los hijos del pipe y guarda premios
  wpid=wait(&results);
  for(j=0;j<nhijos;j++){
    if(child[j].pid == wpid){
      child[j].premio = (long) getmoney ((results / 256));
    }
  }
}

// Intenta realizar el fichero snr
if(!makesnr(child, atoi(argv[1]), atoi(argv[2]))) return printerr(6);

printf("Hecho\n");
return EXIT_SUCCESS;

}
