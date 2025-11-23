#include "render_config.hpp"
#include "vector.hpp"
#include <cstdio>
#include <fstream>
#include <gtest/gtest.h>
#include <string>

// probar si se cargan los valores default
TEST(test_render_config, load_default_values) {
  std::string const temp_filename = "temp_config_test.txt";
  {
    std::ofstream out(temp_filename);
    out << "";
    out.close();
  }

  render::RenderConfig config;
  bool const succes = load_render_config(temp_filename, config);

  EXPECT_TRUE(succes);
  EXPECT_EQ(config.image_width, 1'920);
  EXPECT_DOUBLE_EQ(config.gamma, 2.2);
  EXPECT_EQ(config.camera_position.get_z(), -10.0);

  (void) std::remove(temp_filename.c_str());
}

TEST(test_render_config, load_ignores_empty_lines) {
  std::string const temp_filename = "temp_config_test.txt";
  {
    std::ofstream out(temp_filename);
    out << "\n   \nimage_width: 800\n\n";
    out.close();
  }

  render::RenderConfig config;
  bool const succes = load_render_config(temp_filename, config);

  EXPECT_TRUE(succes);
  EXPECT_EQ(config.image_width, 800);

  (void) std::remove(temp_filename.c_str());
}

// probar que falla con dato inexistente
TEST(test_render_config, error_unknown_key) {
  std::string const temp_filename = "temp_config_test.txt";
  {
    std::ofstream out(temp_filename);
    out << "image_widddth: 800";
    out.close();
  }

  render::RenderConfig config;
  bool const succes = load_render_config(temp_filename, config);

  EXPECT_FALSE(succes);

  (void) std::remove(temp_filename.c_str());
}

// probar que falla al introducir string (se busca int)
TEST(test_render_config, error_invalid_value_string) {
  std::string const temp_filename = "temp_config_test.txt";
  {
    std::ofstream out(temp_filename);
    out << "image_width: abc";
    out.close();
  }

  render::RenderConfig config;
  bool const succes = load_render_config(temp_filename, config);

  EXPECT_FALSE(succes);

  (void) std::remove(temp_filename.c_str());
}

// probar fallo si valor fuera de rango (< 0)
TEST(test_render_config, error_invalid_value_range) {
  std::string const temp_filename = "temp_config_test.txt";
  {
    std::ofstream out(temp_filename);
    out << "image_width: -100";
    out.close();
  }

  render::RenderConfig config;
  bool const succes = load_render_config(temp_filename, config);

  EXPECT_FALSE(succes);

  (void) std::remove(temp_filename.c_str());
}
