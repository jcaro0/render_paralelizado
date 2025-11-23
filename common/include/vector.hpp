#ifndef RENDER_VECTOR_HPP
#define RENDER_VECTOR_HPP

#include <cmath>
#include <ostream>

namespace render {

  class vector {
  public:
    vector(double cx, double cy, double cz) : _x{cx}, _y{cy}, _z{cz} { }

    vector() : _x{0.0}, _y{0.0}, _z{0.0} { }

    // Getters para los componentes
    [[nodiscard]] double get_x() const { return _x; }

    [[nodiscard]] double get_y() const { return _y; }

    [[nodiscard]] double get_z() const { return _z; }

    [[nodiscard]] double magnitude() const;

    [[nodiscard]] double magnitude_squared() const;

    // Operaciones básicas
    friend vector operator+(vector const & a, vector const & b) {
      return {a._x + b._x, a._y + b._y, a._z + b._z};
    }

    friend vector operator-(vector const & a, vector const & b) {
      return {a._x - b._x, a._y - b._y, a._z - b._z};
    }

    friend vector operator-(vector const & b) { return {-b._x, -b._y, -b._z}; }

    friend vector operator*(vector const & v1, vector const & v2) {
      return {v1._x * v2._x, v1._y * v2._y, v1._z * v2._z};
    }

    friend vector operator*(vector const & v, double s) { return {v._x * s, v._y * s, v._z * s}; }

    friend vector operator*(double s, vector const & v) { return v * s; }

    friend vector operator/(vector const & v, double s) { return {v._x / s, v._y / s, v._z / s}; }

    // Producto escalar
    friend double dot(vector const & a, vector const & b) {
      return a._x * b._x + a._y * b._y + a._z * b._z;
    }

    // Producto vectorial
    friend vector cross(vector const & a, vector const & b) {
      return {a._y * b._z - a._z * b._y, a._z * b._x - a._x * b._z, a._x * b._y - a._y * b._x};
    }

    // Normalización
    [[nodiscard]] vector normalized() const;
    // Si es muy cerca de cero lo "descartamos" (ponemos a cero)
    [[nodiscard]] bool near_zero() const;

    //__STR__
    friend std::ostream & operator<<(std::ostream & os, vector const & v) {
      os << "(" << v._x << ", " << v._y << ", " << v._z << ")";
      return os;
    }

  private:
    double _x, _y, _z;
  };

}  // namespace render

#endif
