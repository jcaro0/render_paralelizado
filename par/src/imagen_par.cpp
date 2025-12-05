#include "imagen_par.hpp"
#include <cstddef>
#include <fstream>
#include <string>
#include <utility>

namespace render {

void ImagenPAR::store_pixel(int x, int y, PixelPAR const &p) {
  if (x < 0 or y < 0) {
    return;
  }
  if (std::cmp_greater_equal(x, width) or std::cmp_greater_equal(y, height)) {
    return;
  }
  at(x, y) = p;
}

bool ImagenPAR::write_ppm(std::string const &path) const {
  std::ofstream out(path);
  if (!out.is_open()) {
    return false;
  }
  out << "P3\n" << width << " " << height << "\n255\n";
  for (size_t y = 0; y < height; ++y) {
    for (size_t x = 0; x < width; ++x) {
      auto const &p = at(static_cast<int>(x), static_cast<int>(y));
      out << int(p.r) << " " << int(p.g) << " " << int(p.b) << " ";
    }
    out << "\n";
  }
  return true;
}

} // namespace render
