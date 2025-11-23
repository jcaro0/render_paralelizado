#include <gtest/gtest.h>

#include "vector.hpp"

TEST(test_vector, magnitude_zero) {
  render::vector const vec{0.0, 0.0, 0.0};
  EXPECT_EQ(vec.magnitude(), 0.0);
}

TEST(test_vector, magnitude_positive) {
  render::vector const vec{3.0, 4.0, 0.0};
  EXPECT_EQ(vec.magnitude(), 5.0);
}

TEST(test_vector, magnitude_squared_zero) {
  render::vector const v(0.0, 0.0, 0.0);
  EXPECT_DOUBLE_EQ(v.magnitude_squared(), 0.0);
}

TEST(test_vector, magnitude_squared_positive) {
  render::vector const v(1.0, 2.0, 3.0);
  EXPECT_DOUBLE_EQ(v.magnitude_squared(), 14.0);
}

TEST(test_vector, identity_add_zero) {
  render::vector const v{1.2, -3.4, 5.6};
  render::vector const zero{0.0, 0.0, 0.0};
  auto sum = v + zero;
  EXPECT_DOUBLE_EQ(sum.get_x(), v.get_x());
  EXPECT_DOUBLE_EQ(sum.get_y(), v.get_y());
  EXPECT_DOUBLE_EQ(sum.get_z(), v.get_z());
}

// normalized
TEST(test_vector, normalization_has_unit_length) {
  render::vector const v{2.0, 0.0, 0.0};
  auto n = v.normalized();
  EXPECT_NEAR(n.magnitude(), 1.0, 1e-8);
}

TEST(test_vector, normalization_pythagorean) {
  render::vector const v(3.0, 4.0, 0.0);

  auto n = v.normalized();

  EXPECT_DOUBLE_EQ(n.get_x(), 0.6);
  EXPECT_DOUBLE_EQ(n.get_y(), 0.8);
  EXPECT_DOUBLE_EQ(n.get_z(), 0.0);
  EXPECT_DOUBLE_EQ(n.magnitude(), 1.0);
}

TEST(test_vector, dot_product_basic) {
  render::vector const v1(1.0, 2.0, 3.0);
  render::vector const v2(2.0, 3.0, 4.0);
  EXPECT_DOUBLE_EQ(dot(v1, v2), 20.0);
}

TEST(test_vector, dot_cross_orthogonality) {
  render::vector const a{1.0, 0.0, 0.0};
  render::vector const b{0.0, 1.0, 0.0};
  auto c = cross(a, b);
  EXPECT_NEAR(dot(c, a), 0.0, 1e-8);
}

TEST(test_vector, cross_product_basic) {
  render::vector const v_x(1.0, 0.0, 0.0);
  render::vector const v_y(0.0, 1.0, 0.0);
  auto resultado = cross(v_x, v_y);

  EXPECT_DOUBLE_EQ(resultado.get_x(), 0.0);
  EXPECT_DOUBLE_EQ(resultado.get_y(), 0.0);
  EXPECT_DOUBLE_EQ(resultado.get_z(), 1.0);
}

// near_zero
TEST(test_vector, near_zero_detection) {
  render::vector const v{1e-10, -1e-10, 1e-10};
  EXPECT_TRUE(v.near_zero());
}

TEST(test_vector, near_zero_completely_zero) {
  render::vector const v(0.0, 0.0, 0.0);
  EXPECT_TRUE(v.near_zero());
}

TEST(test_vector, near_zero_normal_vector) {
  render::vector const v(1.0, 2.0, 3.0);
  EXPECT_FALSE(v.near_zero());
}

TEST(test_vector, near_zero_mixed_vector) {
  render::vector const v(1e-10, 1.0, 1e-10);
  EXPECT_FALSE(v.near_zero());
}

TEST(test_vector, near_zero_outside_limit) {
  render::vector const v(1e-7, 0.0, 0.0);
  EXPECT_FALSE(v.near_zero());
}

TEST(test_vector, default_constructor_is_zero) {
  render::vector const v;
  EXPECT_DOUBLE_EQ(v.get_x(), 0.0);
  EXPECT_DOUBLE_EQ(v.get_y(), 0.0);
  EXPECT_DOUBLE_EQ(v.get_z(), 0.0);
}

TEST(test_vector, parameterized_constructor_sets_values) {
  render::vector const v(1.2, -3.4, 5.6);
  EXPECT_DOUBLE_EQ(v.get_x(), 1.2);
  EXPECT_DOUBLE_EQ(v.get_y(), -3.4);
  EXPECT_DOUBLE_EQ(v.get_z(), 5.6);
}

// operator
TEST(test_vector, operator_add) {
  render::vector const v1(1.0, 2.0, 3.0);
  render::vector const v2(10.0, 20.0, 30.0);

  auto resultado = v1 + v2;

  EXPECT_DOUBLE_EQ(resultado.get_x(), 11.0);
  EXPECT_DOUBLE_EQ(resultado.get_y(), 22.0);
  EXPECT_DOUBLE_EQ(resultado.get_z(), 33.0);
}

TEST(test_vector, operator_subtract) {
  render::vector const v1(10.0, 20.0, 30.0);
  render::vector const v2(1.0, 2.0, 3.0);

  auto resultado = v1 - v2;

  EXPECT_DOUBLE_EQ(resultado.get_x(), 9.0);
  EXPECT_DOUBLE_EQ(resultado.get_y(), 18.0);
  EXPECT_DOUBLE_EQ(resultado.get_z(), 27.0);
}

TEST(test_vector, operator_unary_negation) {
  render::vector const v(1.0, -2.0, 3.0);

  auto resultado = -v;

  EXPECT_DOUBLE_EQ(resultado.get_x(), -1.0);
  EXPECT_DOUBLE_EQ(resultado.get_y(), 2.0);
  EXPECT_DOUBLE_EQ(resultado.get_z(), -3.0);
}

TEST(test_vector, operator_multiply_by_scalar) {
  render::vector const v(1.0, 2.0, 3.0);

  auto resultado = v * 3.0;

  EXPECT_DOUBLE_EQ(resultado.get_x(), 3.0);
  EXPECT_DOUBLE_EQ(resultado.get_y(), 6.0);
  EXPECT_DOUBLE_EQ(resultado.get_z(), 9.0);
}

TEST(test_vector, operator_divide_by_scalar) {
  render::vector const v(3.0, 6.0, 9.0);

  auto resultado = v / 3.0;

  EXPECT_DOUBLE_EQ(resultado.get_x(), 1.0);
  EXPECT_DOUBLE_EQ(resultado.get_y(), 2.0);
  EXPECT_DOUBLE_EQ(resultado.get_z(), 3.0);
}

TEST(test_vector, operator_multiply_component_wise) {
  render::vector const v1(1.0, 2.0, 3.0);
  render::vector const v2(2.0, 3.0, 4.0);

  auto resultado = v1 * v2;

  EXPECT_DOUBLE_EQ(resultado.get_x(), 2.0);   // 1.0 * 2.0
  EXPECT_DOUBLE_EQ(resultado.get_y(), 6.0);   // 2.0 * 3.0
  EXPECT_DOUBLE_EQ(resultado.get_z(), 12.0);  // 3.0 * 4.0
}
