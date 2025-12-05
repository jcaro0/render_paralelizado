#include <gtest/gtest.h>
#include <numbers>

#include "camara.hpp"
#include "rayo.hpp"
#include "render_config.hpp"
#include "vector.hpp"

TEST(test_camara, rayo_origin_is_camera_position) {
  render::RenderConfig config;
  config.camera_position = render::vector(1.0, 2.0, 3.0);
  config.ray_rng_seed = 123;

  int const img_w = 100, img_h = 100;

  render::Camara cam(config, img_w, img_h);

  render::rayo const r = cam.get_rayo(10, 20);

  EXPECT_DOUBLE_EQ(r.get_origen().get_x(), 1.0);
  EXPECT_DOUBLE_EQ(r.get_origen().get_y(), 2.0);
  EXPECT_DOUBLE_EQ(r.get_origen().get_z(), 3.0);
}

TEST(test_camara, rayo_is_different_with_different_seed) {
  render::RenderConfig config1;
  config1.ray_rng_seed = 123;

  render::RenderConfig config2;
  config2.ray_rng_seed = 456;

  int const img_w = 100, img_h = 100;

  render::Camara cam1(config1, img_w, img_h);
  render::Camara cam2(config2, img_w, img_h);

  render::rayo const r1 = cam1.get_rayo(10, 20);
  render::rayo const r2 = cam2.get_rayo(10, 20);

  EXPECT_NE(r1.get_direccion().get_x(), r2.get_direccion().get_x());
}

TEST(test_camara, rayo_is_different_on_consecutive_calls) {
  render::RenderConfig config;
  config.ray_rng_seed = 123;

  int const img_w = 100, img_h = 100;

  render::Camara cam(config, img_w, img_h);

  render::rayo const r1 = cam.get_rayo(10, 20);
  render::rayo const r2 = cam.get_rayo(10, 20);

  EXPECT_NE(r1.get_direccion().get_x(), r2.get_direccion().get_x());
}

TEST(test_camara, degrees_to_radians_zero) {
  double const degrees = 0.0;
  EXPECT_DOUBLE_EQ(render::degrees_to_radians(degrees), 0.0);
}

TEST(test_camara, degrees_to_radians_90) {
  double const degrees = 90.0;
  EXPECT_DOUBLE_EQ(render::degrees_to_radians(degrees), std::numbers::pi / 2.0);
}

TEST(test_camara, degrees_to_radians_360) {
  double const degrees = 360.0;
  EXPECT_DOUBLE_EQ(render::degrees_to_radians(degrees), std::numbers::pi * 2.0);
}

TEST(test_camara, degrees_to_radians_neg_90) {
  double const degrees = -90.0;
  EXPECT_DOUBLE_EQ(render::degrees_to_radians(degrees),
                   -std::numbers::pi / 2.0);
}

TEST(test_camara, degrees_to_radians_neg_360) {
  double const degrees = -360.0;
  EXPECT_DOUBLE_EQ(render::degrees_to_radians(degrees),
                   -std::numbers::pi * 2.0);
}
