#!/bin/bash
set -Eeuo pipefail
export LD_LIBRARY_PATH="/opt/gcc-14/lib64${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

EXECUTABLE_PATH="./ejecutables_avignon/render-par"
RENDER_CFG="./examples/config4.txt"
SCENE_CFG="./examples/scene4.txt"
OUT="out.ppm"

THREADS=(32 64)
PARTITIONERS=("auto" "static" "simple")
GRAINS=(1 4 16 64 256 1024)

echo "threads,partitioner,grain,time"

for t in "${THREADS[@]}"; do
  for p in "${PARTITIONERS[@]}"; do
    for g in "${GRAINS[@]}"; do
      "$EXECUTABLE_PATH" "$RENDER_CFG" "$SCENE_CFG" "$OUT" $t $p $g 2>&1 \
        | grep "Tiempo de ejecución" \
        | awk -v t=$t -v p=$p -v g=$g '{print t "," p "," g "," $NF}'
    done
  done
done
