#include "config.hpp"
#include <cstdint>
#include <random>

namespace {

  // En lugar de la variable, creamos una función que la contiene
  std::mt19937_64 & get_material_rng() {
    // Esta variable 'g_material_rng' ahora es estática LOCAL.
    // Solo se inicializa una vez, la primera vez que se llama a esta función.
    static std::mt19937_64 g_material_rng;
    return g_material_rng;
  }

}  // namespace

namespace render {

  void seed_material_rng(std::uint64_t seed) {
    // Usamos la función de acceso para obtener y modificar el RNG
    get_material_rng().seed(static_cast<std::mt19937_64::result_type>(seed));
  }

  double random_double_01() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);

    // Usamos la función de acceso para obtener el RNG
    return distribution(get_material_rng());
  }

  double random_double_in_range(double min, double max) {
    // Esta función no necesita cambios, ya que depende de random_double_01()
    return min + (max - min) * random_double_01();
  }

}  // namespace render
