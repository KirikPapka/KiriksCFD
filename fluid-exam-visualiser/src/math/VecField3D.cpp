#include "math/VecField3D.hpp"

#include <algorithm>
#include <cmath>

void VecField3D::resize(int nxi, int nyi, int nzi, float x0, float x1, float y0, float y1,
                        float z0, float z1) {
  nx = std::max(2, nxi);
  ny = std::max(2, nyi);
  nz = std::max(2, nzi);
  xmin = x0;
  xmax = x1;
  ymin = y0;
  ymax = y1;
  zmin = z0;
  zmax = z1;
  dx = (xmax - xmin) / static_cast<float>(nx - 1);
  dy = (ymax - ymin) / static_cast<float>(ny - 1);
  dz = (zmax - zmin) / static_cast<float>(nz - 1);
  data_.assign(static_cast<size_t>(nx * ny * nz), {0, 0, 0});
}

Vector3 VecField3D::nodePosition(int i, int j, int k) const {
  return {xmin + static_cast<float>(i) * dx, ymin + static_cast<float>(j) * dy,
          zmin + static_cast<float>(k) * dz};
}

static Vector3 analyticVelocity(Vector3 p, AnalyticFieldId id, float a) {
  float x = p.x, y = p.y, z = p.z;
  switch (id) {
    case AnalyticFieldId::Shear2D:
      return {y, x, 0.0f};
    case AnalyticFieldId::Helical:
      return {z, a, -x};
    case AnalyticFieldId::SinCos2D:
    default:
      return {std::sin(y), std::cos(x), 0.0f};
  }
}

void VecField3D::fillAnalytic(AnalyticFieldId id, float paramA) {
  for (int k = 0; k < nz; ++k) {
    for (int j = 0; j < ny; ++j) {
      for (int i = 0; i < nx; ++i) {
        at(i, j, k) = analyticVelocity(nodePosition(i, j, k), id, paramA);
      }
    }
  }
}

Vector3 VecField3D::sample(Vector3 p) const {
  auto clampf = [](float v, float lo, float hi) { return std::clamp(v, lo, hi); };
  float fx = (p.x - xmin) / dx;
  float fy = (p.y - ymin) / dy;
  float fz = (p.z - zmin) / dz;
  fx = clampf(fx, 0.0f, static_cast<float>(nx - 1));
  fy = clampf(fy, 0.0f, static_cast<float>(ny - 1));
  fz = clampf(fz, 0.0f, static_cast<float>(nz - 1));
  int i0 = static_cast<int>(std::floor(fx));
  int j0 = static_cast<int>(std::floor(fy));
  int k0 = static_cast<int>(std::floor(fz));
  int i1 = std::min(i0 + 1, nx - 1);
  int j1 = std::min(j0 + 1, ny - 1);
  int k1 = std::min(k0 + 1, nz - 1);
  float tx = fx - static_cast<float>(i0);
  float ty = fy - static_cast<float>(j0);
  float tz = fz - static_cast<float>(k0);
  auto lerp3 = [](const Vector3& a, const Vector3& b, float t) {
    return Vector3{a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t};
  };
  const Vector3& c000 = at(i0, j0, k0);
  const Vector3& c100 = at(i1, j0, k0);
  const Vector3& c010 = at(i0, j1, k0);
  const Vector3& c110 = at(i1, j1, k0);
  const Vector3& c001 = at(i0, j0, k1);
  const Vector3& c101 = at(i1, j0, k1);
  const Vector3& c011 = at(i0, j1, k1);
  const Vector3& c111 = at(i1, j1, k1);
  Vector3 x00 = lerp3(c000, c100, tx);
  Vector3 x10 = lerp3(c010, c110, tx);
  Vector3 x01 = lerp3(c001, c101, tx);
  Vector3 x11 = lerp3(c011, c111, tx);
  Vector3 y0 = lerp3(x00, x10, ty);
  Vector3 y1 = lerp3(x01, x11, ty);
  return lerp3(y0, y1, tz);
}
