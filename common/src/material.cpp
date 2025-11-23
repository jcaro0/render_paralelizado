#include "material.hpp"
#include "config.hpp"
#include "figuras.hpp"
#include "rayo.hpp"
#include "vector.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

  /**
   * @brief Calcula el vector de reflexión (Sección 3.5.2)
   */
  render::vector reflect(render::vector const & v, render::vector const & n) {
    // Implementa: d_r1 = d_o - 2(d_o · d_n)d_n
    return v - 2 * dot(v, n) * n;
  }

  /**
   * @brief Calcula el vector de refracción (Sección 3.5.3)
   */
  render::vector refract(render::vector const & uv, render::vector const & n, double etai_over_etat,
                         double cos_theta) {
    // Implementa:
    // u = rho'(d_o + (cos θ)d_n)
    render::vector const r_out_perp = etai_over_etat * (uv + cos_theta * n);
    // v = -sqrt(|1 - ||u||^2|)d_n
    render::vector const r_out_para =
        -std::sqrt(std::abs(1.0 - r_out_perp.magnitude_squared())) * n;
    // d_r = u + v
    return r_out_perp + r_out_para;
  }

}  // namespace

namespace render {

  // Contructor con vector
  matte::matte(vector const & reflectancia) : _reflectancia{reflectancia} {
    double const x = reflectancia.get_x();
    double const y = reflectancia.get_y();
    double const z = reflectancia.get_z();
    if (x < 0.0 or x > 1.0 or y < 0.0 or y > 1.0 or z < 0.0 or z > 1.0) {
      throw std::invalid_argument("Los valores de reflectancia deben estar entre 0 y 1");
    }
  }

  // Contructor con doubles
  matte::matte(double cx, double cy, double cz) : _reflectancia{cx, cy, cz} {
    if (cx < 0.0 or cx > 1.0 or cy < 0.0 or cy > 1.0 or cz < 0.0 or cz > 1.0) {
      throw std::invalid_argument("Los valores de reflectancia deben estar entre 0 y 1");
    }
  }

  // Contructor con vector
  metal::metal(vector const & reflectancia, double factor_difusion)
      : _reflectancia{reflectancia}, _factor_difusion{factor_difusion} {
    double const x = reflectancia.get_x();
    double const y = reflectancia.get_y();
    double const z = reflectancia.get_z();
    if (x < 0.0 or x > 1.0 or y < 0.0 or y > 1.0 or z < 0.0 or z > 1.0) {
      throw std::invalid_argument("Los valores de reflectancia deben estar entre 0 y 1");
    }
    if (factor_difusion < 0.0) {
      throw std::invalid_argument("El factor de difusión debe ser mayor o igual a 0");
    }
  }

  // Contructor con doubles
  metal::metal(double cx, double cy, double cz, double factor_difusion)
      : _reflectancia{cx, cy, cz}, _factor_difusion{factor_difusion} {
    if (cx < 0.0 or cx > 1.0 or cy < 0.0 or cy > 1.0 or cz < 0.0 or cz > 1.0) {
      throw std::invalid_argument("Los valores de reflectancia deben estar entre 0 y 1");
    }
    if (factor_difusion < 0.0) {
      throw std::invalid_argument("El factor de difusión debe ser mayor o igual a 0");
    }
  }

  refractive::refractive(double indice_refraccion) : _indice_refraccion{indice_refraccion} {
    if (indice_refraccion <= 0.0) {
      throw std::invalid_argument("El índice de refracción debe ser mayor que 0");
    }
  }

  // El material matte no necesita el rayo (toda la informacion la saca del hit)
  // pero necesitamos que tenga la misma estructura que los demas
  [[nodiscard]] ScatterResult matte::scatter([[maybe_unused]] rayo const & r_in,
                                             HitInfo const & hit) const {
    ScatterResult result;
    render::vector dir_reflejada =
        hit.normal + render::vector(render::random_double_in_range(-1.0, 1.0),
                                    render::random_double_in_range(-1.0, 1.0),
                                    render::random_double_in_range(-1.0, 1.0));
    if (dir_reflejada.near_zero()) {
      dir_reflejada = hit.normal;
    }
    result.rayo_dispersado = render::rayo(hit.punto, dir_reflejada);
    result.atenuacion      = _reflectancia;  // Accede al miembro de la clase
    result.se_dispersa     = true;
    return result;
  }

