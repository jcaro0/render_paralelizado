#ifndef RENDER_IMAGEN_SOA_HPP
#define RENDER_IMAGEN_SOA_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace render {

  struct RawPixel {
    int r;
    int g;
    int b;
  };

  class ImagenSOA {
  public:
    ImagenSOA(int w, int h);

    /**
     * @brief Almacena un píxel.
     * Acepta RawPixel (3 parámetros en total)
     */
    void store_pixel(int x, int y, RawPixel const & pixel);

    [[nodiscard]] bool write_ppm(std::string const & path) const;

  private:
    size_t width;
    size_t height;
    std::vector<std::uint8_t> red_channel;
    std::vector<std::uint8_t> green_channel;
    std::vector<std::uint8_t> blue_channel;

    // Funciones auxiliares privadas
    void check_bounds(int x, int y) const;
    [[nodiscard]] size_t index(int x, int y) const noexcept;
  };

}  // namespace render

#endif  // RENDER_IMAGEN_SOA_HPP
