#ifndef RENDER_COMMON_CONFIG_HPP
#define RENDER_COMMON_CONFIG_HPP

#include <cstdint>

namespace render {

  namespace constants {

    // Tolerancias del proyecto
    inline constexpr double EPS        = 1e-8;
    inline constexpr double lambda_min = 1e-3;

  }  // namespace constants

  // Funciones para el generador de materiales
  double random_double_01();
  double random_double_in_range(double min, double max);
  void seed_material_rng(std::uint64_t seed);

}  // namespace render

#endif  // RENDER_CONFIG_HPP
