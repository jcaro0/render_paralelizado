#include "material.hpp"
#include "vector.hpp"
#include <gtest/gtest.h>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

TEST(test_material_constructors, matte_valid) {
  render::matte const m(0.1, 0.2, 0.3);
  EXPECT_DOUBLE_EQ(m.get_reflectancia().get_x(), 0.1);
}

TEST(test_material_constructors, matte_invalid) {
  EXPECT_THROW(render::matte(1.1, 0.2, 0.3), std::invalid_argument);
  EXPECT_THROW(render::matte(-0.1, 0.2, 0.3), std::invalid_argument);
}

TEST(test_material_constructors, metal_valid) {
  render::metal const m(0.1, 0.2, 0.3, 0.5);
  EXPECT_DOUBLE_EQ(m.get_reflectancia().get_x(), 0.1);
  EXPECT_DOUBLE_EQ(m.get_factor_difusion(), 0.5);
}

TEST(test_material_constructors, metal_invalid) {
  EXPECT_THROW(render::metal(1.1, 0.2, 0.3, 0.5), std::invalid_argument);
  EXPECT_THROW(render::metal(0.1, 0.2, 0.3, -0.1), std::invalid_argument);
}

TEST(test_material_constructors, refractive_valid) {
  render::refractive const r(1.5);
  EXPECT_DOUBLE_EQ(r.get_indice_refraccion(), 1.5);
}

TEST(test_material_constructors, refractive_invalid) {
  EXPECT_THROW(render::refractive(0.0), std::invalid_argument);
  EXPECT_THROW(render::refractive(-1.5), std::invalid_argument);
}

TEST(test_material_parsers, parse_matte) {
  std::vector<std::string> const params = {"mat1", "0.1", "0.2", "0.3"};
  std::map<std::string, std::unique_ptr<render::Material>> materiales;
  render::parse_matte(params, materiales);

  EXPECT_EQ(materiales.size(), 1);
  EXPECT_NE(materiales.find("mat1"), materiales.end());
}

TEST(test_material_parsers, parse_metal) {
  std::vector<std::string> const params = {"met1", "0.1", "0.2", "0.3", "0.5"};
  std::map<std::string, std::unique_ptr<render::Material>> materiales;
  render::parse_metal(params, materiales);

  EXPECT_EQ(materiales.size(), 1);
  EXPECT_NE(materiales.find("met1"), materiales.end());
}

TEST(test_material_parsers, parse_refractive) {
  std::vector<std::string> const params = {"ref1", "1.5"};
  std::map<std::string, std::unique_ptr<render::Material>> materiales;
  render::parse_refractive(params, materiales);

  EXPECT_EQ(materiales.size(), 1);
  EXPECT_NE(materiales.find("ref1"), materiales.end());
}
