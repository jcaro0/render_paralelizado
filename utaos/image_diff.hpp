#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace render {

  struct Rgb8 {
    std::uint8_t r{0};
    std::uint8_t g{0};
    std::uint8_t b{0};
  };

  struct ImageDifference {
    double max_difference{0.0};
    double rmse{0.0};
  };

  inline constexpr double MAX_ACCEPTABLE_PIXEL_DIFFERENCE = 150.0;
  inline constexpr double MAX_ACCEPTABLE_RMSE             = 10.0;

  bool load_ppm(std::string const & path, std::vector<Rgb8> & pixels, std::size_t & width,
                std::size_t & height);

  bool compute_difference(std::span<Rgb8 const> lhs, std::span<Rgb8 const> rhs,
                          ImageDifference & out) noexcept;

  [[nodiscard]] bool is_within_thresholds(ImageDifference const & diff) noexcept;

}  // namespace render
