#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define PR 1 //número de produtores
#define CN 1 // número de consumidores
#define N 5  //tamanho do buffer

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
sem_t empty;
sem_t full;

int in = 0;
int out = 0;
int buffer[N] = {0};


void * produtor(void * meuid);
void * consumidor (void * meuid);
int read_data_base();
void use_data_read();
int think_up_data();
void write_data_base();


void main(argc, argv)
int argc;
char *argv[];
{

  int erro;
  int i, n, m;
  int *id;

  sem_init(&empty, 0, N);  
  sem_init(&full, 0, 0);

  pthread_t tid[PR];
   
  for (i = 0; i < PR; i++)
  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tid[i], NULL, produtor, (void *) (id));

    if(erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  pthread_t tCid[CN];

  for (i = 0; i < CN; i++)
  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tCid[i], NULL, consumidor, (void *) (id));

    if(erro)
    {
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }
  sem_destroy(&empty);  
  sem_destroy(&full);   
  pthread_join(tid[0],NULL);

}

void * produtor (void* pi) {
	int id = *((int *) pi);
  while(1) {
    int value = think_up_data(id);
    
    sem_wait(&empty);
    
    pthread_mutex_lock(&lock);

    write_data_base(id, in, value);
    in = (in+1)%N;

    pthread_mutex_unlock(&lock);
    
    sem_post(&full);

    int sem = 0;
    // checa se o buffer esta cheio
    sem_getvalue(&full, &sem);    
    if (sem == N) {            
      printf("BUFFER CHEIO!\n"); 
    }

  }
  pthread_exit(0);

}

void * consumidor (void* pi) {
  int id = *((int *) pi);
  while(1) {
    sem_wait(&full);
    
    pthread_mutex_lock(&lock);
    
    read_data_base(id, out);
    out = (out+1)%N;
    
    pthread_mutex_unlock(&lock);
    
    sem_post(&empty);
    

    int sem = 0;
    // checa se o buffer esta vazio
    sem_getvalue(&full, &sem);    
    if (sem == 0) {            
      printf("BUFFER VAZIO!\n", sem); 
    }

    use_data_read(id);

  }
  pthread_exit(0);
  
}

int read_data_base(int id, int index) {
	printf("Consumidor %d está lendo o dado %d no index %d!\n", id, buffer[index], index);
  sleep(rand() % 2 );
  return buffer[index];
}

void use_data_read(int id) {
	printf("Consumidor %d está usando o dado lidos\n", id);
	sleep(rand() % 2);
}

int think_up_data(int id) {
	printf("Produtor %d está pensando no que escrever!\n", id);
	sleep(rand() % 2);
  return (rand() % 10);
}

void write_data_base(int id, int index, int value) {
	printf("Produtor %d está escrevendo o dado %d no index %d!\n", id, value, index);
  buffer[index] = value;
  sleep( rand() % 2);
}