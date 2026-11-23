#!/bin/bash
set -Eeuo pipefail
cmake --fresh --preset default
cmake --build --preset gcc-release --config Release --target all --parallel
