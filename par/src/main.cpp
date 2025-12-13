#include "camara.hpp"
#include "config.hpp"
#include "figuras.hpp"
#include "gsl/span"
#include "imagen_par.hpp"
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
#include <chrono>
#include <stdexcept>
#include <string_view>
#include <tbb/blocked_range2d.h>
#include <tbb/global_control.h>
#include <tbb/parallel_for.h>

namespace {

/**
 * @brief Encuentra la intersección más cercana del rayo con la escena.
 *
 */
std::optional<render::HitInfo>
find_closest_hit(render::rayo const &r, render::SceneData const &scene) {
  std::optional<render::HitInfo> best_hit;
  double lambda_max_actual = std::numeric_limits<double>::infinity();

  for (auto const &esfera : scene.esferas) {
    std::optional<render::HitInfo> temp_hit = esfera.interseccion(
        r, render::constants::lambda_min, lambda_max_actual);
    if (temp_hit.has_value()) {
      best_hit = temp_hit;
      lambda_max_actual = temp_hit->lambda;
    }
  }
  for (auto const &cilindro : scene.cilindros) {
    std::optional<render::HitInfo> temp_hit = cilindro.interseccion(
        r, render::constants::lambda_min, lambda_max_actual);
    if (temp_hit.has_value()) {
      best_hit = temp_hit;
      lambda_max_actual = temp_hit->lambda;
    }
  }
  return best_hit;
}

/**
 * @brief Llama al metodo de cada material
 *
 */
render::ScatterResult scatter(render::rayo const &r_in,
                              render::HitInfo const &hit,
                              render::SceneData const &scene) {
  std::string const &mat_nombre = hit.material_nombre;

  // Buscar el material por nombre en el mapa
  auto const &it = scene.materiales.find(mat_nombre);
  // Si encontramos el material
  if (it != scene.materiales.end()) {
    // Llamar al método virtual scatter() del material encontrado.
    // it->second es el std::unique_ptr<Material>
    return it->second->scatter(r_in, hit);
  }
  // Devuelve un resultado "sin dispersión"
  return render::ScatterResult{};
}

/**
 * @brief Calcula el color de fondo (cielo).
 *
 */
render::vector background_color(render::rayo const &r,
                                render::RenderConfig const &config) {
  render::vector const dir_unitaria = r.get_direccion_normalized();
  double const m = 0.5 * (dir_unitaria.get_y() + 1.0);
  return (1.0 - m) * config.background_light_color +
         m * config.background_dark_color;
}

/**
 * @brief Calcula el color de un rayo en la escena de forma recursiva.
 *
 */
render::vector color_rayo(render::rayo const &r, render::SceneData const &scene,
                          render::RenderConfig const &config, int depth) {
  if (depth <= 0) {
    return render::vector{0.0, 0.0, 0.0};
  }
  // buscamos el closest hit
  std::optional<render::HitInfo> best_hit = find_closest_hit(r, scene);

  if (best_hit.has_value()) {
    render::ScatterResult const scatter_result = scatter(r, *best_hit, scene);

    if (scatter_result.se_dispersa) {
      return scatter_result.atenuacion *
             color_rayo(scatter_result.rayo_dispersado, scene, config,
                        depth - 1);
    }
    // No se encontro material de hit
    return render::vector{0, 0, 0};
  }
  // No choco el rayo
  return background_color(r, config);
}

// Estructura para agrupar las opciones de paralelismo
struct ParallelConfig {
    int num_threads = 0;
    std::string partitioner = "auto";
    int grain_size = 0;
};

// Estructura para agrupar los nombres de fichero de entrada/salida
struct FileNames {
  std::string config_file;
  std::string scene_file;
  std::string out_file;
  ParallelConfig par_config;
};

bool parse_args(gsl::span<char const *const> args, FileNames &files) {
    // Deben existir al menos 4 argumentos
    if (args.size() < 4) {
        std::cerr << "Uso: ./render-par <render.cfg> <scene.cfg> <out.ppm> [threads] [partitioner] [grain]\n";
        return false;
    }

    // Argumentos obligatorios
    files.config_file = args[1];
    files.scene_file = args[2];
    files.out_file   = args[3];

    //Número de hilos
    if (args.size() >= 5) {
        try {
            files.par_config.num_threads = std::stoi(args[4]);
        } catch (...) {
            std::cerr << "Error: num_threads debe ser un número entero\n";
            return false;
        }
    }

    //Particionador
    if (args.size() >= 6) {
        files.par_config.partitioner = args[5]; // simple | auto | static
    }

    // Grain size
    if (args.size() >= 7) {
        try {
            files.par_config.grain_size = std::stoi(args[6]);
        } catch (...) {
            std::cerr << "Error: grain_size debe ser un número entero\n";
            return false;
        }
    }

    return true;
}

// Estructura que agrupa la anchura con la altura
struct ImageDimensions {
  int width;
  int height;
};

render::ImagenPAR render_image(render::RenderConfig const &config,
                               render::SceneData const &scene,
                               render::Camara &cam,
                               ImageDimensions const &dims,
                               std::string const &partitioner,
                               int grain_size) {
  double const MUESTRAS_POR_PIXEL_INV =
      1.0 / static_cast<double>(config.samples_per_pixel);
  double const GAMMA = config.gamma;
  int const MAX_DEPTH = config.max_depth;

  render::ImagenPAR img(dims.width, dims.height);

  // Bucle principal (paralelizado)

  // tbb::parallel_for(
      // tbb::blocked_range2d<int>(0, dims.height, grain_size, 0, dims.width, grain_size),

    auto body = [&](const tbb::blocked_range2d<int> &r) {
        // Iteramos sobre el sub-rango asignado a este hilo
        for (int j = r.rows().begin(); j != r.rows().end(); ++j) {
          for (int i = r.cols().begin(); i != r.cols().end(); ++i) {

            render::vector color_pixel(0.0, 0.0, 0.0);

            for (int s = 0; s < config.samples_per_pixel; ++s) {
              // color_rayo usa números aleatorios.
              // Asegúrate de que 'config.cpp' implementa TLS (Thread Local
              // Storage) para el generador de aleatorios, o esto fallará/será
              // incorrecto.
              render::rayo const r_cam = cam.get_rayo(j, i);
              color_pixel =
                  color_pixel + color_rayo(r_cam, scene, config, MAX_DEPTH);
            }

            color_pixel = color_pixel * MUESTRAS_POR_PIXEL_INV;

            color_pixel =
                render::vector(std::pow(color_pixel.get_x(), 1.0 / GAMMA),
                               std::pow(color_pixel.get_y(), 1.0 / GAMMA),
                               std::pow(color_pixel.get_z(), 1.0 / GAMMA));

            int const ir = static_cast<int>(255.999 * color_pixel.get_x());
            int const ig = static_cast<int>(255.999 * color_pixel.get_y());
            int const ib = static_cast<int>(255.999 * color_pixel.get_z());

            // img.store_pixel es seguro aquí porque cada hilo escribe en (i, j)
            // únicos
            img.store_pixel(i, j, render::PixelPAR(ir, ig, ib));
          }
        }
      }; // Fin parallel_for

  std::cerr << "\nDone.\n";
  return img;
}

bool save_image(render::ImagenPAR const &img, std::string const &out_file) {
  if (!img.write_ppm(out_file)) {
    std::cerr << "Error: Failed to write output file: " << out_file << "\n";
    return false;
  }
  std::cout << "Image saved to " << out_file << "\n";
  return true;
}

} // namespace

