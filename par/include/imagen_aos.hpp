#ifndef RENDER_IMAGEN_AOS_HPP
#define RENDER_IMAGEN_AOS_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace render {

  struct PixelAOS {
    std::uint8_t r{0}, g{0}, b{0};
    PixelAOS() = default;

    PixelAOS(int ri, int gi, int bi) {
      if (ri < 0) {
        ri = 0;
      } else if (ri > 255) {
        ri = 255;
      }

      if (gi < 0) {
        gi = 0;
      } else if (gi > 255) {
        gi = 255;
      }

      if (bi < 0) {
        bi = 0;
      } else if (bi > 255) {
        bi = 255;
      }

      r = static_cast<std::uint8_t>(ri);
      g = static_cast<std::uint8_t>(gi);
      b = static_cast<std::uint8_t>(bi);
    }
  };

  class ImagenAOS {
  public:
    ImagenAOS(int w, int h)
        : width(static_cast<size_t>(w)), height(static_cast<size_t>(h)), pixels(width * height) { }

    void store_pixel(int x, int y, PixelAOS const & p);
    [[nodiscard]] bool write_ppm(std::string const & path) const;

    size_t width{0}, height{0};

  private:
    std::vector<PixelAOS> pixels;

    PixelAOS & at(int x, int y) {
      return pixels[static_cast<size_t>(y) * width + static_cast<size_t>(x)];
    }

    [[nodiscard]] PixelAOS const & at(int x, int y) const {
      return pixels[static_cast<size_t>(y) * width + static_cast<size_t>(x)];
    }
  };

}  // namespace render

#endif  // RENDER_IMAGEN_AOS_HPP
