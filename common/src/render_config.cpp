#include "render_config.hpp"
#include "vector.hpp"
#include <fstream>
#include <cstddef>
#include <string>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

namespace {

  // Eliminar espacios/tabuladores al inicio y final de una cadena.
  void trim(std::string & s) {
    size_t const a = s.find_first_not_of(" \t");
    if (a == std::string::npos) {
      s.clear();
      return;
    }
    size_t const b = s.find_last_not_of(" \t");
    s        = s.substr(a, b - a + 1);
  }

  // Extrae la clave y el valor desde una línea ya recortada a la izquierda.
  bool split_key_value(std::string const & line_left_trimmed, std::string & key,
                       std::string & value) {
    auto pos = line_left_trimmed.find(':');
    if (pos == std::string::npos) {
      std::istringstream iss(line_left_trimmed);
      if (!(iss >> key)) {
        return false;
      }
      return false;
    }
    key   = line_left_trimmed.substr(0, pos);
    value = line_left_trimmed.substr(pos + 1);
    return true;
  }

  // Comprueba si hay datos extra en el stream después de leer el valor esperado.
  // Devuelve true y rellena "extra" si hay contenido extra no vacío.
  bool detect_extra(std::istringstream & iss, std::string & extra) {
    if ((iss >> std::ws) and (std::getline(iss, extra)) and !extra.empty()) {
      size_t const a = extra.find_first_not_of(" \t");
      if (a != std::string::npos) {
        extra = extra.substr(a);
      }
      if (!extra.empty()) {
        return true;
      }
    }
    extra.clear();
    return false;
  }

  // Comprueba si una clave es conocida (lista centralizada).
  bool is_known_key(std::string const & k) {
    static std::vector<std::string> const keys = {
      "aspect_ratio",          "image_width",       "gamma",
      "camera_position",       "camera_target",     "camera_north",
      "field_of_view",         "samples_per_pixel", "max_depth",
      "material_rng_seed",     "ray_rng_seed",      "background_dark_color",
      "background_light_color"};
    // devuelve true si k "esta" en keys
    bool encontrado = false;
    for (std::string const & key : keys) {
      if (key == k) {
        encontrado = true;
      }
    }
    if (!encontrado) {
      std::cerr << "Error: Unknown configuration key: [" << k << ":]" << "\n";
    }
    return encontrado;
  }

  // HELPERS
  bool print_invalid(std::string const & key, std::string const & line) {
    std::cerr << "Error: Invalid value for key: [" << key << ":]" << "\n";
    std::cerr << "Line: \"" << line << "\"" << "\n";
    return false;
  }

  bool print_extra(std::string const & key, std::string const & extra) {
    std::cerr << "Error: Extra data after configuration value for key: [" << key << ":]" << "\n";
    std::cerr << "Extra: \"" << extra << "\"" << "\n";
    return false;
  }

  bool parse_aspect_ratio(std::string const & value, render::RenderConfig & config,
                          std::string const & line) {
    std::istringstream iss(value);
    int w = 0, h = 0;
    if (!(iss >> w >> h)) {
      return print_invalid("aspect_ratio", line);
    }
    // CORRECCIÓN: Comprobar <= 0 según el PDF
    if (w <= 0 or h <= 0) {
      return print_invalid("aspect_ratio", line);
    }
    std::string extra;
    if (detect_extra(iss, extra)) {
      return print_extra("aspect_ratio", extra);
    }
    config.aspect_ratio_width  = w;
    config.aspect_ratio_height = h;
    return true;
  }

  bool parse_image_width(std::string const & value, render::RenderConfig & config,
                         std::string const & line) {
    std::istringstream iss(value);
    int w = 0;
    if (!(iss >> w)) {
      return print_invalid("image_width", line);
    }
    // CORRECCIÓN: Comprobar <= 0 según el PDF
    if (w <= 0) {
      return print_invalid("image_width", line);
    }
    std::string extra;
    if (detect_extra(iss, extra)) {
      return print_extra("image_width", extra);
    }
    config.image_width = w;
    return true;
  }

  bool parse_gamma(std::string const & value, render::RenderConfig & config,
                   std::string const & line) {
    std::istringstream iss(value);
    double g = 0.0;
    if (!(iss >> g)) {
      return print_invalid("gamma", line);
    }
    std::string extra;
    if (detect_extra(iss, extra)) {
      return print_extra("gamma", extra);
    }
    config.gamma = g;
    return true;
  }

