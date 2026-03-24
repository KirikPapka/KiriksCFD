#pragma once

#include "raylib.h"

#include <vector>

// Regular Cartesian grid storing u = (ux, uy, uz) at each node.

enum class AnalyticFieldId {
  Shear2D,   // (y, x, 0)
  Helical,   // (z, a, -x)
  SinCos2D,  // (sin y, cos x, 0)
};

class VecField3D {
public:
  int nx = 0, ny = 0, nz = 0;
  float xmin = 0, xmax = 0, ymin = 0, ymax = 0, zmin = 0, zmax = 0;
  float dx = 1, dy = 1, dz = 1;

  void resize(int nxi, int nyi, int nzi, float x0, float x1, float y0, float y1, float z0,
              float z1);
  void fillAnalytic(AnalyticFieldId id, float paramA);

  Vector3 nodePosition(int i, int j, int k) const;
  int index(int i, int j, int k) const { return (k * ny + j) * nx + i; }

  const Vector3& at(int i, int j, int k) const { return data_[index(i, j, k)]; }
  Vector3& at(int i, int j, int k) { return data_[index(i, j, k)]; }

  // Trilinear interpolation (clamped to box).
  Vector3 sample(Vector3 p) const;

  const std::vector<Vector3>& data() const { return data_; }

private:
  std::vector<Vector3> data_;
};
