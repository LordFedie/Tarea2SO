#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "barrera_monitor.h"

// Valores por defecto
#define DEFAULT_N 5
#define DEFAULT_E 4

// Variables globales
barrier_t barrera;
int num_hebras;
int num_etapas;

// Estructura para pasar argumentos a las hebras
typedef struct {
    int tid;
} thread_args_t;

/**
 * Función ejecutada por cada hebra
 */
void *worker(void *arg){
    thread_args_t *args = (thread_args_t *)arg;
    int tid = args->tid;
    
    // Inicializar semilla aleatoria única para cada hebra
    unsigned int seed = time(NULL) + tid;
    
    for(int etapa = 0; etapa < num_etapas; etapa++){
        // Trabajo simulado con tiempo aleatorio (50-150 ms)
        int tiempo_trabajo = 50000 + (rand_r(&seed) % 100000);
        usleep(tiempo_trabajo);
        
        // Mensaje ANTES de esperar en la barrera
        printf("[Hebra %d] esperando en etapa %d\n", tid, etapa);
        fflush(stdout);
        
        // Punto de sincronización - esperar en la barrera
        barrier_wait(&barrera);
        
        // Mensaje DESPUÉS de pasar la barrera
        printf("[Hebra %d] pasó barrera en etapa %d\n", tid, etapa);
        fflush(stdout);
        
        // Pequeña pausa para visualizar mejor
        usleep(10000);
    }
    
    return NULL;
}

/**
 * Función para imprimir el uso del programa
 */
void print_usage(const char *prog_name){
    printf("Uso: %s [N] [E]\n", prog_name);
    printf("  N: Número de hebras (por defecto: %d)\n", DEFAULT_N);
    printf("  E: Número de etapas (por defecto: %d)\n", DEFAULT_E);
    printf("\nEjemplos:\n");
    printf("  %s           # Usa valores por defecto (5 hebras, 4 etapas)\n", prog_name);
    printf("  %s 8         # 8 hebras, 4 etapas\n", prog_name);
    printf("  %s 10 6      # 10 hebras, 6 etapas\n", prog_name);
}

int main(int argc, char *argv[]){
    // Parsear argumentos de línea de comandos
    num_hebras = DEFAULT_N;
    num_etapas = DEFAULT_E;
    
    if(argc >= 2){
        num_hebras = atoi(argv[1]);
        if(num_hebras <= 0){
            fprintf(stderr, "Error: N debe ser un número positivo\n");
            print_usage(argv[0]);
            return 1;
        }
    }
    
    if(argc >= 3){
        num_etapas = atoi(argv[2]);
        if(num_etapas <= 0){
            fprintf(stderr, "Error: E debe ser un número positivo\n");
            print_usage(argv[0]);
            return 1;
        }
    }
    
    if(argc > 3){
        fprintf(stderr, "Error: Demasiados argumentos\n");
        print_usage(argv[0]);
        return 1;
    }
    
    printf("VERIFICACIÓN DE BARRERA REUTILIZABLE\n");
    printf("Número de hebras: %d\n", num_hebras);
    printf("Número de etapas: %d\n", num_etapas);
    
    // Inicializar la barrera
    barrier_init(&barrera, num_hebras);
    
    // Crear arreglos para hebras y argumentos
    pthread_t *threads = malloc(num_hebras * sizeof(pthread_t));
    thread_args_t *args = malloc(num_hebras * sizeof(thread_args_t));
    
    if(threads == NULL || args == NULL){
        fprintf(stderr, "Error: No se pudo asignar memoria\n");
        return 1;
    }
    
    // Crear las hebras
    printf("Creando %d hebras...\n\n", num_hebras);
    for(int i = 0; i < num_hebras; i++){
        args[i].tid = i;
        if(pthread_create(&threads[i], NULL, worker, &args[i]) != 0){
            fprintf(stderr, "Error al crear hebra %d\n", i);
            return 1;
        }
    }
    
    // Esperar a que todas las hebras terminen
    for(int i = 0; i < num_hebras; i++){
        pthread_join(threads[i], NULL);
    }
    
    // Destruir la barrera
    barrier_destroy(&barrera);
    
    // Liberar memoria
    free(threads);
    free(args);
    
    // Mensaje final
    printf("\n");
    printf("TODAS LAS ETAPAS FUERON COMPLETADAS CORRECTAMEMTE\n");
    printf("Verificación: Si la barrera funciona correctamente, para cada etapa e, todas las hebras deben imprimir\n");
    printf("'esperando en etapa e' ANTES de que cualquiera imprima 'pasó barrera en etapa e'.\n");
    printf("\n");
    
    return 0;
}