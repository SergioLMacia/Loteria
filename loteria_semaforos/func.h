#define DES 2
#define ASC 1
#define S_WAIT 1
#define S_SIGNAL 2
struct sigaction sa;
union sigval v;

union semun {
  int val;
  struct semid_ds buf;
  ushort *array;
};
typedef struct{
	pid_t pid;
	int num;
	long premio;
	int pipe[2];
	int maxnum;
	int childBet[6];
}HIJO;

//Genera una combinacion aleatoria de seis numeros
int randbet(int seed, int *bet);
	// (Usada por randbet): Comprueba que un numero no se repita en un vector
	int isavailable(int num, int *vector, int limit);
//Devuelve el premio segun los aciertos
int getmoney(int coincidences);

//Muestra la apuesta (debug)
void show_bet(int* bet);

//Ordena un vector de 6. (1->Ascendente o 2->Descendente)
int orderby(int order, int *vector);

//Vuelca la combinacion ganadora del padre en el pipe
void pipe_bet(int nhijos, HIJO *child, int *bet);

//Lee el contenido de un pipe y lo devuelve como resultado
int pipe_read(HIJO child);

// Comprueba si un valor existe en el array de los hijos y devuelve 1. si no, devuelve 0
int check(int n, int *childBet, int *pos);

//Generar el archivo SNR con los resultados del sorteo (N es el numero de sorteo)
int makesnr(HIJO *childArray, int game, int players);

//Imprime los errores y devuelve el estado de salida del programa
int printerr(int e);

// Obtiene una key para el semáforo
key_t set_semkey(const char *path, int id);

// Crea un array de semáforos y los inicializa
int createsem(struct sembuf *sbuf, int *semid, int key, unsigned n, unsigned val);

// Decrementa en 1 el semaforo
int semwait(struct sembuf *sbuf, int semid, unsigned nsem);

// Incrementa en 1 el semaforo
int semsignal(struct sembuf *sbuf, int semid, unsigned nsem);

// Incrementa o decrementa todo un array de semáforos
int sem_allset(unsigned type, unsigned nhijos, struct sembuf *sinc, int semid);