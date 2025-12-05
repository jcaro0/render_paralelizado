#include "config.hpp"
#include <tbb> // Necesario para TBB
#include <random>
#include <atomic>

namespace {

  // Definimos el tipo de nuestro generador
  using RandomGenerator = std::mt19937_64;

  // Función lambda para inicializar el generador de cada hilo
  // Implementa la lógica de la sección 2.2.4 y 2.2.7 para semillas únicas
  RandomGenerator create_tls_generator() {
      // Variable atómica estática para contar hilos/generadores creados
      static std::atomic<unsigned> counter{0}; 
      
      // Obtenemos un ID único para esta instancia
      unsigned id = counter++; 
      
      // Generamos una semilla inicial base (puedes parametrizarla si quieres determinismo estricto)
      // Aquí combinamos una semilla base con el ID para evitar correlación
      std::uint64_t initial_seed = 12345 + id * 999999; 
      
      return RandomGenerator(initial_seed);
  }

  // El objeto enumerable_thread_specific de TBB gestiona una copia por hilo
  tbb::enumerable_thread_specific<RandomGenerator>& get_tls_rng() {
      static tbb::enumerable_thread_specific<RandomGenerator> tls_rng(create_tls_generator);
      return tls_rng;
  }

}  // namespace

namespace render {

  void seed_material_rng(std::uint64_t seed) {
      // Nota: En un entorno paralelo, "resetear" la semilla es complejo.
      // Esta función podría reiniciar solo el generador del hilo actual 
      // o iterar sobre todos (local() solo accede al actual).
      // Para este proyecto, la inicialización en la lambda suele ser suficiente.
      get_tls_rng().local().seed(seed);
  }

  double random_double_01() {
      static std::uniform_real_distribution<double> distribution(0.0, 1.0);
      
      // .local() devuelve la referencia al generador exclusivo de este hilo
      return distribution(get_tls_rng().local());
  }

  double random_double_in_range(double min, double max) {
      return min + (max - min) * random_double_01();
  }

}  // namespace render