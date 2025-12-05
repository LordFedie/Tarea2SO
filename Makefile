# Makefile para compilar el proyecto de barrera

CC = gcc
CFLAGS = -Wall -Wextra -pthread -O2
TARGET = main
OBJS = main.o barrera_monitor.o

# Regla por defecto
all: $(TARGET)

# Compilar el ejecutable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compilar main.o
main.o: main.c barrera_monitor.h
	$(CC) $(CFLAGS) -c main.c

# Compilar barrier.o
barrera_monitor.o: barrera_monitor.c barrera_monitor.h
	$(CC) $(CFLAGS) -c barrera_monitor.c

# Limpiar archivos generados
clean:
	rm -f $(TARGET) $(OBJS)

# Ejecutar con valores por defecto
run: $(TARGET)
	./$(TARGET)

# Ejecutar con parámetros personalizados
# Ejemplo: make test N=8 E=3
test: $(TARGET)
	./$(TARGET) $(N) $(E)

# Regla para mostrar ayuda
help:
	@echo "Uso del Makefile:"
	@echo "  make         - Compila el proyecto"
	@echo "  make clean   - Elimina archivos compilados"
	@echo "  make run     - Compila y ejecuta con valores por defecto"
	@echo "  make test N=<num_hebras> E=<num_etapas> - Ejecuta con parámetros"
	@echo ""
	@echo "Ejemplos:"
	@echo "  make test N=8 E=3"
	@echo "  make test N=10 E=5"

.PHONY: all clean run test help
