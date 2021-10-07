#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "unistd.h"

#define N 10 //número de usuários
#define CARTAS 20 //quantidade de cartas na mochila

int cartas_mochila = 0;
pthread_mutex_t mochila_lock = PTHREAD_MUTEX_INITIALIZER;

sem_t posicao;   

sem_t sem_mochila; 
sem_t sem_pombo;     

void * f_usuario(void *arg);
void * f_pombo(void *arg);

int main(int argc, char **argv){
    int i;
    int *id;

    sem_init(&posicao, 0, 1);
	sem_init(&sem_mochila, 0, CARTAS);
    sem_init(&sem_pombo, 0, 0);

    pthread_t usuario[N];
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
    int num_cartas = 0;

    while(1){
        sem_wait(&sem_pombo);
        sem_wait(&posicao);  
        printf("Pombo esta carregado!\n");  
        printf("Pombo vooando para B!\n");
        sleep(5);

        printf("Pombo ira descarregar as cartas em B!\n");
        for (int i = 0; i < CARTAS; i++) {
            sem_wait(&sem_mochila);
            pthread_mutex_lock(&mochila_lock);
            cartas_mochila--;
            pthread_mutex_unlock(&mochila_lock);
        }
        sleep(2);

        printf("Pombo vooando para A!\n");
        sem_post(&posicao);

    }
    pthread_exit(0);
}

void * f_usuario(void *arg){
    int id = *((int *) arg);
    int num_cartas = 0;

    while(1){
        printf("Usuario %d escreveu carta!\n", id);
        sleep(2);

        sem_wait(&posicao); 
        sem_post(&sem_mochila);

        pthread_mutex_lock(&mochila_lock);
        cartas_mochila++;
        printf("Usuario %d colocou uma carta na mochila do pombo! Total de cartas na mochila: %d\n", id, cartas_mochila);
        pthread_mutex_unlock(&mochila_lock);
        
        
        if (cartas_mochila == CARTAS){
            printf("Mochila do pombo cheia! Aguardando esvaziar. \n");
            sem_post(&sem_pombo);
        }
        sem_post(&posicao);
    }
    pthread_exit(0);
}