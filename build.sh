#!/bin/bash
set -Eeuo pipefail
export LD_LIBRARY_PATH="/opt/gcc-14/lib64${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
cmake --preset default
cmake --build --preset gcc-release --config Release --target all --parallel
cmake --preset clang-tidy
cmake --build --preset clang-tidy-debug --config Debug --target all --parallel

cp out/build/default/aos/Release/render-aos ejecutables_avignon/render-aos && cp out/build/default/soa/Release/render-soa ejecutables_avignon/render-soa