int main(int argc, char *argv[]) {
  auto args = gsl::span<char const *const>(argv, static_cast<size_t>(argc));
  FileNames files;
  if (!parse_args(args, files)) {
    return 1;
  }
  // Cargamos el fichero config
  render::RenderConfig config_data;
  if (!load_render_config(files.config_file, config_data)) {
    std::cerr << "Error: Failed to load render config file: "
              << files.config_file << "\n";
    return 1;
  }
  // Cargamos el fichero escena
  render::SceneData scene_data;
  if (load_scene_config(files.scene_file, scene_data) != 0) {
    std::cerr << "Error: Failed to load scene config file: " << files.scene_file
              << "\n";
    return 1;
  }
  // Configuramos un par de cosas
  render::seed_material_rng(
      static_cast<std::uint64_t>(config_data.material_rng_seed));
  int const WIDTH = config_data.image_width;
  int const HEIGHT =
      static_cast<int>(static_cast<double>(WIDTH) /
                       (static_cast<double>(config_data.aspect_ratio_width) /
                        static_cast<double>(config_data.aspect_ratio_height)));

  ImageDimensions const dims{WIDTH, HEIGHT};

  render::Camara cam(config_data, dims.width, dims.height);

  std::optional<tbb::global_control> tbb_limit;
  
  if (files.par_config.num_threads > 0) {
    // Si el usuario especificó hilos, aplicamos el límite
    tbb_limit.emplace(tbb::global_control::max_allowed_parallelism, 
                      files.par_config.num_threads);
    std::cerr << "Using " << files.par_config.num_threads << " threads (TBB limited).\n";
  } else {
    // Si es 0 o no se especificó, TBB usará el default (hardware_concurrency)
    std::cerr << "Using default TBB threads (hardware_concurrency).\n";
  }

// Esto es para la medición de tiempo para la selección de configuración
auto end = std::chrono::high_resolution_clock::now();
double seconds =
    std::chrono::duration<double>(end - start).count();
std::cout << "Tiempo de ejecución: " << seconds << " segundos\n";

// El render_image se guarda
render::ImagenPAR const img =
  render_image(config_data, scene_data, cam, dims,
              files.par_config.partitioner, 
              files.par_config.grain_size); 

  if (!save_image(img, files.out_file)) {
    return 1;
  }

  return 0;
}
