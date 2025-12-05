#include "camara.hpp"
#include "config.hpp"
#include "rayo.hpp"
#include "render_config.hpp"
#include "vector.hpp"
#include <cmath>

namespace render {

Camara::Camara(render::RenderConfig const &config, int img_w, int img_h)
    : _origen_rayos(config.camera_position) {
  // Vector focal (Usa P y D de config)
  vector const vf = config.camera_position - config.camera_target;

  // Distancia focal
  double const df = vf.magnitude();

  // Altura de la ventana (hp) (Usa fov_grados de config)
  double const alpha_rad = degrees_to_radians(config.field_of_view);
  double const hp = 2.0 * std::tan(alpha_rad / 2.0) * df;

  // Anchura de la ventana (wp)
  double const ratio = static_cast<double>(img_w) / static_cast<double>(img_h);
  double const wp = hp * ratio;

  // Vectores directores (u, v) (Usa 'n' de config)
  vector const vf_unit = vf.normalized();
  vector const u = cross(config.camera_north, vf_unit).normalized();
  vector const v =
      cross(vf_unit, u); // v ya es unitario si u y vf_unit lo son y ortogonales

  // Vectores horizontal (ph) y vertical (pv)
  vector const ph = wp * u;
  // El PDF indica que pv va hacia abajo (coordenadas de imagen)
  vector const pv = hp * (-v); // Corrección de signo según PDF

  // Determinación del origen (O) y los deltas (∆x, ∆y)

  // Vectores de desplazamiento entre píxeles
  _delta_x = ph / static_cast<double>(img_w);
  _delta_y = pv / static_cast<double>(img_h);

  // Origen O (centro del píxel superior izquierdo (0,0)) según PDF
  // O = P - vf - 0.5*(ph + pv) + 0.5*(_delta_x + _delta_y)
  // (Usa 'P' de config)
  _origen_viewport = config.camera_position - vf - 0.5 * (ph + pv) +
                     0.5 * (_delta_x + _delta_y);
}

rayo Camara::get_rayo(int f, int c) {
  // Implementación de "Proceso de trazado de rayos" (Sección 3.2 PDF)

  // Generar δx y δy en el intervalo [-0.5, 0.5] usando el RNG INTERNO
  // _dist_rayos(_rng_rayos) genera en [0.0, 1.0)
  // Restamos 0.5 para obtener el rango [-0.5, 0.5)
  double const delta_x_rand = render::random_double_in_range(-0.5, 0.5);
  double const delta_y_rand = render::random_double_in_range(-0.5, 0.5);

  // Calcular la posición aleatoria Q en el píxel (f, c)
  // Usamos la fórmula del PDF Q = O + (c + x)*deltax + (f + y)*deltay
  vector const direccion =
      _origen_viewport + (static_cast<double>(c) + delta_x_rand) * _delta_x +
      (static_cast<double>(f) + delta_y_rand) * _delta_y - _origen_rayos;

  return rayo(_origen_rayos, direccion);
}

} // namespace render
