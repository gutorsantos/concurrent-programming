#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "unistd.h"

#define N 10 //número de usuários

#define CARTAS 20 //quantidade de cartas na mochila

int posicao = 'a';
int cartas_mochila = 0;

pthread_cond_t c_usuario = PTHREAD_COND_INITIALIZER;
pthread_cond_t c_pombo = PTHREAD_COND_INITIALIZER;

pthread_mutex_t mochila = PTHREAD_MUTEX_INITIALIZER;

void * f_usuario(void *arg);
void * f_pombo(void *arg);

int main(int argc, char **argv){
    int i;

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
        pthread_mutex_lock(&mochila);

        if(cartas_mochila == CARTAS) {
            printf("Pombo esta carregado!\n");     
            printf("Pombo vooando para B!\n");
            sleep(5);

            posicao = 'b';
            sleep(2);

            printf("Pombo ira descarregar as cartas em B!\n");
            cartas_mochila = 0;
            sleep(2);
            
            printf("Pombo vooando para A!\n");
            posicao = 'a';
            sleep(2);

            if(posicao == 'a')
                pthread_cond_broadcast(&c_usuario);
        }

        pthread_mutex_unlock(&mochila);  
    }
}

void * f_usuario(void *arg){
	int id = *((int *) arg);
    while(1){

        printf("Usuario %d escreveu carta! Total de cartas na mochila: %d\n", id, cartas_mochila);
        sleep(2);
        pthread_mutex_lock(&mochila);

        if(posicao != 'a') {
            printf("Pombo esta realizando viagem para ponto B! Aguardando pombo retornar.\n");
            pthread_cond_wait(&c_usuario, &mochila);
        }else if(cartas_mochila == CARTAS) {
            printf("Mochila do pombo cheia! Aguardando esvaziar. \n");
            pthread_cond_signal(&c_pombo);
            pthread_cond_wait(&c_usuario, &mochila);
        }else {

            printf("Usuario %d colocou uma carta na mochila do pombo!\n", id);
            sleep(0.5);
            cartas_mochila++;
        }

        pthread_mutex_unlock(&mochila);

    }
}
