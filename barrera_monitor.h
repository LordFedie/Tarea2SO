#ifndef BARRERA_MONITOR_H
#define BARRERA_MONITOR_H

#include <pthread.h>

typedef struct {
    int count;              // Nº de hebras que han llegado a la barrera
    int N;                  // Nº total de hebras que deben sincronizarse
    int etapa;              // Identificador de la etapa actual
    pthread_mutex_t mutex;  // Mutex para proteger el estado interno
    pthread_cond_t cond;    // Variable de condición para esperar/broadcast
} barrier_t;

/**
 * Inicializa una barrera para sincronizar N hebras
 * @param b Puntero a la barrera
 * @param N Número de hebras a sincronizar
 */
void barrier_init(barrier_t *b, int N);

/**
 * Destruye la barrera y libera recursos
 * @param b Puntero a la barrera
 */
void barrier_destroy(barrier_t *b);

/**
 * Espera en la barrera hasta que todas las N hebras lleguen
 * @param b Puntero a la barrera
 */
void barrier_wait(barrier_t *b);

#endif // BARRIER_H