  bool parse_field_of_view(std::string const & value, render::RenderConfig & config,
                           std::string const & line) {
    std::istringstream iss(value);
    double fov = 0.0;
    if (!(iss >> fov)) {
      return print_invalid("field_of_view", line);
    }
    if (fov <= 0 or fov >= 180) {
      return print_invalid("field_of_view", line);
    }
    std::string extra;
    if (detect_extra(iss, extra)) {
      return print_extra("field_of_view", extra);
    }
    config.field_of_view = fov;
    return true;
  }

  bool parse_samples_per_pixel(std::string const & value, render::RenderConfig & config,
                               std::string const & line) {
    std::istringstream iss(value);
    int spp = 0;
    if (!(iss >> spp) or spp <= 0) {
      return print_invalid("samples_per_pixel", line);
    }
    std::string extra;
    if (detect_extra(iss, extra)) {
      return print_extra("samples_per_pixel", extra);
    }
    config.samples_per_pixel = spp;
    return true;
  }

  bool parse_max_depth(std::string const & value, render::RenderConfig & config,
                       std::string const & line) {
    std::istringstream iss(value);
    int md = 0;
    if (!(iss >> md) or md <= 0) {
      return print_invalid("max_depth", line);
    }
    std::string extra;
    if (detect_extra(iss, extra)) {
      return print_extra("max_depth", extra);
    }
    config.max_depth = md;
    return true;
  }

  bool parse_material_rng_seed(std::string const & value, render::RenderConfig & config,
                               std::string const & line) {
    std::istringstream iss(value);
    int seed = 0;
    if (!(iss >> seed) or seed <= 0) {
      return print_invalid("material_rng_seed", line);
    }
    std::string extra;
    if (detect_extra(iss, extra)) {
      return print_extra("material_rng_seed", extra);
    }
    config.material_rng_seed = seed;
    return true;
  }

  bool parse_ray_rng_seed(std::string const & value, render::RenderConfig & config,
                          std::string const & line) {
    std::istringstream iss(value);
    int seed = 0;
    if (!(iss >> seed) or seed <= 0) {
      return print_invalid("ray_rng_seed", line);
    }
    std::string extra;
    if (detect_extra(iss, extra)) {
      return print_extra("ray_rng_seed", extra);
    }
    config.ray_rng_seed = seed;
    return true;
  }

  // Parsers para vectores de 3 doubles (cámara y colores)
  bool parse_camera_vector(std::string const & key, std::string const & value,
                           render::RenderConfig & config, std::string const & line) {
    std::istringstream iss(value);
    double x = 0, y = 0, z = 0;
    if (!(iss >> x >> y >> z)) {
      return print_invalid(key, line);
    }
    std::string extra;
    if (detect_extra(iss, extra)) {
      return print_extra(key, extra);
    }
    render::vector const v(x, y, z);
    if (key == "camera_position") {
      config.camera_position = v;
    } else if (key == "camera_target") {
      config.camera_target = v;
    } else {
      config.camera_north = v;
    }
    return true;
  }

  bool parse_background_color(std::string const & key, std::string const & value,
                              render::RenderConfig & config, std::string const & line) {
    std::istringstream iss(value);
    double x = 0, y = 0, z = 0;
    if (!(iss >> x >> y >> z)) {
      return print_invalid(key, line);
    }
    std::string extra;
    if (detect_extra(iss, extra)) {
      return print_extra(key, extra);
    }
    if (x < 0 or x > 1 or y < 0 or y > 1 or z < 0 or z > 1) {
      return print_invalid(key, line);
    }
    render::vector const v(x, y, z);
    if (key == "background_dark_color") {
      config.background_dark_color = v;
    } else {
      config.background_light_color = v;
    }
    return true;
  }

