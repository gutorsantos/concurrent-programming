#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "unistd.h"

#define N 10 //número de usuários

#define CARTAS 20 //quantidade de cartas na mochila


int cartas_mochila = 0;
pthread_mutex_t mochila_lock = PTHREAD_MUTEX_INITIALIZER;
sem_t sem_mochila;
sem_t pombo;
sem_t usuario;
sem_t posicao;

void * f_usuario(void *arg);
void * f_pombo(void *arg);

int main(int argc, char **argv){
    int i;
    

    sem_init(&sem_mochila, 0, 20);
    sem_init(&pombo, 0, 0);
    sem_init(&usuario, 0, 0);
    sem_init(&posicao, 0, 1);

    pthread_t usuario[N];
    int *id;
    for(i = 0; i < N; i++){
         id = (int *) malloc(sizeof(int));
          *id = i;
	pthread_create(&(usuario[i]),NULL,f_usuario,  (void *) (id));
    }
    pthread_t pombo;
    id = (int *) malloc(sizeof(int));
    *id = 0;
    pthread_create(&(pombo),NULL,f_pombo, (void*) (id));

    pthread_join(pombo,NULL);
   
}


void * f_pombo(void *arg){
  
    while(1){
        sem_wait(&pombo);
        printf("Pombo vooando para B!\n");
        sleep(5);

        //Leva as cartas para B e volta para A
        printf("Pombo ira descarregar as cartas em B!\n");
        for(int i = 0; i < CARTAS; i++) {
            pthread_mutex_lock(&mochila_lock);
            cartas_mochila--;
            sem_post(&sem_mochila);
            pthread_mutex_unlock(&mochila_lock);
            
        }
        printf("Pombo vooando para A!\n");
        sleep(5);
        sem_post(&usuario);
        sem_post(&posicao);
    }
}

void * f_usuario(void *arg){
	int id = *((int *) arg);
    while(1){
        printf("Usuario %d escreveu carta!\n", id);
        sleep(1);
        
        if(sem_trywait(&sem_mochila) == 0) {
            if (sem_trywait(&posicao) == 0) {
                sem_wait(&usuario);
            } else {    
                pthread_mutex_lock(&mochila_lock);
                cartas_mochila++;
                printf("Usuario %d colocou uma carta na mochila do pombo! Total de cartas na mochila: %d\n", id, cartas_mochila);
                pthread_mutex_unlock(&mochila_lock);
                sleep(0.5);
            }
        }else {
            printf("Mochila do pombo cheia! Aguardando esvaziar. \n");
            sem_post(&pombo);
            sem_wait(&usuario);
        }
        
    }
}
