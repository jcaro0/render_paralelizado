#!/bin/bash
EXECUTABLE_PATH="./ejecutables_avignon/render-par"
RENDER_CFG="./examples/config5.txt"
SCENE_CFG="./examples/scene5.txt"
OUT="out.ppm"

THREADS=(1 2 4 8 16 32 64 128 256)
PARTITIONERS=("auto" "static" "simple")
GRAINS=(1 4 16 64 256 1024)

echo "threads,partitioner,grain,time"

for t in "${THREADS[@]}"; do
  for p in "${PARTITIONERS[@]}"; do
    for g in "${GRAINS[@]}"; do

      echo "Running: threads=$t partitioner=$p grain=$g" >&2

      "$EXECUTABLE_PATH" "$RENDER_CFG" "$SCENE_CFG" "$OUT" $t $p $g 2>&1 \
        | grep "Tiempo de ejecución" \
        | awk -v t=$t -v p=$p -v g=$g '{print t "," p "," g "," $NF}'

    done
  done
done
