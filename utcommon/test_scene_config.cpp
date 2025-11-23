#include <cstdio>
#include <fstream>
#include <gtest/gtest.h>
#include <string>

#include "scene_config.hpp"

TEST(test_scene_config, load_valid_scene) {
  std::string const temp_filename = "temp_scene_test.txt";

  // Crear el archivo temporal
  {
    std::ofstream out(temp_filename);
    out << "matte: mat1 0.1 0.2 0.3\n";
    out << "sphere: 0 0 0 1.0 mat1\n";
    out.close();
  }

  // Ejecutar la prueba
  render::SceneData scene;
  int const result = load_scene_config(temp_filename, scene);

  EXPECT_EQ(result, 0);
  EXPECT_EQ(scene.materiales.size(), 1);
  EXPECT_EQ(scene.esferas.size(), 1);
  EXPECT_EQ(scene.cilindros.size(), 0);
  EXPECT_EQ(scene.esferas[0].get_material(), "mat1");

  // 3. Borrar el archivo temporal
  (void) std::remove(temp_filename.c_str());
}

TEST(test_scene_config, error_file_not_found) {
  std::string const temp_filename = "un_archivo_que_no_existe.txt";
  render::SceneData scene;

  int const result = load_scene_config(temp_filename, scene);

  EXPECT_NE(result, 0);
  // No hay archivo que borrar
}

TEST(test_scene_config, load_complex_scene) {
  std::string const temp_filename = "temp_scene_test.txt";

  {
    std::ofstream out(temp_filename);
    out << "matte: mat1 0.1 0.2 0.3\n";
    out << "\n";
    out << "metal: met1 0.8 0.8 0.8 0.2\n";
    out << "sphere: 0 0 0 1.0 mat1\n";
    out << "   \n";
    out << "cylinder: 10 0 0 2.0 0 1 0 met1\n";
    out << "sphere: 5 0 0 1.0 mat1\n";
    out.close();
  }

  render::SceneData scene;
  int const result = load_scene_config(temp_filename, scene);

  EXPECT_EQ(result, 0);
  EXPECT_EQ(scene.materiales.size(), 2);
  EXPECT_EQ(scene.esferas.size(), 2);
  EXPECT_EQ(scene.cilindros.size(), 1);
  EXPECT_EQ(scene.cilindros[0].get_material(), "met1");

  // 3. Borrar el archivo temporal
  (void) std::remove(temp_filename.c_str());
}