  [[nodiscard]] ScatterResult metal::scatter(rayo const & r_in, HitInfo const & hit) const {
    ScatterResult result;
    render::vector const dir_original_norm = r_in.get_direccion_normalized();
    render::vector const reflejado_inicial = reflect(dir_original_norm, hit.normal);
    double const fuzz                      = _factor_difusion;  // Accede al miembro
    render::vector const diffusion_vec(render::random_double_in_range(-fuzz, fuzz),
                                       render::random_double_in_range(-fuzz, fuzz),
                                       render::random_double_in_range(-fuzz, fuzz));

    result.rayo_dispersado =
        render::rayo(hit.punto, reflejado_inicial.normalized() + diffusion_vec);
    result.atenuacion  = _reflectancia;  // Accede al miembro
    result.se_dispersa = true;
    return result;
  }

  [[nodiscard]] ScatterResult refractive::scatter(rayo const & r_in, HitInfo const & hit) const {
    ScatterResult result;
    result.atenuacion    = render::vector(1.0, 1.0, 1.0);  // Siempre atenúa a blanco
    double const rho     = _indice_refraccion;             // Accede al miembro
    double rho_corregido = 0.0;
    if (hit.hacia_fuera) {
      rho_corregido = 1.0 / rho;
    } else {
      rho_corregido = rho;
    }

    render::vector const dir_original_norm = r_in.get_direccion_normalized();
    double const cos_theta           = std::min(dot(-dir_original_norm, hit.normal), 1.0);
    double const sin_theta           = std::sqrt(1.0 - cos_theta * cos_theta);

    render::vector dir_dispersada;
    if (rho_corregido * sin_theta > 1.0) {
      dir_dispersada = reflect(dir_original_norm, hit.normal);  // Reflexión interna
    } else {
      dir_dispersada =
          refract(dir_original_norm, hit.normal, rho_corregido, cos_theta);  // Refracción
    }

    result.rayo_dispersado = render::rayo(hit.punto, dir_dispersada);
    result.se_dispersa     = true;
    return result;
  }

  // PARSERS

  // añade un std::unique_ptr<Material> al mapa
  void parse_matte(std::vector<std::string> const & parametros,
                   std::map<std::string, std::unique_ptr<Material>> & materiales) {
    if (parametros.size() < 4) {
      std::cerr << "Error: Material matte necesita 4 parámetros (nombre r g b)\n";
      return;
    }
    std::string const & nombre = parametros[0];
    double const r             = std::stod(parametros[1]);
    double const g             = std::stod(parametros[2]);
    double const b             = std::stod(parametros[3]);

    // Crea el material en el heap y lo almacena en el mapa
    materiales[nombre] = std::make_unique<matte>(r, g, b);

    std::cout << "Material matte creado: " << nombre << " (" << r << ", " << g << ", " << b
              << ")\n";
  }

  void parse_metal(std::vector<std::string> const & parametros,
                   std::map<std::string, std::unique_ptr<Material>> & materiales) {
    if (parametros.size() < 5) {
      std::cerr << "Error: Material metal necesita 5 parámetros (nombre r g b factor_difusion)\n";
      return;
    }
    std::string const & nombre   = parametros[0];
    double const r               = std::stod(parametros[1]);
    double const g               = std::stod(parametros[2]);
    double const b               = std::stod(parametros[3]);
    double const factor_difusion = std::stod(parametros[4]);

    materiales[nombre] = std::make_unique<metal>(r, g, b, factor_difusion);

    std::cout << "Material metal creado: " << nombre << " (" << r << ", " << g << ", " << b
              << ") factor: " << factor_difusion << "\n";
  }

  void parse_refractive(std::vector<std::string> const & parametros,
                        std::map<std::string, std::unique_ptr<Material>> & materiales) {
    if (parametros.size() < 2) {
      std::cerr << "Error: Material refractive necesita 2 parámetros (nombre indice)\n";
      return;
    }
    std::string const & nombre = parametros[0];
    double const indice        = std::stod(parametros[1]);

    materiales[nombre] = std::make_unique<refractive>(indice);

    std::cout << "Material refractive creado: " << nombre << " índice: " << indice << "\n";
  }

}  // namespace render
