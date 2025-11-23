#!/bin/bash
set -Eeuo pipefail
export LD_LIBRARY_PATH="/opt/gcc-14/lib64${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
set -e
BUILD_DIR="./out/build/default"
CONFIG="Release"

echo "=== Navegando al directorio de build ==="
cd $BUILD_DIR

# CTest necesita saber qué configuración (Release/Debug) usar
# -C $CONFIG       -> Especifica la configuración (ej: Release)
# --output-on-failure -> Muestra la salida de la prueba SÓLO si falla
echo "=== Ejecutando todas las pruebas con CTest ==="
ctest -C $CONFIG --output-on-failure

# Si quieres ejecutar SÓLO las pruebas de utaos:
# ctest -C $CONFIG -R utaos --output-on-failure

echo "=== Todas las pruebas ejecutadas correctamente ==="