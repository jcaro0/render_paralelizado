#include "camara.hpp"
#include "config.hpp"
#include "figuras.hpp"
#include "gsl/span"
#include "imagen_soa.hpp"
#include "material.hpp"
#include "rayo.hpp"
#include "render_config.hpp"
#include "scene_config.hpp"
#include "vector.hpp"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <optional>
#include <string>

namespace {

  /**
   * @brief Buscamos la intersección más cercana del rayo con la escena.
   */
  std::optional<render::HitInfo> find_closest_hit(render::rayo const & r,
                                                  render::SceneData const & scene) {
    std::optional<render::HitInfo> best_hit;
    double lambda_max_actual = std::numeric_limits<double>::infinity();

    // Iterar sobre Esferas
    for (auto const & esfera : scene.esferas) {
      std::optional<render::HitInfo> temp_hit =
          esfera.interseccion(r, render::constants::lambda_min, lambda_max_actual);

      if (temp_hit.has_value()) {
        best_hit          = temp_hit;
        lambda_max_actual = temp_hit->lambda;
      }
    }

    // Iterar sobre Cilindros
    for (auto const & cilindro : scene.cilindros) {
      std::optional<render::HitInfo> temp_hit =
          cilindro.interseccion(r, render::constants::lambda_min, lambda_max_actual);
      if (temp_hit.has_value()) {
        best_hit          = temp_hit;
        lambda_max_actual = temp_hit->lambda;
      }
    }

    return best_hit;
  }

  /**
   * @brief Despacha al handler de dispersión correcto usando polimorfismo.
   *
   */
  render::ScatterResult scatter(render::rayo const & r_in, render::HitInfo const & hit,
                                render::SceneData const & scene) {
    std::string const & mat_nombre = hit.material_nombre;

    // Buscar el material por nombre en el mapa
    auto const & it = scene.materiales.find(mat_nombre);

    if (it != scene.materiales.end()) {
      // Llamar al método virtual scatter() del material encontrado.
      // it->second es el std::unique_ptr<Material>
      return it->second->scatter(r_in, hit);
    }

    // No se encontró material
    return render::ScatterResult{};  // Devuelve un resultado "sin dispersión"
  }

  /**
   * @brief Calcula el color de fondo (cielo).
   */
  render::vector background_color(render::rayo const & r, render::RenderConfig const & config) {
    // (Usa get_direccion_normalized() como en la versión AOS corregida)
    render::vector const dir_unitaria = r.get_direccion_normalized();
    double const m                    = 0.5 * (dir_unitaria.get_y() + 1.0);
    return (1.0 - m) * config.background_light_color + m * config.background_dark_color;
  }

  /**
   * @brief Calcula el color de un rayo en la escena de forma recursiva.
   */
  render::vector color_rayo(render::rayo const & r, render::SceneData const & scene,
                            render::RenderConfig const & config, int depth) {
    if (depth <= 0) {
      return render::vector{0.0, 0.0, 0.0};
    }

    std::optional<render::HitInfo> best_hit = find_closest_hit(r, scene);

    if (best_hit.has_value()) {
      render::ScatterResult const scatter_result = scatter(r, *best_hit, scene);

      if (scatter_result.se_dispersa) {
        return scatter_result.atenuacion *
               color_rayo(scatter_result.rayo_dispersado, scene, config, depth - 1);
      }
      return render::vector{0, 0, 0};
    }

    return background_color(r, config);
  }

  /**
   * @brief Estructura para agrupar los nombres de fichero de la app.
   */
  struct AppFileNames {
    std::string config_file;
    std::string scene_file;
    std::string out_file;
  };

  /**
   * @brief Extrae y valida los argumentos de la línea de comandos.
   */
  bool parse_args(gsl::span<char const * const> args, AppFileNames & files) {
    if (args.size() != 4) {
      std::string program_name = "program";
      if (!args.empty()) {
        program_name = args[0];
      }
      std::cerr << "Error: Usage: " << program_name
                << " <render-config-file> <scene-config-file> <output-file>\n";
      return false;
    }
    files.config_file = args[1];
    files.scene_file  = args[2];
    files.out_file    = args[3];
    return true;
  }

