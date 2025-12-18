#!/bin/bash
set -Eeu

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

EXECUTABLE_PATH="./ejecutables_avignon/render-par"
RENDER_CFG="./examples/config4.txt"
SCENE_CFG="./examples/scene4.txt"
OUT="out.ppm"

THREADS=(128 256)
PARTITIONERS=("auto" "static" "simple")
GRAINS=(1 4 16 64 256 1024)

echo "threads,partitioner,grain,time"

for t in "${THREADS[@]}"; do
  for p in "${PARTITIONERS[@]}"; do
    for g in "${GRAINS[@]}"; do

      LINE=$("$EXECUTABLE_PATH" "$RENDER_CFG" "$SCENE_CFG" "$OUT" \
             "$t" "$p" "$g" 2>&1 | grep "Tiempo de ejecución")

      if [[ -n "$LINE" ]]; then
        TIME=$(echo "$LINE" | awk '{print $4}')
        echo "$t,$p,$g,$TIME"
      else
        echo "$t,$p,$g,ERROR"
      fi

    done
  done
done
