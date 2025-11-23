#ifndef RENDER_RAYO_HPP
#define RENDER_RAYO_HPP

#include "vector.hpp"

namespace render {

  class rayo {
  public:
    // Constructores
    rayo() = default;

    // El constructor ya NO normaliza la dirección
    rayo(vector const & origen, vector const & direccion)
        : _origen(origen), _direccion(direccion) { }

    // Getters
    [[nodiscard]] vector const & get_origen() const { return _origen; }

    // Devuelve la dirección SIN normalizar (para logs y 'at')
    [[nodiscard]] vector const & get_direccion() const { return _direccion; }

    // Devuelve la dirección normalizada (para física/cálculos)
    [[nodiscard]] vector get_direccion_normalized() const { return _direccion.normalized(); }

    // Calcula el punto en el rayo para un valor lambda
    // Esto es correcto, 'at' debe usar la dirección original
    // (sin normalizar) para que coincida con el 'lambda' calculado
    // por las funciones de intersección.
    [[nodiscard]] vector at(double lambda) const { return _origen + lambda * _direccion; }

  private:
    vector _origen;
    vector _direccion;  // Esta es ahora la dirección SIN normalizar
  };

}  // namespace render

#endif
