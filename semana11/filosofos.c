#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

#define N 5
#define ESQ(id) (id)
#define DIR(id)	(id+1)%N

char estados[N];        // f -> faminto, p -> pensando, c -> comendo
pthread_mutex_t estados_lock = PTHREAD_MUTEX_INITIALIZER;

sem_t s[N];

void *filosofos (void *arg);

void pega_talher (int n);
void devolve_talher (int n);
void comer (int n);

int main () {
	int i;
	int *id;
	//semaforo 
	for (i=0; i<N; i++) {
		sem_init(&s[i], 0, 1);
	}
	pthread_t r[N]; 

	//criacao das threads de filosofos
       for (i = 0; i < N ; i++) {
	     id = (int *) malloc(sizeof(int));
	     *id = i;
	     pthread_create(&r[i], NULL, filosofos, (void*) (id));
	}

    pthread_join(r[0],NULL);
	return 0;
}


void *filosofos (void *arg) {
	int n = *((int *) arg);
	while(1) {
		//pensar
    	pthread_mutex_lock(&estados_lock);
        estados[n] = 'p';
		pthread_mutex_unlock(&estados_lock);
		printf("Filosofo %d pensando ...\n", n);
		sleep(3);
		pega_talher(n);
		
		//comer
		printf("\tFilosofo %d comendo ...\n", n);
		comer(n);
		sleep(3);
        
		printf("\tFilosofo %d acabou de comer ...\n", n);
		devolve_talher(n);
	} 
}


void comer(int n) {

    // verifica, de fato, se os talheres dos vizinhos estao livres
    if(estados[n] == 'f' && estados[(n-1+N)%N] != 'c' && estados[DIR(n)] != 'c') {
    	pthread_mutex_lock(&estados_lock);
        estados[n] = 'c';
		pthread_mutex_unlock(&estados_lock);
    }

    
}

void pega_talher (int n) {
    pthread_mutex_lock(&estados_lock);
		estados[n] = 'f';
	pthread_mutex_unlock(&estados_lock);
    
    sem_wait(&s[ESQ(n)]);
    sem_wait(&s[DIR(n)]);
}

void devolve_talher (int n) {

	sem_post(&s[ESQ(n)]);
	sem_post(&s[DIR(n)]);

    // filosofo verifica se os talheres dos vizinhos estão disponiveis, se sim, faz eles comerem
    comer((n-1+N)%N);
    comer(DIR(n));
    
}
