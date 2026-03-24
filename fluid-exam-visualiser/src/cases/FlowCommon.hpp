#pragma once

#include "raylib.h"

#include <cmath>

// Shared small helpers for exam-style visuals (keep in one place to avoid duplication).

inline Vector3 SphericalThetaHat(float theta, float phi) {
  // Polar axis +z: θ from +z, φ azimuth from +x in xy-plane.
  float st = std::sin(theta);
  float ct = std::cos(theta);
  float cp = std::cos(phi);
  float sp = std::sin(phi);
  return {ct * cp, ct * sp, -st};
}

inline void CartesianFromSpherical(float r, float theta, float phi, float* x, float* y, float* z) {
  float st = std::sin(theta);
  *x = r * st * std::cos(phi);
  *y = r * st * std::sin(phi);
  *z = r * std::cos(theta);
}
