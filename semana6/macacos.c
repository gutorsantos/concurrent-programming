#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MA 10 //macacos que andam de A para B
#define MB 10 //macacos que andam de B para A

pthread_mutex_t lock_corda = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_mab = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_mba = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_vez = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_gorila = PTHREAD_MUTEX_INITIALIZER;

int mab = 0;
int mba = 0;
int gorilak = 0;

/*
 *  Temos um lock vez para assegurar a vez dos macacos AB
 *  e temos outro lock para o numeros de macacos AB para podermos permitir
 *  que mais de um macaco atravesse na mesma direção, quando o primeiro macaco AB
 *  chegar, ele ira ativar o lock da corda para evitar que um macaco BA ou
 *  um gorila tente atrevessar conjuntamente. E quando o ultimo macaco AB sair da corda
 *  este ira liberar a corda para os demais.
 */
void * macacoAB(void * a) {
  sleep(rand() % 2);    // randomizar a saida
  int i = *((int *) a);    
  while(1){
    if(!gorilak){       // caso nao haja nenhum gorila querendo passar, os macacos AB irao passar
    pthread_mutex_lock(&lock_vez);
    pthread_mutex_lock(&lock_mab);
      mab++;
      if(mab == 1){
        pthread_mutex_lock(&lock_corda);
      }
    pthread_mutex_unlock(&lock_vez);
    pthread_mutex_unlock(&lock_mab);
      printf("Macaco %d passado de A para B \n",i);
      sleep(1);
    
    pthread_mutex_lock(&lock_mab);
      mab--;
      if(mab == 0){
        pthread_mutex_unlock(&lock_corda);
      }
    pthread_mutex_unlock(&lock_mab);
  }}
  pthread_exit(0);
}

/*
 *  Temos um lock vez para assegurar a vez dos macacos BA
 *  e temos outro lock para o numeros de macacos BA para podermos permitir
 *  que mais de um macaco atravesse na mesma direção, quando o primeiro macaco BA
 *  chegar, ele ira ativar o lock da corda para evitar que um macaco AB ou
 *  um gorila tente atrevessar conjuntamente. E quando o ultimo macaco BA sair da corda
 *  este ira liberar a corda para os demais.
 */
void * macacoBA(void * a) {
  sleep(rand() % 2);    // randomizar a saida
  int i = *((int *) a);    
  while(1){
    if(!gorilak){       // caso nao haja nenhum gorila querendo passar, os macacos BA irao passar
    pthread_mutex_lock(&lock_vez);
    pthread_mutex_lock(&lock_mba);
      mba++;
      if(mba == 1){
        pthread_mutex_lock(&lock_corda);
      }
    pthread_mutex_unlock(&lock_vez);
    pthread_mutex_unlock(&lock_mba);

    printf("Macaco %d passado de B para A\n",i);
    sleep(1);

    pthread_mutex_lock(&lock_mba);
      mba--;
      if(mba == 0){
        pthread_mutex_unlock(&lock_corda);
      }
    pthread_mutex_unlock(&lock_mba);
  }}
  pthread_exit(0);
}

/*
 *  Temos um lock vez para assegurar a vez do gorila AB
 *  e temos outro lock para garantir que a variavel gorilak seja modificada corretamente
 *  ela ira armazenar se tem um gorila com intençao de atravessar,
 *  quando o gorila AB atravessar, este ira definir a variavel gorilak como 0 pois
 *  como ele ja atravessou nao ha mais a intencao de passar, liberando para os demais
 */
void * gorilaBA(void * a) {
  sleep(rand() % 2);    // randomizar a saida
  while(1){
    pthread_mutex_lock(&lock_vez);
    pthread_mutex_lock(&lock_gorila);
			gorilak = 1;
    pthread_mutex_unlock(&lock_vez);
		pthread_mutex_unlock(&lock_gorila);

    printf("Gorila passado de B para A \n");
    sleep(5);
    //Procedimentos para quando sair da corda
    pthread_mutex_lock(&lock_gorila);
			gorilak = 0;
		pthread_mutex_unlock(&lock_gorila);
  }
  pthread_exit(0);
}

/*
 *  Temos um lock vez para assegurar a vez do gorila BA
 *  e temos outro lock para garantir que a variavel gorilak seja modificada corretamente
 *  ela ira armazenar se tem um gorila com intençao de atravessar,
 *  quando o gorila BA atravessar, este ira definir a variavel gorilak como 0 pois
 *  como ele ja atravessou nao ha mais a intencao de passar, liberando para os demais
 */
void * gorilaAB(void * a) {
  sleep(rand() % 2);    // randomizar a saida
  while(1){
    pthread_mutex_lock(&lock_vez);
    pthread_mutex_lock(&lock_gorila);
			gorilak = 1;
    pthread_mutex_unlock(&lock_vez);
		pthread_mutex_unlock(&lock_gorila);

    printf("Gorila passado de A para B \n");
    sleep(5);
    //Procedimentos para quando sair da corda
    pthread_mutex_lock(&lock_gorila);
			gorilak = 0;
		pthread_mutex_unlock(&lock_gorila);
  }
  pthread_exit(0);
}

int main(int argc, char * argv[])
{
  pthread_t macacos[MA+MB];
  int *id;
  int i = 0;

  for(i = 0; i < MA+MB; i++){
    id = (int *) malloc(sizeof(int));
    *id = i;
    if(i%2 == 0){
      if(pthread_create(&macacos[i], NULL, &macacoAB, (void*)id)){
        printf("Não pode criar a thread %d\n", i);
        return -1;
      }
    }else{
      if(pthread_create(&macacos[i], NULL, &macacoBA, (void*)id)){
        printf("Não pode criar a thread %d\n", i);
        return -1;
      }
    }
  }
  pthread_t gAB, gBA;
  pthread_create(&gAB, NULL, &gorilaAB, NULL);
  pthread_create(&gBA, NULL, &gorilaBA, NULL);


  pthread_join(macacos[0], NULL);
  return 0;
}