  // Despachador directo: una única función que llama a los parsers concretos.
  // Evitamos typedefs/aliases y tablas tipo-función; es más explícito y
  // cumple la petición de usar solo funciones.
  bool dispatch_parser(std::string const & key, std::string const & value,
                       render::RenderConfig & config, std::string const & line) {
    if (key == "aspect_ratio") {
      return parse_aspect_ratio(value, config, line);
    }
    if (key == "image_width") {
      return parse_image_width(value, config, line);
    }
    if (key == "gamma") {
      return parse_gamma(value, config, line);
    }
    if (key == "field_of_view") {
      return parse_field_of_view(value, config, line);
    }
    if (key == "samples_per_pixel") {
      return parse_samples_per_pixel(value, config, line);
    }
    if (key == "max_depth") {
      return parse_max_depth(value, config, line);
    }
    if (key == "material_rng_seed") {
      return parse_material_rng_seed(value, config, line);
    }
    if (key == "ray_rng_seed") {
      return parse_ray_rng_seed(value, config, line);
    }
    if (key == "camera_position" or key == "camera_target" or key == "camera_north") {
      return parse_camera_vector(key, value, config, line);
    }
    if (key == "background_dark_color" or key == "background_light_color") {
      return parse_background_color(key, value, config, line);
    }

    std::cerr << "Error: Unknown configuration key: [" << key << ":]" << "\n";
    return false;
  }

  // Maneja una línea completa de configuración: recorta, separa clave/valor,
  // valida y llama al dispatcher. Devuelve true para continuar, false para
  // indicar error crítico (imprime el mensaje correspondiente).
  bool handle_config_line(std::string const & raw_line, render::RenderConfig & config) {
    std::string line = raw_line;
    trim(line);
    if (line.empty()) {
      return true;  // línea en blanco -> ignorar
    }
    std::string key, value;
    if (!split_key_value(line, key, value)) {
      // No hay ':' en la línea: el primer token se considera clave desconocida
      if (value.empty()) {
        std::cerr << "Error: Unknown configuration key: [" << key << ":]" << "\n";
        return false;
      }
      // token inválido -> ignorar línea
      return true;
    }
    // nos aseguramos de que sea una sola palabra
    trim(key);
    {
      std::istringstream kss(key);
      std::string token;
      if (!(kss >> token) or (kss >> token)) {
        std::cerr << "Error: Unknown configuration key: [" << key << ":]" << "\n";
        return false;
      }
    }
    if (!is_known_key(key)) {
      return false;
    }
    // Sólo recorto la izquierda del valor (se permiten espacios al final)
    if (!value.empty()) {
      size_t const s = value.find_first_not_of(" \t");
      if (s != std::string::npos) {
        value.erase(0, s);
      } else {
        value.clear();
      }
    }

    return dispatch_parser(key, value, config, raw_line);
  }

  void imprimir(render::RenderConfig const & config) {
    std::cout << "Configuration loaded\n";
    std::cout << "===Configuration===\n";
    std::cout << "Image configuration:\n";
    std::cout << "  Image width: " << config.image_width << "\n";
    std::cout << "  Aspect ratio: " << std::fixed << std::setprecision(17)
              << (static_cast<double>(config.aspect_ratio_width) /
                  static_cast<double>(config.aspect_ratio_height))
              << "\n";
    std::cout.unsetf(std::ios_base::fixed);
    std::cout << "  Gamma: " << config.gamma << "\n";
    std::cout << "Camera configuration: \n";
    std::cout << "  Camera position: " << config.camera_position << "\n";
    std::cout << "  Camera target: " << config.camera_target << "\n";
    std::cout << "  Camera north: " << config.camera_north << "\n";
    std::cout << "  Field of view: " << config.field_of_view << "\n";
    std::cout << "Rendering configuration:\n";
    std::cout << "  Samples per pixel: " << config.samples_per_pixel << "\n";
    std::cout << "  Max depth: " << config.max_depth << "\n";
    std::cout << "Background configuration:\n";
    std::cout << "  Light color: " << config.background_light_color << "\n";
    std::cout << "  Dark color: " << config.background_dark_color << "\n";
    std::cout << "RNG configuration:\n";
    std::cout << "  Material RNG seed: " << config.material_rng_seed << "\n";
    std::cout << "  Ray RNG seed: " << config.ray_rng_seed << "\n";
  }

}  // namespace

bool load_render_config(std::string const & filename, render::RenderConfig & config) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "No se pudo abrir el archivo de configuración: " << filename << "\n";
    return false;
  }

  std::string line;
  while (std::getline(file, line)) {
    // Delego TODO: manejo de la línea en la función centralizada
    if (!handle_config_line(line, config)) {
      return false;
    }
  }
  imprimir(config);

  return true;
}
