# Tarea2SO
Tarea 2 de sistemas operativos

#  PARTE I:
Act 1: barrera.c
Act 2: barrera_monitor.c barrera_monitor.h Makefile test.sh (script de pruebas)

# Sobre la Compilación
Escribir make en la terminal
./verificacion prueba con los valores por defecto de 5 hebras y 4 etapas
o 
./verificacion 8 3 para probar con 8 hebras y 3 etapas

# Sobre el Makefile
make : ejecuta y crea archivos.o
make clean : para limpiar los archivos creados durante la ejecucion
Se pueden encontrar más comandos para probarlo dentro del mismo Makefile

# Sobre el Script de Pruebas automáticas test.sh
Compilar con: chmod +x test.sh
Ejecutar con: ./test.sh

# PARTE II
Act 1: sim.c

# Sobre la Compilación
Escribir en la terminal "gcc sim.c -o sim.exe", luego ejecutar con "./sim.exe Nmarcos tamañomarco [--verbose] traza.txt", donde "Nmarcos" corresponde al número de marcos disponibles, "tamañomarco" el tamaño del marco en bytes, --verbose es un argumento opcional para mostrar paso a paso el proceso de traducción y traza.txt el archivo con las direcciones.
