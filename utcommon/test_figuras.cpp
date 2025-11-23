#include "config.hpp"
#include "figuras.hpp"
#include "rayo.hpp"
#include "vector.hpp"
#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <vector>

// HitInfo
TEST(test_hit_info, set_face_normal_outside) {  // normal apunta hacia fuera
  render::HitInfo hit;
  render::rayo const r(render::vector(0, 0, 5), render::vector(0, 0, -1));
  render::vector const normal_absoluta(0, 0, 1);

  hit.set_face_normal(r, normal_absoluta);

  EXPECT_TRUE(hit.hacia_fuera);
  EXPECT_DOUBLE_EQ(hit.normal.get_z(), 1.0);
}

// esfera
TEST(test_esfera, constructors) {
  render::esfera const e1(1.0, 2.0, 3.0, 4.0, "mat1");
  EXPECT_DOUBLE_EQ(e1.get_centro().get_x(), 1.0);  // guarda bien el centro
  EXPECT_DOUBLE_EQ(e1.get_radio(), 4.0);           // guarda bien el radio
  EXPECT_EQ(e1.get_material(), "mat1");            // guarda bien el material

  render::vector const centro(1.0, 2.0, 3.0);
  render::esfera const e2(centro, 4.0, "mat1");
  EXPECT_DOUBLE_EQ(e2.get_centro().get_x(), 1.0);
  EXPECT_DOUBLE_EQ(e2.get_radio(), 4.0);
  EXPECT_EQ(e2.get_material(), "mat1");
}

TEST(test_esfera, interseccion_hit) {
  render::esfera const e(0, 0, 0, 2.0, "mat1");
  render::rayo const r(render::vector(0, 0, 5), render::vector(0, 0, -1));
  auto hit = e.interseccion(r, render::constants::lambda_min, 100.0);

  if (hit.has_value()) {
    EXPECT_DOUBLE_EQ(hit.value().lambda, 3.0);
    EXPECT_DOUBLE_EQ(hit.value().punto.get_z(), 2.0);
    EXPECT_DOUBLE_EQ(hit.value().normal.get_z(), 1.0);
    EXPECT_TRUE(hit.value().hacia_fuera);
  } else {
    FAIL() << "Se esperaba una intersección (hit) pero no ocurrió";
  }
}

TEST(test_esfera, interseccion_miss) {
  render::esfera const e(0, 0, 0, 2.0, "mat1");
  render::rayo const r(render::vector(0, 3, 5), render::vector(0, 0, -1));
  auto hit = e.interseccion(r, render::constants::lambda_min, 100.0);

  EXPECT_FALSE(hit.has_value());
}

// cilindro
TEST(test_cilindro, constructors) {
  render::vector const eje(0, 5, 0);
  render::cilindro const c1(1, 2, 3, 4.0, eje, "mat1");
  EXPECT_DOUBLE_EQ(c1.get_centro().get_x(), 1.0);
  EXPECT_DOUBLE_EQ(c1.get_radio(), 4.0);
  EXPECT_DOUBLE_EQ(c1.get_eje().get_y(), 5.0);
  EXPECT_EQ(c1.get_material(), "mat1");
}

TEST(test_cilindro, interseccion_hit_top_cap) {
  render::cilindro const c(0, 0, 0, 2.0, render::vector(0, 10, 0), "mat1");
  render::rayo const r(render::vector(0, 10, 0), render::vector(0, -1, 0));
  auto hit = c.interseccion(r, render::constants::lambda_min, 100.0);

  if (hit.has_value()) {
    EXPECT_DOUBLE_EQ(hit.value().lambda, 5.0);
    EXPECT_DOUBLE_EQ(hit.value().punto.get_y(), 5.0);
    EXPECT_DOUBLE_EQ(hit.value().normal.get_y(), 1.0);
  } else {
    FAIL() << "Se esperaba una intersección (hit) pero no ocurrió";
  }
}

TEST(test_cilindro, interseccion_miss) {
  render::cilindro const c(0, 0, 0, 2.0, render::vector(0, 10, 0), "mat1");
  render::rayo const r(render::vector(0, 0, 5), render::vector(1, 0, 0));
  auto hit = c.interseccion(r, render::constants::lambda_min, 100.0);

  EXPECT_FALSE(hit.has_value());
}

// parse

TEST(test_parsers, parse_sphere) {
  std::vector<std::string> const params = {"1", "2", "3", "4.0", "mat_test"};
  std::vector<render::esfera> esferas;
  render::parse_sphere(params, esferas);

  EXPECT_EQ(esferas.size(), 1);
  EXPECT_DOUBLE_EQ(esferas[0].get_centro().get_x(), 1.0);
  EXPECT_DOUBLE_EQ(esferas[0].get_radio(), 4.0);
  EXPECT_EQ(esferas[0].get_material(), "mat_test");
}

TEST(test_parsers, parse_cylinder) {
  std::vector<std::string> const params = {"1", "2", "3", "4.0", "0", "5", "0", "mat_test"};
  std::vector<render::cilindro> cilindros;
  render::parse_cylinder(params, cilindros);

  EXPECT_EQ(cilindros.size(), 1);
  EXPECT_DOUBLE_EQ(cilindros[0].get_centro().get_x(), 1.0);
  EXPECT_DOUBLE_EQ(cilindros[0].get_radio(), 4.0);
  EXPECT_DOUBLE_EQ(cilindros[0].get_eje().get_y(), 5.0);
  EXPECT_EQ(cilindros[0].get_material(), "mat_test");
}
