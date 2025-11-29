#include "barrera_monitor.h"
#include <stdlib.h>

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

    int etapa_local = b->etapa;  // Capturar etapa actual

    b->count++;  // Esta hebra llegó al punto de sincronización

    if(b->count < b->N){
        // Si NO soy la última hebra en llegar debo esperar hasta que la etapa cambie
        while(etapa_local == b->etapa){
            // Espera condicional: libera el mutex y suspende la hebra
            pthread_cond_wait(&b->cond, &b->mutex);
        }
    } else {
        // Si SOY la última hebra:
        // 1. Reiniciamos contador
        // 2. Avanzamos de etapa
        // 3. Despertamos a las demás hebras
        b->count = 0;
        b->etapa++;
        pthread_cond_broadcast(&b->cond);
    }

    pthread_mutex_unlock(&b->mutex);
}