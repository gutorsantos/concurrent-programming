#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define PR 1 //número de produtores
#define CN 1 // número de consumidores
#define N 5  //tamanho do buffer

pthread_mutex_t lock_buffer = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_nl = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_ne = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_producer = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_consumer = PTHREAD_COND_INITIALIZER;

int count_buffer = 0;

void * produtor(void * meuid);
void * consumidor (void * meuid);
void read_data_base();
void use_data_read();
void think_up_data();
void write_data_base();


void main(argc, argv)
int argc;
char *argv[];
{

  int erro;
  int i, n, m;
  int *id;

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
 
  pthread_join(tid[0],NULL);

}

void * produtor (void* pi) {
	int id = *((int *) pi);
  while(1) {
    think_up_data(id);
	  pthread_mutex_lock(&lock_buffer);

    // caso o buffer esteja cheio, va dormir
    if(count_buffer == N) {
      printf("buffer cheio!");
      pthread_cond_wait(&cond_producer, &lock_buffer);
    }

    // regiao critica
    write_data_base(id);
    count_buffer++;     // incrementa quantidade dos itens no buffer

    // caso o buffer esteja não vazio, acorde o consumidor
    if(count_buffer == 1) {
      pthread_cond_signal(&cond_consumer);
    }

	  pthread_mutex_unlock(&lock_buffer);

  }
  pthread_exit(0);

}

void * consumidor (void* pi) {
	int id = *((int *) pi);
  while(1) {
	  pthread_mutex_lock(&lock_buffer);

    // caso o buffer esteja vazio, va dormir
    if(count_buffer == 0) {
      pthread_cond_wait(&cond_consumer, &lock_buffer);
    }

    // regiao critica
    read_data_base(id);
    count_buffer--;

     // caso o buffer esteja vazio, acorde o produtor
    if(count_buffer == N-1) {
      pthread_cond_signal(&cond_producer);
    }

	  pthread_mutex_unlock(&lock_buffer);

    use_data_read(id);
  }
  pthread_exit(0);
  
}

void read_data_base(int i) {
	printf("Leitor %d está lendo os dados! Número de leitores: %d\n", i);
	sleep(rand() % 5 );
}

void use_data_read(int i) {
	printf("Leitor %d está usando os dados lidos! Número de leitores: %d\n", i);
	sleep(rand() % 5);
}

void think_up_data(int i) {
	printf("Escritor %d está pensando no que escrever!\n", i);
	sleep(rand() % 5);
}

void write_data_base(int i) {
	printf("Escritor %d está escrevendo os dados! Número de leitores: %d\n", i);
	sleep( rand() % 5 + 2);
}