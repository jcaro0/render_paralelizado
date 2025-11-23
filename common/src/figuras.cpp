#include "figuras.hpp"
#include "config.hpp"
#include "rayo.hpp"
#include "vector.hpp"
#include <array>
#include <cmath>
#include <iostream>
#include <limits>
#include <numbers>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace render {

  // Helper: devuelve la raíz más cercana válida de ax^2 + bx + c = 0 en el intervalo
  // [lambda_min, lambda_max]. Si no hay raíz válida devuelve std::nullopt.
  namespace {

    std::optional<double> nearest_root_in_range(std::array<double, 3> const & abc,
                                                double lambda_min, double lambda_max) {
      double const a    = abc[0];
      double const b    = abc[1];
      double const c    = abc[2];
      double const disc = b * b - 4 * a * c;
      if (disc < 0) {
        return std::nullopt;
      }
      double const sq = std::sqrt(disc);
      double r1       = (-b - sq) / (2.0 * a);
      double r2       = (-b + sq) / (2.0 * a);

      // ordenar para que r1 sea la más cercana
      if (r1 > r2) {
        std::swap(r1, r2);
      }

      if (r1 >= lambda_min and r1 <= lambda_max) {
        return r1;
      }
      if (r2 >= lambda_min and r2 <= lambda_max) {
        return r2;
      }
      return std::nullopt;
    }

  }  // namespace

  // ===================== ESFERA =====================

  esfera::esfera(double x, double y, double z, double radio, std::string material_nombre)
      : _centro(x, y, z), _radio(radio), _material_nombre(std::move(material_nombre)) { }

  esfera::esfera(vector centro, double radio, std::string material_nombre)
      : _centro(centro), _radio(radio), _material_nombre(std::move(material_nombre)) { }

  double esfera::volumen() const {
    return (4.0 / 3.0) * std::numbers::pi * _radio * _radio * _radio;
  }

  double esfera::superficie() const {
    return 4.0 * std::numbers::pi * _radio * _radio;
  }

  std::optional<HitInfo> esfera::interseccion(rayo const & r, double lambda_min,
                                              double lambda_max) const {
    vector const rc = _centro - r.get_origen();
    double const a  = dot(r.get_direccion(), r.get_direccion());
    double const b  = -2.0 * dot(r.get_direccion(), rc);
    double const c  = dot(rc, rc) - _radio * _radio;

    // Usar helper para obtener la raíz válida más cercana en el rango
    auto root = nearest_root_in_range(std::array<double, 3>{a, b, c}, lambda_min, lambda_max);
    if (!root) {
      return std::nullopt;
    }
    double const lambda = *root;

    // Tenemos una intersección válida
    HitInfo hit_info;
    hit_info.lambda                  = lambda;
    hit_info.punto                   = r.at(lambda);
    vector const normal_hacia_afuera = (hit_info.punto - _centro) / _radio;
    hit_info.set_face_normal(r, normal_hacia_afuera);
    hit_info.material_nombre = _material_nombre;

    return hit_info;
  }

  // ===================== Cilindro =====================

  cilindro::cilindro(double cx, double cy, double cz, double radio, vector eje,
                     std::string material_nombre)
      : _centro(cx, cy, cz), _eje(eje), _radio(radio),
        _material_nombre(std::move(material_nombre)) { }

  cilindro::cilindro(vector p_centro, vector p_eje, double radio, std::string material_nombre)
      : _centro(p_centro), _eje(p_eje), _radio(radio),
        _material_nombre(std::move(material_nombre)) { }

  double cilindro::altura() const {
    return _eje.magnitude();
  }

  double cilindro::volumen() const {
    return std::numbers::pi * _radio * _radio * altura();
  }

  double cilindro::superficie() const {
    return 2.0 * std::numbers::pi * _radio * (_radio + altura());
  }

  std::optional<HitInfo> cilindro::interseccion_cuerpo(rayo const & r, double lambda_min,
                                                       double lambda_max) const {
    // Normalizado del eje y semialtura
    vector const eje_n     = _eje.normalized();
    double const media_alt = _eje.magnitude() / 2.0;
    // Vector desde el origen del rayo hasta el centro del cilindro
    vector const rc = r.get_origen() - _centro;
    vector const dr = r.get_direccion();
    // "Sombras"
    vector const rc_a = rc - (dot(rc, eje_n) * eje_n);
    vector const dr_a = dr - (dot(dr, eje_n) * eje_n);

    double const a = dot(dr_a, dr_a);
    if (std::fabs(a) < constants::EPS) {
      return std::nullopt;  // rayo es paralelo al eje
    }
    double const b = 2.0 * (dot(rc_a, dr_a));
    double const c = dot(rc_a, rc_a) - (_radio * _radio);
    // obtener la raíz más cercana válida en el rango
    auto root = nearest_root_in_range(std::array<double, 3>{a, b, c}, lambda_min, lambda_max);
    if (!root) {
      return std::nullopt;
    }
    double const lambda = *root;
    // comprobar altura (componente paralela al eje)
    double const altura_hit = dot(rc + (lambda * dr), eje_n);
    if (altura_hit < -media_alt or altura_hit > media_alt) {
      return std::nullopt;
    }
    // Creamos la estructura
    HitInfo hit_info;
    hit_info.lambda = lambda;
    hit_info.punto  = r.at(lambda);
    vector const normal =
        (hit_info.punto - _centro) - (dot(hit_info.punto - _centro, eje_n) * eje_n);
    hit_info.set_face_normal(r, normal);
    hit_info.material_nombre = _material_nombre;
    return hit_info;
  }

  std::optional<HitInfo> cilindro::interseccion_tapas(rayo const & r, double lambda_min,
                                                      double lambda_max) const {
    vector const eje_n     = _eje.normalized();
    double const alt       = _eje.magnitude();
    double const media_alt = alt / 2.0;

    double lambda_tapa = std::numeric_limits<double>::infinity();
    vector normal_tapa;

    // Las tapas son planos perpendiculares al eje
    double const den_tapa = dot(r.get_direccion(), eje_n);

    // Comprobar ambas tapas usando un bucle
    for (int i = 0; i < 2; ++i) {
      bool const es_tapa_superior = i == 1;
      vector const normal         = es_tapa_superior ? eje_n : -eje_n;
      vector const centro_tapa    = _centro + normal * media_alt;

      double const t = dot(centro_tapa - r.get_origen(), eje_n) / den_tapa;

      if (t >= lambda_min and t <= lambda_max and t < lambda_tapa) {
        vector const p_hit = r.at(t);
        if (dot(p_hit - centro_tapa, p_hit - centro_tapa) < _radio * _radio) {
          lambda_tapa = t;
          normal_tapa = normal;
        }
      }
    }

    if (lambda_tapa == std::numeric_limits<double>::infinity()) {
      return std::nullopt;
    }

    // Hemos encontrado una intersección válida con una tapa
    HitInfo hit_info;
    hit_info.lambda = lambda_tapa;
    hit_info.punto  = r.at(lambda_tapa);
    hit_info.set_face_normal(r, normal_tapa);
    hit_info.material_nombre = _material_nombre;

    return hit_info;
  }

  std::optional<HitInfo> cilindro::interseccion(rayo const & r, double lambda_min,
                                                double lambda_max) const {
    // Calcular intersección con el cuerpo
    std::optional<HitInfo> hit_cuerpo = interseccion_cuerpo(r, lambda_min, lambda_max);
    // Calcular intersección con las tapas
    std::optional<HitInfo> hit_tapas = interseccion_tapas(r, lambda_min, lambda_max);
    // Determinar la intersección más cercana
    if (hit_tapas.has_value() and !hit_cuerpo.has_value()) {
      return hit_tapas;
    }
    // Ambos tienen hit con el rayo (ver si la tapa esta mas cerca)
    if (hit_tapas.has_value() and hit_cuerpo.has_value() and hit_tapas->lambda < hit_cuerpo->lambda)
    {
      return hit_tapas;
    }
    // Si no se devolvió antes, devolvemos la intersección del cuerpo (puede ser null)
    return hit_cuerpo;
  }

  // ===================== PARSERS =====================

  void parse_sphere(std::vector<std::string> const & parametros, std::vector<esfera> & esferas) {
    if (parametros.size() < 5) {
      std::cerr << "Error: Sphere necesita 5 parámetros (x y z radio material_nombre)\n";
      return;
    }
    double const x                      = std::stod(parametros[0]);
    double const y                      = std::stod(parametros[1]);
    double const z                      = std::stod(parametros[2]);
    double const radio                  = std::stod(parametros[3]);
    std::string const & material_nombre = parametros[4];
    esfera const nueva_esfera(x, y, z, radio, material_nombre);
    esferas.push_back(nueva_esfera);
    std::cout << "Esfera creada: centro(" << x << ", " << y << ", " << z << ") radio: " << radio
              << "\n";
    std::cout << "  Volumen: " << esferas.back().volumen() << "\n";
    std::cout << "  Superficie: " << esferas.back().superficie() << "\n";
    std::cout << " Material: " << esferas.back().get_material() << "\n";
  }

  void parse_cylinder(std::vector<std::string> const & parametros,
                      std::vector<cilindro> & cilindros) {
    if (parametros.size() < 8) {
      std::cerr << "Error: Cylinder necesita 8 parámetros (cx cy cz "
                << "radio ex ey ez material_nombre)\n";
      return;
    }
    double const cx                     = std::stod(parametros[0]);
    double const cy                     = std::stod(parametros[1]);
    double const cz                     = std::stod(parametros[2]);
    double const radio                  = std::stod(parametros[3]);
    double const ex                     = std::stod(parametros[4]);
    double const ey                     = std::stod(parametros[5]);
    double const ez                     = std::stod(parametros[6]);
    std::string const & material_nombre = parametros[7];
    vector const eje(ex, ey, ez);
    cilindro const nuevo_cilindro(cx, cy, cz, radio, eje, material_nombre);
    cilindros.push_back(nuevo_cilindro);
    std::cout << "Cilindro creado: centro(" << cx << ", " << cy << ", " << cz
              << ") Radio: " << radio << "\n";
    std::cout << "  Eje: (" << ex << ", " << ey << ", " << ez << ")\n";
    std::cout << "  Altura: " << cilindros.back().altura() << "\n";
    std::cout << "  Volumen: " << cilindros.back().volumen() << "\n";
    std::cout << "  Superficie: " << cilindros.back().superficie() << "\n";
    std::cout << " Material: " << cilindros.back().get_material() << "\n";
  }

}  // namespace render
