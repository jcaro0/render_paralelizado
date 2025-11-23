#include "imagen_soa.hpp"
#include <cstddef>
#include <cstdint>  // Para std::uint8_t
#include <fstream>
#include <ios>
#include <stdexcept>
#include <string>
#include <utility>

namespace {

  /**
   * @brief Limita un valor de color al rango [0, 255] y lo convierte a uint8_t.
   */
  std::uint8_t clamp(int value) {
    if (value < 0) {
      return 0;
    }
    if (value > 255) {
      return 255;
    }
    return static_cast<std::uint8_t>(value);
  }

}  // namespace

namespace render {

  ImagenSOA::ImagenSOA(int w, int h)
      : width(static_cast<size_t>(w)), height(static_cast<size_t>(h)) {
    if (w <= 0 or h <= 0) {
      throw std::invalid_argument("Invalid image dimensions");
    }
    // Inicializa los vectores con el tamaño correcto y valor 0
    size_t const total_pixels = width * height;
    red_channel.resize(total_pixels, 0);
    green_channel.resize(total_pixels, 0);
    blue_channel.resize(total_pixels, 0);
  }

  // Firma y lógica para usar RawPixel
  void ImagenSOA::store_pixel(int x, int y, RawPixel const & pixel) {
    // Ignoramos píxeles fuera de rango
    if (std::cmp_greater_equal(x, width) or std::cmp_greater_equal(y, height)) {
      return;
    }

    size_t const idx = index(x, y);
    // Usamos el helper 'clamp' con los miembros de la estructura
    red_channel[idx]   = clamp(pixel.r);
    green_channel[idx] = clamp(pixel.g);
    blue_channel[idx]  = clamp(pixel.b);
  }

  [[nodiscard]] bool ImagenSOA::write_ppm(std::string const & path) const {
    std::ofstream file(path, std::ios::out bitor std::ios::trunc);
    if (not file.is_open()) {
      return false;
    }

    file << "P3\n" << width << " " << height << "\n255\n";
    for (size_t y = 0; y < height; ++y) {
      for (size_t x = 0; x < width; ++x) {
        size_t const idx = y * width + x;
        // El casteo a (int) es para la salida en el fichero PPM
        file << static_cast<int>(red_channel[idx]) << " " << static_cast<int>(green_channel[idx])
             << " " << static_cast<int>(blue_channel[idx]) << "\n";
      }
    }
    return true;
  }

  void ImagenSOA::check_bounds(int x, int y) const {
    if (std::cmp_less(x, 0) or
        std::cmp_less(y, 0) or
        std::cmp_greater_equal(x, width) or
        std::cmp_greater_equal(y, height))
    {
      throw std::out_of_range("Pixel coordinates out of bounds");
    }
  }

  [[nodiscard]] size_t ImagenSOA::index(int x, int y) const noexcept {
    return static_cast<size_t>(y) * width + static_cast<size_t>(x);
  }

}  // namespace render
