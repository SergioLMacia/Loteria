#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "func.h"

extern unsigned int scount;

// 1(ASC) min to max : 2(DES) max to min 
int orderby(int order, int *vector){ 
  int i=0, j=0;
  int temp;

  if(order<1 || order>2 || !vector){
    errno=22;
      perror("Uso incorrecto de la funcion \"orderby()\"\nEmplee 1 para ordenar de minimo a maximo\nEmplee 2 para ordenar de maximo a minimo");
    return 0;
  }
  for(i=0;i<6;i++){
    for(j=0;j<5;j++){
      if(vector[j]>vector[j+1]){
        temp = vector[j];
        vector[j] = vector[j+1];
        vector[j+1] = temp;
      }
    }
  }    
  if(order==2){
    int vt[6];
    j=0;
    for(i=5;i>=0;i--){
      vt[i] = vector[j];
      j++;
    }
    for(i=0;i<6;i++){
      vector[i] = vt[i];
    }
  }
  return 1;
}
int isavailable(int num, int *vector, int limit){
  int i=0;
  for(i=0;i<limit;i++){
    if(vector[i] == num)
        return 0;
  }
  return 1;
}
int randbet(int seed, int *bet){
  
  int i, temp;
  srand(seed+1234567890);
  i=0; 
  while(i<6){
    temp = rand()%15 +1;
    if(!isavailable(temp, bet, i)){
      continue;
    }
    bet[i] = temp;
    i++;
  }
  if(!bet){
    return 0; // El codigo nunca deberia alcanzar este punto
  }
  return 1;
}

int getmoney(int coincidences){
  switch(coincidences){
    case 3: return 10;
    case 4: return 50;
    case 5: return 500;
    case 6: return 10000;
    default: return 0;
  }
}

void sighandler(int tsign, siginfo_t *info, void *other){
  switch(info->si_code){
    case SI_QUEUE: 
      if(tsign==SIGRTMIN)
        scount++;
      break;
    default:
      break;
  }
}

void show_bet(int* bet){
  
  int i=0;
  for(i=0;i<6;i++){
    printf("%d ", bet[i]);
  }
  printf("\n");
}


void pipe_bet(int nhijos, HIJO *child, int *bet){
  int i, j;
  for(i=0;i<nhijos;i++){
    close(child[i].pipe[0]); //cierra lectura (no vamos a leer nada)
    for(j=0;j<6;j++){
      write(child[i].pipe[1], &bet[j], sizeof(int));
    }
    close(child[i].pipe[1]); //cierra escritura
  }
}

int pipe_read(HIJO child){
  int i, temp, res=0, pos=0;
  close(child.pipe[1]); //cierra escritura (solo vamos a leer el volcado del padre)
  for(i=0;i<6;i++){
    read(child.pipe[0], &temp, sizeof(int));
    if(temp>child.maxnum){
      break;
    }else{
      res+=check(temp, child.childBet, &pos);
    }
  }
  close(child.pipe[0]); //cerramos el canal de lectura
  return res;
}

int check(int n, int *childBet, int *pos){
  int i;
  for(i=*pos; i<6; i++){
    if(childBet[i] == n){
      *pos= i;
      return 1;
    }
  }
  return 0;
}

int makesnr(HIJO *childArray, int game, int players){
  char nombre[5];
  FILE *f;
  int i;
  sprintf(nombre, "%s%d%s", "S", game, "R");
  f=fopen(nombre, "w");

  if(!f){
    return 0;
  }
  fprintf(f,"#Resultados del Sorteo %d\n",game);
  for(i=0;i<players;i++){
    fprintf(f, "%lu #Comentario %d (Para probar el ignorado de comentarios)\n", childArray[i].premio, i);    
  }
  fclose(f);
  return 1;
}

int printerr(int e){
  switch(e){
    case 0: return EXIT_SUCCESS; // No error
    case 1:                      // Error de parametros
      errno=22;
      perror("Parametros incorrectos"); 
      break;
    case 2:                      // Error con parametro sorteo
      errno=22;
      perror("El numero de sorteo debe estar entre 1 y 15");
      return EXIT_FAILURE;
    case 3:                      // Error con parametro jugadores
      errno=22;
      perror("El numero de jugadores debe estar entre 1 y 10");
      break;
    case 4:                      // Error creando fork
      printf("Error creando fork\n");
      break;
    case 5:                      // Error generando apuestas (preventivo, no debe aparecer)
      errno=14;
      perror("No se han podido generar las apuestas");
      break;
    case 6:                      // Error creando archivo
      errno=30;
      perror("No se ha podido crear el archivo.\nQuizas el programa o el usuario no tiene permisos de escritura en este directorio");
    default:                     // Error desconocido
      printf("Error de origen desconocido\n");
  }
  return EXIT_FAILURE;
}