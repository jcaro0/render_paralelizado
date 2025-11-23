#include "vector.hpp"
#include "config.hpp"
#include <cmath>

namespace render {

  double vector::magnitude() const {
    return std::sqrt(_x * _x + _y * _y + _z * _z);
  }

  double vector::magnitude_squared() const {
    return _x * _x + _y * _y + _z * _z;
  }

  vector vector::normalized() const {
    double const n = magnitude();
    if (n < constants::EPS) {
      return {0.0, 0.0, 0.0};
    }
    return vector{_x / n, _y / n, _z / n};
  }

  bool vector::near_zero() const {
    return std::fabs(_x) < constants::EPS and
           std::fabs(_y) < constants::EPS and
           std::fabs(_z) < constants::EPS;
  }

}  // namespace render
