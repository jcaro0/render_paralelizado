#include "config.hpp"
#include <cstdint>
#include <gtest/gtest.h>

// seed_material_rng
TEST(test_seed_material_rng, seed_is_reproducible) {
  std::uint64_t const seed = 42;

  render::seed_material_rng(seed);
  double const val1 = render::random_double_01();

  render::seed_material_rng(seed);
  double const val2 = render::random_double_01();

  EXPECT_DOUBLE_EQ(val1, val2);
}

TEST(test_seed_material_rng, different_seeds_are_different) {
  render::seed_material_rng(123);
  double const val1 = render::random_double_01();

  render::seed_material_rng(456);
  double const val2 = render::random_double_01();

  EXPECT_NE(val1, val2);
}

// random_double_01
TEST(test_random_double_01, state_advances) {
  render::seed_material_rng(123);
  double const val1 = render::random_double_01();
  double const val2 = render::random_double_01();
  EXPECT_NE(val1, val2);
}

TEST(test_random_double_01, range_is_correct) {
  render::seed_material_rng(123);
  for (int i = 0; i < 100; ++i) {
    double const val = render::random_double_01();
    EXPECT_GE(val, 0.0);
    EXPECT_LT(val, 1.0);
  }
}

// random_double_in_range
TEST(test_random_double_in_range, range_is_correct) {
  double const min = 10.0;
  double const max = 20.0;

  render::seed_material_rng(123);

  for (int i = 0; i < 100; ++i) {
    double const val = render::random_double_in_range(min, max);
    EXPECT_GE(val, min);
    EXPECT_LT(val, max);
  }
}

TEST(test_random_double_in_range, seed_is_reproducible) {
  std::uint64_t const seed = 42;

  render::seed_material_rng(seed);
  double const val1 = render::random_double_in_range(50.0, 100.0);

  render::seed_material_rng(seed);
  double const val2 = render::random_double_in_range(50.0, 100.0);

  EXPECT_DOUBLE_EQ(val1, val2);
}
