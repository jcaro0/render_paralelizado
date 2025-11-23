#ifndef RENDER_MATERIAL_HPP
#define RENDER_MATERIAL_HPP

#include "figuras.hpp"
#include "rayo.hpp"
#include "vector.hpp"
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace render {

  struct ScatterResult {
    bool se_dispersa = false;
    render::rayo rayo_dispersado;
    render::vector atenuacion;
  };

  // Clase base abstracta para materiales
  class Material {
  public:
    //"Rule of Five" para base polimórfica (no copiable)
    Material()                             = default;
    Material(Material const &)             = delete;
    Material(Material &&)                  = delete;
    Material & operator=(Material const &) = delete;
    Material & operator=(Material &&)      = delete;
    virtual ~Material()                    = default;  // Destructor virtual

    /**
     * @brief Calcula cómo se dispersa un rayo al golpear este material.
     * @param r_in El rayo entrante que golpeó la superficie.
     * @param hit La información de la intersección (punto, normal, etc.).
     * @return Un ScatterResult que describe el rayo dispersado y la atenuación.
     */
    [[nodiscard]] virtual ScatterResult scatter(rayo const & r_in, HitInfo const & hit) const = 0;
  };

  // matte hereda de Material
  class matte : public Material {
  public:
    // constructor con vector - valida que los valores estén entre 0 y 1
    matte(vector const & reflectancia);
    // constructor con componentes individuales - valida que los valores estén entre 0 y 1
    matte(double cx, double cy, double cz);

    // "Rule of Five" (clase no copiable, sigue la base)
    matte(matte const &)             = delete;
    matte(matte &&)                  = delete;
    matte & operator=(matte const &) = delete;
    matte & operator=(matte &&)      = delete;
    ~matte() override                = default;  // Destructor override

    // Getter para la reflectancia
    [[nodiscard]] vector const & get_reflectancia() const { return _reflectancia; }

    // Implementación de la interfaz
    [[nodiscard]] ScatterResult scatter(rayo const & r_in, HitInfo const & hit) const override;

  private:
    vector _reflectancia;
  };

  // metal ahora hereda de Material
  class metal : public Material {
  public:
    // constructor con reflectancia y factor de difusión - valida que los valores estén entre 0 y 1
    metal(vector const & reflectancia, double factor_difusion);
    // constructor con componentes individuales y factor de difusión - valida que los valores estén
    // entre 0 y 1
    metal(double cx, double cy, double cz, double factor_difusion);

    // "Rule of Five" (clase no copiable, sigue la base)
    metal(metal const &)             = delete;
    metal(metal &&)                  = delete;
    metal & operator=(metal const &) = delete;
    metal & operator=(metal &&)      = delete;
    ~metal() override                = default;  // Destructor override

    // Getters
    [[nodiscard]] vector const & get_reflectancia() const { return _reflectancia; }

    [[nodiscard]] double get_factor_difusion() const { return _factor_difusion; }

    // Implementación de la interfaz
    [[nodiscard]] ScatterResult scatter(rayo const & r_in, HitInfo const & hit) const override;

  private:
    vector _reflectancia;
    double _factor_difusion;
  };

  // refractive ahora hereda de Material
  class refractive : public Material {
  public:
    // constructor con índice de refracción - valida que sea > 0
    refractive(double indice_refraccion);

    // "Rule of Five" (clase no copiable, sigue la base)
    refractive(refractive const &)             = delete;
    refractive(refractive &&)                  = delete;
    refractive & operator=(refractive const &) = delete;
    refractive & operator=(refractive &&)      = delete;
    ~refractive() override                     = default;  // Destructor override

    // Getter para el índice de refracción
    [[nodiscard]] double get_indice_refraccion() const { return _indice_refraccion; }

    // Implementación de la interfaz
    [[nodiscard]] ScatterResult scatter(rayo const & r_in, HitInfo const & hit) const override;

  private:
    double _indice_refraccion;
  };

  // Las firmas de los parsers aceptan el mapa de materiales.
  void parse_matte(std::vector<std::string> const & parametros,
                   std::map<std::string, std::unique_ptr<Material>> & materiales);

  void parse_metal(std::vector<std::string> const & parametros,
                   std::map<std::string, std::unique_ptr<Material>> & materiales);

  void parse_refractive(std::vector<std::string> const & parametros,
                        std::map<std::string, std::unique_ptr<Material>> & materiales);

}  // namespace render

#endif
