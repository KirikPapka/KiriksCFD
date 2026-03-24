#pragma once

#include "math/VecField3D.hpp"

#include <vector>

struct ScalarField3D {
  int nx = 0, ny = 0, nz = 0;
  float xmin = 0, xmax = 0, ymin = 0, ymax = 0, zmin = 0, zmax = 0;
  float dx = 1, dy = 1, dz = 1;
  std::vector<float> data;

  float at(int i, int j, int k) const { return data[static_cast<size_t>((k * ny + j) * nx + i)]; }
  float& at(int i, int j, int k) { return data[static_cast<size_t>((k * ny + j) * nx + i)]; }
};

// Central differences on the same grid as the vector field (interior only; boundaries zero).
void computeDivergence(const VecField3D& u, ScalarField3D& outDiv);
void computeCurl(const VecField3D& u, VecField3D& outCurl);
void computeSpeed(const VecField3D& u, ScalarField3D& outSpeed);
void computeVorticityMagnitude(const VecField3D& curl, ScalarField3D& out);
