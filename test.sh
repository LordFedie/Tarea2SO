#!/bin/bash

# Script de pruebas para verificar la barrera reutilizable

echo "======================================================"
echo "  Script de Pruebas - Barrera Reutilizable"
echo "======================================================"
echo ""

# Compilar si es necesario
if [ ! -f "./verificacion" ]; then
    echo "Compilando el proyecto..."
    make
    echo ""
fi

# Función para ejecutar una prueba
run_test() {
    local n=$1
    local e=$2
    echo "------------------------------------------------------"
    echo "Prueba: $n hebras, $e etapas"
    echo "------------------------------------------------------"
    ./main $n $e
    echo ""
    sleep 1
}

# Casos de prueba
echo "Ejecutando casos de prueba..."
echo ""

# Caso 1: Configuración mínima
run_test 2 1

# Caso 2: Pocos hilos, pocas etapas
run_test 3 2

# Caso 3: Configuración por defecto
run_test 5 4

# Caso 4: Más hebras
run_test 8 3

# Caso 5: Muchas etapas
run_test 4 6

# Caso 6: Configuración grande
run_test 10 5

echo "======================================================"
echo "  Todas las pruebas completadas"
echo "======================================================"
echo ""
echo "Verificación manual:"
echo "Para cada prueba, confirme que para cada etapa e:"
echo "  - Todas las hebras imprimen 'esperando en etapa e'"
echo "  - LUEGO todas imprimen 'pasó barrera en etapa e'"
echo "  - Ninguna hebra pasa antes de que todas lleguen"
echo "======================================================"