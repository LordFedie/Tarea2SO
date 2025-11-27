#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    int count;              // Nº de hebras que han llegado a la barrera
    int N;                  // Nº total de hebras que deben sincronizarse
    int etapa;              // Identificador de la etapa actual
    pthread_mutex_t mutex;  // Mutex para proteger el estado interno
    pthread_cond_t cond;    // Variable de condición para esperar/broadcast
} barrier_t;

void barrier_init(barrier_t *b, int N){
    b->count = 0;           // Ninguna hebra ha llegado aún
    b->N = N;               // Total de hebras a sincronizar
    b->etapa = 0;           // Primera etapa
    pthread_mutex_init(&b->mutex, NULL);
    pthread_cond_init(&b->cond, NULL);
}

void barrier_destroy(barrier_t *b){
    pthread_mutex_destroy(&b->mutex);
    pthread_cond_destroy(&b->cond);
}

void barrier_wait(barrier_t *b){
    pthread_mutex_lock(&b->mutex);

    int etapa_local = b->etapa;  // Esto sirve para detectar cuándo la barrera avanza de etapa

    b->count++;  // Esta hebra llegó al punto de sincronización

    if(b->count < b->N){
        // Si NO soy la última hebra en llegar debo esperar hasta que la etapa cambie.
        while(etapa_local == b->etapa){
            // Espera condicional libera el mutex y suspende la hebra
            pthread_cond_wait(&b->cond, &b->mutex);
        }
    } else {
        // Si SOY la última hebra:
        // Reiniciamos contador
        // Avanzamos de etapa
        // Despertamos a las demás hebras
        b->count = 0;
        b->etapa++;
        pthread_cond_broadcast(&b->cond);
    }

    pthread_mutex_unlock(&b->mutex);
}

#define THREADS 8
#define ETAPAS  2

barrier_t barrera;

void *worker(void *arg){
    int id = *((int*)arg);

    for(int etapa = 0; etapa < ETAPAS; etapa++){
        printf("Hilo %d -> alcanzó etapa %d\n", id, etapa);

        barrier_wait(&barrera);

        printf("Hilo %d -> pasó la barrera en etapa %d\n", id, etapa);

        usleep(100000); 
    }

    return NULL;
}

int main(){
    pthread_t threads[THREADS];
    int ids[THREADS];

    barrier_init(&barrera, THREADS);

    for(int i = 0; i < THREADS; i++){
        ids[i] = i;
        pthread_create(&threads[i], NULL, worker, &ids[i]);
    }

    for(int i = 0; i < THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    barrier_destroy(&barrera);

    printf("Todas las etapas completadas correctamente.\n");
    return 0;
}
