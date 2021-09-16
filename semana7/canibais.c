#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAXCANIBAIS 20

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_canibal = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t canibal_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cozinheiro_cond = PTHREAD_COND_INITIALIZER;

void *canibal(void*meuid);
void *cozinheiro(int m);

int comida_cont = 0;  // contador

void main(argc, argv)
int argc;
char *argv[];
{
  int erro;
  int i, n, m;
  int *id;

  pthread_t tid[MAXCANIBAIS];

  if(argc != 3){
    printf("erro na chamada do programa: jantar <#canibais> <#comida>\n");
    exit(1);
  }
  
  n = atoi (argv[1]); //número de canibais
  m = atoi (argv[2]); // quantidade de porções que o cozinheiro consegue preparar por vez
  printf("numero de canibais: %d -- quantidade de comida: %d\n", n, m);

  if(n > MAXCANIBAIS){
    printf("o numero de canibais e' maior que o maximo permitido: %d\n", MAXCANIBAIS);
    exit(1);
  }
  
  for (i = 0; i < n; i++)  {
    id = (int *) malloc(sizeof(int));
    *id = i;
    erro = pthread_create(&tid[i], NULL, canibal, (void *) (id));

    if(erro){
      printf("erro na criacao do thread %d\n", i);
      exit(1);
    }
  }

  cozinheiro(m);
}

void * canibal (void* pi){
  while(1) {
    pthread_mutex_lock(&lock);

    //pegar uma porção de comida e acordar o cozinheiro se as porções acabaram
    while(comida_cont == 0) { 
      printf("Acabou a comida!!\n");
      pthread_cond_signal(&cozinheiro_cond);  // acorda cozinheiro
      pthread_cond_wait(&cozinheiro_cond, &lock); // dorme canibal
    }
    comida_cont--;
    pthread_mutex_unlock(&lock);
    
    printf("%d: vou comer a porcao que peguei\n", *(int *)(pi));
    sleep(10);

  }
  
}

void *cozinheiro (int m){
  while(1){
    int porcoes = m;
    pthread_mutex_lock(&lock);

    //dormir enquanto tiver comida
    while(comida_cont > 0) { 
      pthread_cond_wait(&cozinheiro_cond, &lock);
    }
    printf("cozinheiro: vou cozinhar\n");
    comida_cont = porcoes;
    sleep(20);
    
    if(comida_cont == porcoes) {
      printf("Comida está pronta!\n");
      pthread_cond_broadcast(&canibal_cond);
    }
    pthread_mutex_unlock(&lock);
    //acordar os canibais
   }

}
