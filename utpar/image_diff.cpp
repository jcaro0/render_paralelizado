#include "image_diff.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace render {

  // Funciones auxiliares (limitadas a este fichero)
  namespace {

    // Valor máximo para un canal de color (unsigned char).
    constexpr double BYTE_MAX = 255.0;

    /**
     * @brief Lee el siguiente "token" (palabra) de un stream, ignorando comentarios.
     * Los comentarios en PPM empiezan con '#' y van hasta el final de la línea.
     * @param in El stream de entrada (ej: el fichero).
     * @return Un std::optional<std::string> que contiene el token, o std::nullopt si se
     * llega al final del fichero.
     */
    std::optional<std::string> read_token(std::istream & in) {
      std::string token;
      // Lee palabra por palabra.
      while (in >> token) {
        // Si es un comentario (empieza por #)...
        if (!token.empty() and token.front() == '#') {
          std::string comment;
          // ...lee y descarta el resto de la línea.
          std::getline(in, comment);
          continue;  // Pasa al siguiente token.
        }
        // Devuelve el token válido encontrado.
        return token;
      }
      // Si se acaba el stream (EOF), devuelve "nada".
      return std::nullopt;
    }

    /**
     * @brief Asegura que un valor 'double' esté en el rango [0, 255] y lo convierte a byte.
     */
    [[nodiscard]] std::uint8_t clamp_byte(double value) noexcept {
      // Limita el valor al rango [0.0, 255.0].
      value = std::clamp(value, 0.0, BYTE_MAX);
      // Redondea al entero más cercano (std::lround) y convierte a byte.
      return static_cast<std::uint8_t>(std::lround(value));
    }

    /**
     * @brief Procesa la cabecera de un fichero PPM (formato P3).
     * Una cabecera P3 válida es: "P3\n[ancho] [alto]\n[valor_maximo]\n"
     * @return true si la cabecera es válida, false en caso contrario.
     */
    [[nodiscard]] bool parse_header(std::istream & in, std::size_t & width, std::size_t & height,
                                    double & max_value) {
      // Lee el "número mágico". Debe ser "P3" (ASCII).
      auto const magic = read_token(in);
      if (!magic.has_value() or *magic != "P3") {
        return false;
      }

      // Lee las dimensiones (ancho y alto).
      auto const width_token  = read_token(in);
      auto const height_token = read_token(in);
      if (!width_token.has_value() or !height_token.has_value()) {
        return false;  // Faltan datos.
      }
      // Convierte las dimensiones de string a números (size_t).
      width  = static_cast<std::size_t>(std::stoul(*width_token));
      height = static_cast<std::size_t>(std::stoul(*height_token));

      // 3. Lee el valor máximo del color (ej: 255, 1000, etc.).
      auto const max_value_token = read_token(in);
      if (!max_value_token.has_value()) {
        return false;
      }
      // Convierte el valor máximo a double.
      max_value = static_cast<double>(std::stoul(*max_value_token));

      // La cabecera es válida si el valor máximo es positivo.
      return max_value > 0.0;
    }

    /**
     * @brief Lee todos los datos de píxeles del fichero PPM, aplicándoles la escala.
     * @param total Número total de píxeles (ancho * alto).
     * @param scale Factor para normalizar los valores a [0, 255].
     * @param pixels Vector de salida donde se guardarán los píxeles.
     * @return true si se leyeron todos los píxeles, false si el fichero terminó
     * prematuramente.
     */
    [[nodiscard]] bool read_pixels(std::istream & in, std::size_t total, double scale,
                                   std::vector<Rgb8> & pixels) {
      // Itera para cada píxel (total = ancho * alto).
      for (std::size_t i = 0; i < total; ++i) {
        // Lee los tres componentes (R, G, B) de color.
        auto const r_token = read_token(in);
        auto const g_token = read_token(in);
        auto const b_token = read_token(in);
        // Falla si el fichero se acaba antes de tiempo.
        if (!r_token.has_value() or !g_token.has_value() or !b_token.has_value()) {
          return false;
        }

        // Convierte el valor leído (ej: 512) y lo normaliza a [0, 255] usando la escala.
        double const r_value = static_cast<double>(std::stoi(*r_token)) * scale;
        double const g_value = static_cast<double>(std::stoi(*g_token)) * scale;
        double const b_value = static_cast<double>(std::stoi(*b_token)) * scale;

        // Sujeta (clamp) el valor final a [0, 255] y lo guarda en el vector.
        pixels[i] = Rgb8{clamp_byte(r_value), clamp_byte(g_value), clamp_byte(b_value)};
      }

      return true;
    }

  }  // namespace

  // Implementaciones de funciones públicas (definidas en image_diff.hpp)

  /**
   * @brief Función pública principal: Carga una imagen PPM desde una ruta.
   */
  bool load_ppm(std::string const & path, std::vector<Rgb8> & pixels, std::size_t & width,
                std::size_t & height) {
    // Abre el fichero en disco.
    std::ifstream file(path);
    if (!file.is_open()) {
      return false;  // Falla si no se puede abrir.
    }

    double max_value = 0.0;
    // Llama a la función auxiliar para procesar la cabecera.
    if (!parse_header(file, width, height, max_value)) {
      return false;  // Falla si la cabecera no es válida.
    }

    // Pre-reserva la memoria para todos los píxeles.
    std::size_t const total = width * height;
    pixels.resize(total);

    // Calcula el factor de escala. Si el valor máximo era 1000,
    // la escala será 255.0 / 1000.0 = 0.255.
    double const scale = BYTE_MAX / max_value;

    // Llama a la función auxiliar para leer todos los píxeles.
    return read_pixels(file, total, scale, pixels);
  }

  /**
   * @brief Compara dos imágenes (vectores de píxeles) y calcula las métricas de
   * diferencia.
   * @param lhs "Left-hand side" (Imagen A).
   * @param rhs "Right-hand side" (Imagen B).
   * @param out Estructura de salida para guardar los resultados (Max Diff y RMSE).
   * @return true si la comparación fue posible, false si las imágenes tienen
   * tamaños distintos.
   */
  bool compute_difference(std::span<Rgb8 const> lhs, std::span<Rgb8 const> rhs,
                          ImageDifference & out) noexcept {
    if (lhs.size() != rhs.size()) {
      return false;
    }

    if (lhs.empty()) {
      out.max_difference = 0.0;
      out.rmse           = 0.0;
      return true;
    }

    // Acumuladores para la diferencia máxima y la raíz del error cuadrático medio (RMSE).
    double max_diff = 0.0;
    double accum_sq = 0.0;
    // Factor de normalización para el RMSE (1.0 / número_de_píxeles).
    double const norm = 1.0 / static_cast<double>(lhs.size());

    for (std::size_t i = 0; i < lhs.size(); ++i) {
      auto const & a = lhs[i];  // Píxel de la imagen A
      auto const & b = rhs[i];  // Píxel de la imagen B

      // Calcula la diferencia media de este píxel (promedio de la diferencia de R, G y B).
      // Se castea a 'int' para evitar desbordamiento con std::abs en 'unsigned char'.
      double const diff = (std::abs(static_cast<int>(a.r) - static_cast<int>(b.r)) +
                           std::abs(static_cast<int>(a.g) - static_cast<int>(b.g)) +
                           std::abs(static_cast<int>(a.b) - static_cast<int>(b.b))) /
                          3.0;
      max_diff = std::max(max_diff, diff);
      accum_sq += diff * diff;
    }

    out.max_difference = max_diff;
    // Calcula y guarda el RMSE (Raíz de (Suma de errores al cuadrado / N)).
    out.rmse = std::sqrt(accum_sq * norm);
    return true;
  }

  /**
   * @brief Comprueba si un objeto 'ImageDifference' está dentro de los umbrales
   * aceptables (definidos en image_diff.hpp).
   */
  bool is_within_thresholds(ImageDifference const & diff) noexcept {
    // Devuelve true solo si tanto la diferencia máxima como el RMSE son aceptables.
    // (Se añade epsilon para comparaciones seguras con números flotantes).
    std::cout << "\nMaxima diferencia de pixel:" << diff.max_difference << "\nRmse:" << diff.rmse
              << "\n\n";
    return diff.max_difference <=
               MAX_ACCEPTABLE_PIXEL_DIFFERENCE + std::numeric_limits<double>::epsilon() and
           diff.rmse <= MAX_ACCEPTABLE_RMSE + std::numeric_limits<double>::epsilon();
  }

}  // namespace render
