#!/bin/bash
set -Eeuo pipefail
cmake --preset default
cmake --build --preset gcc-release --config Release --target all --parallel
