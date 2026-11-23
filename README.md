# Render paralelo

Proyecto C++20 para renderizar escenas en formato PPM usando Intel oneTBB.

## Requisitos

- CMake 4.0 o superior
- compilador C++20 (GCC o Clang)
- Ninja
- Intel oneTBB instalado y disponible para CMake (`TBBConfig.cmake`)

Durante la configuración, CMake descarga Microsoft GSL y GoogleTest desde
GitHub.

## Compilar

```bash
./build.sh
```

El ejecutable queda en `out/build/default/par/Release/render-par`.

## Ejecutar

```bash
./out/build/default/par/Release/render-par \
  examples/config1.txt examples/scene1.txt out/scene1.ppm
```

Los argumentos opcionales son el número de hilos, el particionador (`simple`,
`auto` o `static`) y el tamaño de grano:

```bash
./out/build/default/par/Release/render-par \
  examples/config1.txt examples/scene1.txt out/scene1.ppm 16 auto 64
```

## Tests

```bash
./utest.sh
```
