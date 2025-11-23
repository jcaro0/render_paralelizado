#ifndef FIGURAS_HPP
#define FIGURAS_HPP

#include "rayo.hpp"
#include "vector.hpp"
#include <optional>
#include <string>
#include <vector>

namespace render {

  // Estructura para almacenar la información de una intersección
  struct HitInfo {
    // 'existe' ya no es necesario; lo maneja std::optional
    double lambda = 0.0;       // Distancia (lambda) hasta el punto de impacto
    vector punto;              // Punto (x,y,z) de la intersección
    vector normal;             // Vector normal en el punto de impacto
    bool hacia_fuera = false;  // Indica si el rayo golpea la cara frontal
    std::string material_nombre;

    // Función para establecer la normal correcta
    void set_face_normal(rayo const & r, vector const & normal_absoluta) {
      // Usar get_direccion_normalized()
      // Si el producto escalar es > 0, el rayo viene desde "dentro" del objeto
      hacia_fuera = dot(r.get_direccion_normalized(), normal_absoluta) < 0;
      if (hacia_fuera) {
        normal = normal_absoluta;
      } else {
        normal = -normal_absoluta;
      }
    }
  };

  // Clase Esfera
  class esfera {
  public:
    // Constructor para el parser (según objeto.cpp)
    esfera(double x, double y, double z, double radio, std::string material_nombre);

    // Constructor que usa vectores
    esfera(vector centro, double radio, std::string material_nombre);

    [[nodiscard]] double volumen() const;
    [[nodiscard]] double superficie() const;

    // Función de intersección
    [[nodiscard]] std::optional<HitInfo> interseccion(rayo const & r, double lambda_min,
                                                      double lambda_max) const;

    // Getter para el material (según objeto.cpp)
    [[nodiscard]] std::string const & get_material() const { return _material_nombre; }

    // Getters para logs
    [[nodiscard]] vector const & get_centro() const { return _centro; }

    [[nodiscard]] double get_radio() const { return _radio; }

  private:
    vector _centro;
    double _radio;
    std::string _material_nombre;
  };

  // Clase Cilindro
  class cilindro {
  public:
    // Constructor para el parser (según objeto.cpp)
    cilindro(double cx, double cy, double cz, double radio, vector eje,
             std::string material_nombre);

    // Constructor que usa vectores
    cilindro(vector p_centro, vector p_eje, double radio, std::string material_nombre);

    [[nodiscard]] double altura() const;
    [[nodiscard]] double volumen() const;
    [[nodiscard]] double superficie() const;

    // Función de intersección
    [[nodiscard]] std::optional<HitInfo> interseccion(rayo const & r, double lambda_min,
                                                      double lambda_max) const;

    // Getter para el material (según objeto.cpp)
    [[nodiscard]] std::string const & get_material() const { return _material_nombre; }

    // Getters para logs
    [[nodiscard]] vector const & get_centro() const { return _centro; }

    [[nodiscard]] vector const & get_eje() const { return _eje; }

    [[nodiscard]] double get_radio() const { return _radio; }

  private:
    vector _centro;  // Punto base del eje del cilindro
    vector _eje;     // Vector que va de la base a la tapa superior
    double _radio;
    std::string _material_nombre;

    // Funciones auxiliares privadas para la intersección
    [[nodiscard]] std::optional<HitInfo> interseccion_cuerpo(rayo const & r, double lambda_min,
                                                             double lambda_max) const;
    [[nodiscard]] std::optional<HitInfo> interseccion_tapas(rayo const & r, double lambda_min,
                                                            double lambda_max) const;
  };

  // PARSERS
  // Funciones de parsing para figuras
  void parse_sphere(std::vector<std::string> const & parametros, std::vector<esfera> & esferas);

  void parse_cylinder(std::vector<std::string> const & parametros,
                      std::vector<cilindro> & cilindros);

}  // namespace render

#endif
