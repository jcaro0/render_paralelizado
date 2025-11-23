#ifndef RENDER_CONFIG_HPP
#define RENDER_CONFIG_HPP
#include "vector.hpp"
#include <string>

namespace render {

  struct RenderConfig {
    int aspect_ratio_width                = 16;
    int aspect_ratio_height               = 9;
    int image_width                       = 1'920;
    double gamma                          = 2.2;
    render::vector camera_position        = render::vector(0, 0, -10);
    render::vector camera_target          = render::vector(0, 0, 0);
    render::vector camera_north           = render::vector(0, 1, 0);
    double field_of_view                  = 90.0;
    int samples_per_pixel                 = 20;
    int max_depth                         = 5;
    int material_rng_seed                 = 13;
    int ray_rng_seed                      = 19;
    render::vector background_dark_color  = render::vector(0.25, 0.5, 1.0);
    render::vector background_light_color = render::vector(1, 1, 1);
  };

}  // namespace render

bool load_render_config(std::string const & filename, render::RenderConfig & config);

#endif
