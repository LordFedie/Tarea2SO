// Uso:
//   ./sim Nmarcos tamaño_marco [--verbose] traza.txt
//
// - Nmarcos: número de marcos físicos (ej. 8,16,32)
// - tamaño_marco: tamaño del marco (en bytes), debe ser potencia de 2
// - --verbose: opcional, muestra la traducción paso a paso
// - traza.txt: archivo con una dirección por línea (decimal o 0xHEX)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

// marco fisico
typedef struct {
    int valid;              // 0 = libre, 1 = ocupado
    unsigned int npv;      // num pag virtual
    int use_bit;            // bit de uso para algoritmo reloj
} Frame;

unsigned int parsear_dir(char *s) {
    char *endptr = NULL;
    unsigned long val = strtoul(s, &endptr, 0);
    if (endptr == s) return 0;
    return (unsigned int) val;
}

int es_potencia_2(unsigned int x) {
    return x != 0 && ( (x & (x - 1)) == 0 );
}

int log2_int(unsigned int x) {
    int r = 0;
    while (x > 1) {
        x = x >> 1;
        r++;
    }
    return r;
}

int buscar_marco_con_npv(Frame *frames, int nframes, unsigned int npv) {
    for (int i = 0; i < nframes; i++) {
        if (frames[i].valid && frames[i].npv == npv)
            return i;
    }
    return -1;
}

int buscar_marco_libre(Frame *frames, int nframes) {
    for (int i = 0; i < nframes; i++) {
        if (!frames[i].valid) return i;
    }
    return -1;
}

int seleccion_victima_reloj(Frame *frames, int nframes, int *clock_ptr) {
    int start = *clock_ptr % nframes;
    int i = start;

    // Primera pasada
    while (1) {
        if (!frames[i].valid || frames[i].use_bit == 0) {
            *clock_ptr = (i + 1) % nframes;
            return i;
        }
        frames[i].use_bit = 0;
        i = (i + 1) % nframes;
        if (i == start) break;
    }

    // Segunda pasada
    i = start;
    while (1) {
        if (!frames[i].valid || frames[i].use_bit == 0) {
            *clock_ptr = (i + 1) % nframes;
            return i;
        }
        i = (i + 1) % nframes;
        if (i == start) break;
    }

    return start;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Uso: %s Nmarcos tamaño_marco [--verbose] traza.txt\n", argv[0]);
        return 1;
    }

    // parseo de args
    int verbose = 0;
    char *argumentos[3];
    int arg_counter = 0;

    for (int i = 1; i < argc && arg_counter < 3; i++) {
        if (strcmp(argv[i], "--verbose") == 0) {
            verbose = 1;
            continue;
        }
        argumentos[arg_counter] = argv[i];
        arg_counter++;
    }

    if (arg_counter < 3) {
        printf("Argumentos insuficientes.\n");
        return 1;
    }

    // Nmarcos
    int Nmarcos = atoi(argumentos[0]);
    if (Nmarcos <= 0) {
        printf("Nmarcos invalido\n");
        return 1;
    }

    // Tamaño del marco
    unsigned int page_size = (unsigned int) strtoul(argumentos[1], NULL, 10);
    if (page_size == 0) {
        printf("tamaño_marco invalido\n");
        return 1;
    }
    if (!es_potencia_2(page_size)) {
        printf("tamaño_marco debe ser potencia de 2. Recibido: %u\n", page_size);
        return 1;
    }

    char *trace_filename = argumentos[2];

    // b = log2(page_size) y MASK
    int b = log2_int(page_size);
    unsigned int mask = page_size - 1;

    // Abrir archivo
    FILE *f = fopen(trace_filename, "r");
    if (!f) {
        printf("Error abriendo archivo");
        return 1;
    }

    // Iniciar marcos
    Frame *frames = malloc(Nmarcos * sizeof(Frame));
    for (int i = 0; i < Nmarcos; i++) {
        frames[i].valid = 0;
        frames[i].npv = 0;
        frames[i].use_bit = 0;
    }

    int clock_ptr = 0;

    unsigned int total_refs = 0;
    unsigned int total_faults = 0;

    unsigned int DV = 0;
    unsigned int offset = 0;
    unsigned int npv = 0;

    char *line = NULL;
    size_t len = 0;
    size_t read;

    while ((read = getline(&line, &len, f)) != -1) {

        // recortar espacios
        char *s = line;
        while (isspace((unsigned char)*s)) s++;
        char *end = s + strlen(s) - 1;
        while (end >= s && isspace((unsigned char)*end)) { *end = '\0'; end--; }
        if (*s == '\0') continue;

        // parsear dirección
        DV = parsear_dir(s);
        offset = DV & mask;
        npv = DV >> b;

        total_refs++;

        // frame index, buscar HIT
        int frame_idx = buscar_marco_con_npv(frames, Nmarcos, npv);
        int hit = (frame_idx != -1);

        unsigned int DF = 0;

        if (hit) {
            frames[frame_idx].use_bit = 1;
            DF = ((unsigned int)frame_idx << b) | offset;

            if (verbose) {
                printf("DV=%s  (0x%X) npv=%u offset=%u  HIT  frame=%d  DF=0x%X\n", s, DV, npv, offset, frame_idx, DF);
            }
        }
        else {
            // FALLO
            total_faults++;

            // free index, marco libre
            int free_idx = buscar_marco_libre(frames, Nmarcos);

            if (free_idx != -1) {
                // asignar a libre
                frames[free_idx].valid = 1;
                frames[free_idx].npv = npv;
                frames[free_idx].use_bit = 1;
                frame_idx = free_idx;

                DF = ((unsigned int)frame_idx << b) | offset;

                if (verbose) {
                    printf("DV=%s (0x%X) npv=%u offset=%u  FALLO asigna->frame=%d DF=0x%X\n", s, DV, npv, offset, frame_idx, DF);
                }
            }
            else {
                // buscar la pagina a sacrificar segun algoritmo del reloj
                int victim = seleccion_victima_reloj(frames, Nmarcos, &clock_ptr);

                if (verbose) {
                    printf(
                        "DV=%s (0x%X) npv=%u offset=%u  FALLO reemplaza->victima=%d (npv_saliente=%u)\n",
                        s, DV, npv, offset, victim, frames[victim].npv
                    );
                }

                frames[victim].npv = npv;
                frames[victim].valid = 1;
                frames[victim].use_bit = 1;

                frame_idx = victim;
                DF = ((unsigned int)frame_idx << b) | offset;

                if (verbose) {
                    printf("nuevo frame=%d DF=0x%X\n", frame_idx, DF);
                }
            }
        }
    }

    // resumen final
    printf("\nTotales:\n");
    printf("Referencias: %u\n", total_refs);
    printf("Fallos de pag: %u\n", total_faults);
    float tasa = (total_refs == 0) ? 0.0f : (float)total_faults / (float)total_refs;
    printf("Tasa de fallos: %.6f\n", tasa);

    free(frames);
    fclose(f);
    free(line);
    return 0;
}