#ifndef RENDER_SCENE_CONFIG_HPP
#define RENDER_SCENE_CONFIG_HPP

#include "figuras.hpp"
#include "material.hpp"
#include <map>
#include <memory>
#include <string>
#include <vector>

// Estructura pública para almacenar los datos parseados de la escena
namespace render {

  struct SceneData {
    // Los tres vectores se reemplazan por un único mapa
    // que almacena punteros polimórficos a los materiales, indexados por nombre.
    std::map<std::string, std::unique_ptr<Material>> materiales;

    // Los objetos no cambian
    std::vector<render::esfera> esferas;
    std::vector<render::cilindro> cilindros;
  };

}  // namespace render

// Carga el fichero de escena en la estructura SceneData. Devuelve 0 en caso de éxito,
// o un código de error distinto de 0 en caso de fallo.
int load_scene_config(std::string const & ruta, render::SceneData & scene);

#endif  // RENDER_SCENE_CONFIG_HPP