  /**
   * @brief Estructura para agrupar las dimensiones de la imagen.
   */
  struct ImageDimensions {
    int width;
    int height;
  };

  /**
   * @brief Ejecuta el bucle principal de renderizado (VERSIÓN SOA).
   */
  render::ImagenSOA render_image(render::RenderConfig const & config,
                                 render::SceneData const & scene, render::Camara & cam,
                                 ImageDimensions const & dims) {
    double const MUESTRAS_POR_PIXEL_INV = 1.0 / static_cast<double>(config.samples_per_pixel);
    double const GAMMA                  = config.gamma;
    int const MAX_DEPTH                 = config.max_depth;

    render::ImagenSOA img(dims.width, dims.height);

    for (int j = 0; j < dims.height; ++j) {
      // Vamos fila por fila
      std::cerr << "\rScanlines remaining: " << (dims.height - 1 - j) << ' ' << std::flush;
      for (int i = 0; i < dims.width; ++i) {
        render::vector color_pixel(0.0, 0.0, 0.0);
        for (int s = 0; s < config.samples_per_pixel; ++s) {
          render::rayo const r = cam.get_rayo(j, i);
          color_pixel          = color_pixel + color_rayo(r, scene, config, MAX_DEPTH);
        }
        color_pixel = color_pixel * MUESTRAS_POR_PIXEL_INV;

        color_pixel = render::vector(std::pow(color_pixel.get_x(), 1.0 / GAMMA),
                                     std::pow(color_pixel.get_y(), 1.0 / GAMMA),
                                     std::pow(color_pixel.get_z(), 1.0 / GAMMA));

        render::RawPixel const pixel{static_cast<int>(255.999 * color_pixel.get_x()),
                                     static_cast<int>(255.999 * color_pixel.get_y()),
                                     static_cast<int>(255.999 * color_pixel.get_z())};

        img.store_pixel(i, j, pixel);
      }
    }
    std::cerr << "\nDone.\n";
    return img;
  }

  /**
   * @brief Guarda la imagen renderizada en un fichero PPM (VERSIÓN SOA).
   */
  bool save_image(render::ImagenSOA const & img, std::string const & out_file) {
    if (!img.write_ppm(out_file)) {
      std::cerr << "Error: Failed to write output file: " << out_file << "\n";
      return false;
    }
    std::cout << "Image saved to " << out_file << "\n";
    return true;
  }

}  // namespace

int main(int argc, char * argv[]) {
  // Parsear argumentos
  auto args = gsl::span<char const * const>(argv, static_cast<size_t>(argc));
  AppFileNames files;
  if (!parse_args(args, files)) {
    return 1;
  }
  // Cargar configuración de renderizado
  render::RenderConfig config_data;
  if (!load_render_config(files.config_file, config_data)) {
    std::cerr << "Error: Failed to load render config file: " << files.config_file << "\n";
    return 1;
  }
  // Cargar escena
  render::SceneData scene_data;
  if (load_scene_config(files.scene_file, scene_data) != 0) {
    std::cerr << "Error: Failed to load scene config file: " << files.scene_file << "\n";
    return 1;
  }
  // Configuración inicial
  render::seed_material_rng(static_cast<std::uint64_t>(config_data.material_rng_seed));
  int const WIDTH  = config_data.image_width;
  int const HEIGHT = static_cast<int>(static_cast<double>(WIDTH) /
                                      (static_cast<double>(config_data.aspect_ratio_width) /
                                       static_cast<double>(config_data.aspect_ratio_height)));

  ImageDimensions const dims{WIDTH, HEIGHT};

  render::Camara cam(config_data, dims.width, dims.height);

  render::ImagenSOA const img = render_image(config_data, scene_data, cam, dims);

  if (!save_image(img, files.out_file)) {
    return 1;
  }

  return 0;
}
