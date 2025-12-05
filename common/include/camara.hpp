#ifndef RENDER_CAMARA_HPP
#define RENDER_CAMARA_HPP

#include "rayo.hpp"
#include "render_config.hpp"
#include "vector.hpp"
#include <numbers>
#include <random>

namespace render {

class Camara {
public:
  /**
   * @brief Constructor de la Cámara basado en la estructura de configuración.
   * @param config Estructura con todos los parámetros de renderizado (posición,
   * objetivo, norte, fov, semilla_rng).
   * @param img_w Anchura de la imagen en píxeles
   * @param img_h Altura de la imagen en píxeles
   */
  Camara(render::RenderConfig const &config, int img_w, int img_h);

  /**
   * @brief Genera un rayo para un píxel específico.
   * Implementa el muestreo aleatorio dentro del píxel.
   * @param f Fila del píxel (coordenada y)
   * @param c Columna del píxel (coordenada x)
   * @return Un rayo que parte del origen de la cámara (P) y pasa por
   * un punto aleatorio dentro del área del píxel (f, c).
   */
  [[nodiscard]] rayo get_rayo(int f, int c);

private:
  vector _origen_rayos;    // Posición P de la cámara
  vector _origen_viewport; // Origen O (centro del píxel (0,0) de la ventana)
  vector _delta_x;         // Vector de desplazamiento horizontal por píxel (⃗∆x)
  vector _delta_y;         // Vector de desplazamiento vertical por píxel (⃗∆y)

  // RNG propio de la cámara
  std::mt19937_64
      _rng_rayos; // Motor para generar números aleatorios para rayos
  std::uniform_real_distribution<double>
      _dist_rayos; // Distribución para generar números en [0.0, 1.0)
};

// Función helper (si no la tienes ya en otro sitio)
inline double degrees_to_radians(double degrees) {
  return degrees * std::numbers::pi / 180.0;
}

} // namespace render

#endif // RENDER_CAMARA_HPP
