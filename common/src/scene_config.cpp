#include "scene_config.hpp"
#include "figuras.hpp"
#include "material.hpp"
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace {

  std::vector<std::string> dividir_string(std::string const & texto) {
    std::vector<std::string> partes;
    std::string parte_actual;
    for (char const c : texto) {
      if (c == ' ' or c == '\t') {
        if (!parte_actual.empty()) {
          partes.push_back(parte_actual);
          parte_actual = "";
        }
      } else {
        parte_actual += c;
      }
    }
    if (!parte_actual.empty()) {
      partes.push_back(parte_actual);
    }
    return partes;
  }

  void limpiar(std::string & texto) {
    texto.erase(0, texto.find_first_not_of(" \t"));
    texto.erase(texto.find_last_not_of(" \t") + 1);
  }

  void manejar_linea(std::string const & linea, render::SceneData & scene) {
    if (linea.empty()) {
      return;
    }
    size_t const pos_dos_puntos = linea.find(':');
    if (pos_dos_puntos == std::string::npos) {
      return;
    }
    std::string tipo = linea.substr(0, pos_dos_puntos);
    limpiar(tipo);
    std::string parametros_texto = linea.substr(pos_dos_puntos + 1);
    limpiar(parametros_texto);
    std::vector<std::string> const parametros = dividir_string(parametros_texto);

    // Pasa el mapa 'scene.materiales' a los parsers de material
    if (tipo == "matte") {
      render::parse_matte(parametros, scene.materiales);
    } else if (tipo == "metal") {
      render::parse_metal(parametros, scene.materiales);
    } else if (tipo == "refractive") {
      render::parse_refractive(parametros, scene.materiales);
    } else if (tipo == "sphere") {
      render::parse_sphere(parametros, scene.esferas);
    } else if (tipo == "cylinder") {
      render::parse_cylinder(parametros, scene.cilindros);
    } else {
      std::cerr << "Error: Unknown scene entity: " << tipo << "\n";
      std::cerr << "Line: \"" << linea << "\"\n";
      std::exit(1);
    }
  }

  void imprimir(render::SceneData const & scene) {
    int const num_obj = static_cast<int>(scene.esferas.size() + scene.cilindros.size());
    int const num_mat = static_cast<int>(scene.materiales.size());

    std::cout << "Scene with " << num_obj << " objects and " << num_mat << " materials\n";
    std::cout << "Materials:\n";
    std::cout << std::fixed << std::setprecision(15);

    for (auto const & esfera : scene.esferas) {
      std::cout << "  - sphere: center: " << esfera.get_centro()
                << ", radius: " << esfera.get_radio() << "\n";
    }
    for (auto const & cilindro : scene.cilindros) {
      std::cout << "  - cylinder: center: " << cilindro.get_centro()
                << ", radius: " << cilindro.get_radio() << ", height: " << cilindro.altura()
                << ", axis: " << cilindro.get_eje().normalized() << "\n";
    }
    std::cout.unsetf(std::ios_base::fixed);
  }

}  // namespace

int load_scene_config(std::string const & ruta, render::SceneData & scene) {
  std::cout << "\n===Scene===\n";
  std::ifstream fichero(ruta);
  if (!fichero.is_open()) {
    std::cerr << "Error: No se pudo abrir el fichero " << ruta << "\n";
    return 1;
  }
  std::string linea;
  while (std::getline(fichero, linea)) {
    try {
      manejar_linea(linea, scene);
    } catch (std::exception const & e) {
      std::cerr << "Error procesando línea '" << linea << "': " << e.what() << "\n";
    }
  }
  imprimir(scene);
  return 0;
}
