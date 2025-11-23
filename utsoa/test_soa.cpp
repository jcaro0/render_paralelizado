#include "image_diff.hpp"
#include <cstdio>
#include <cstdlib>
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

// Estructura para los parámetros del test
struct RenderTestParams {
  std::string config_file;
  std::string scene_file;
  std::string ejemplo_file;
  std::string test_name;
};

// Test Fixture Parametrizado
class SOARenderTest : public ::testing::TestWithParam<RenderTestParams> {  // < SOARenderTest
protected:
  std::string temp_out_file;

  void SetUp() override {
    auto const & params = GetParam();
    temp_out_file       = "temp_test_output_for_" + params.ejemplo_file;
  }

  void TearDown() override { [[maybe_unused]] int const ret = std::remove(temp_out_file.c_str()); }
};

// El Test Parametrizado
TEST_P(SOARenderTest, CompararImagenes) {  // < SOARenderTest
  auto const & params = GetParam();
  // LA ÚNICA DIFERENCIA IMPORTANTE ESTÁ AQUÍ:
  std::string const executable_path = SOA_EXECUTABLE_PATH;  // < SOA_EXECUTABLE_PATH

  // Construimos las rutas absolutas a los ficheros de datos
  std::string const test_data_path   = TEST_DATA_DIR;
  std::string const config_file_path = test_data_path + "/" + params.config_file;
  std::string const scene_file_path  = test_data_path + "/" + params.scene_file;

  // Construir el comando
  std::cout << "Ejecutando comando: " << executable_path << " " << config_file_path << " "
            << scene_file_path << " " << temp_out_file << "\n";

  // Ejecutar el renderizador
  std::vector<std::string> args_str;
  args_str.push_back(executable_path);
  args_str.push_back(config_file_path);
  args_str.push_back(scene_file_path);
  args_str.push_back(temp_out_file);

  // Pasamos los argumntos a char *
  std::vector<char *> argv(args_str.size() + 1);
  for (std::size_t i = 0; i < args_str.size(); ++i) {
    argv[i] = args_str[i].data();
  }
  argv.back() = nullptr;

  pid_t const pid = fork();
  int result      = -1;
  int status      = 0;

  if (pid == -1) {
    std::cerr << "fork() falló!" << "\n";
  } else if (pid == 0) {
    execv(executable_path.c_str(), argv.data());
    std::perror("execv falló");
    _exit(127);
  } else {
    if (waitpid(pid, &status, 0) == -1) {
      std::perror("waitpid falló");
    } else {
      // NOLINTNEXTLINE(misc-include-cleaner)
      if (WIFEXITED(status)) {
        // NOLINTNEXTLINE(misc-include-cleaner)
        result = WEXITSTATUS(status);
      }
    }
  }

  ASSERT_EQ(result, 0) << "El proceso de renderizado falló para " << params.scene_file;

  // 3. Cargar la imagen de referencia
  std::vector<render::Rgb8> ejemplo_pixels;
  std::size_t ejemplo_width = 0, ejemplo_height = 0;
  std::string const ejemplo_file_path = test_data_path + "/" + params.ejemplo_file;
  ASSERT_TRUE(render::load_ppm(ejemplo_file_path, ejemplo_pixels, ejemplo_width, ejemplo_height))
      << "Error al cargar el fichero PPM de referencia: " << ejemplo_file_path;

  // 4. Cargar nuestra imagen generada (temporal)
  std::vector<render::Rgb8> generated_pixels;
  std::size_t gen_width = 0, gen_height = 0;
  ASSERT_TRUE(render::load_ppm(temp_out_file, generated_pixels, gen_width, gen_height))
      << "Error al cargar el fichero PPM temporal generado: " << temp_out_file;

  // 5. Comparar dimensiones
  ASSERT_EQ(gen_width, ejemplo_width) << "El ancho de las imágenes no coincide";
  ASSERT_EQ(gen_height, ejemplo_height) << "La altura de las imágenes no coincide";

  // 6. Calcular la diferencia
  render::ImageDifference diff;
  ASSERT_TRUE(render::compute_difference(generated_pixels, ejemplo_pixels, diff))
      << "Error al calcular la diferencia entre imágenes";

  // 7. Comprobar si la diferencia está dentro de los umbrales
  EXPECT_TRUE(render::is_within_thresholds(diff))
      << "La diferencia de la imagen excede los umbrales para " << params.scene_file << ":\n"
      << "  Max Diff: " << diff.max_difference
      << " (Max Aceptable: " << render::MAX_ACCEPTABLE_PIXEL_DIFFERENCE << ")\n"
      << "  RMSE: " << diff.rmse << " (Max Aceptable: " << render::MAX_ACCEPTABLE_RMSE << ")";
}

// Instanciación de los 8 tests
INSTANTIATE_TEST_SUITE_P(
    RenderScenes, SOARenderTest,  // < SOARenderTest
    ::testing::Values(RenderTestParams{"config1.txt", "scene1.txt", "s1.ppm", "Scene1"},
                      RenderTestParams{"config2.txt", "scene2.txt", "s2.ppm", "Scene2"},
                      RenderTestParams{"config3.txt", "scene3.txt", "s3.ppm", "Scene3"},
                      RenderTestParams{"config4.txt", "scene4.txt", "s4.ppm", "Scene4"},
                      RenderTestParams{"config1lite.txt", "scene1.txt", "s1lite.ppm", "Scene1lite"},
                      RenderTestParams{"config2lite.txt", "scene2.txt", "s2lite.ppm", "Scene2lite"},
                      RenderTestParams{"config3lite.txt", "scene3.txt", "s3lite.ppm",
                                       "Scene3lite"}),
    // Lambda para dar nombres legibles a los tests (ej: RenderScenes/Scene1)
    [](::testing::TestParamInfo<RenderTestParams> const & info) { return info.param.test_name; });
