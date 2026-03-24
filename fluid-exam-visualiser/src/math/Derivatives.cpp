#include "math/Derivatives.hpp"

#include <cmath>

static void matchLayout(const VecField3D& u, ScalarField3D& s) {
  s.nx = u.nx;
  s.ny = u.ny;
  s.nz = u.nz;
  s.xmin = u.xmin;
  s.xmax = u.xmax;
  s.ymin = u.ymin;
  s.ymax = u.ymax;
  s.zmin = u.zmin;
  s.zmax = u.zmax;
  s.dx = u.dx;
  s.dy = u.dy;
  s.dz = u.dz;
  s.data.assign(static_cast<size_t>(u.nx * u.ny * u.nz), 0.0f);
}

void computeSpeed(const VecField3D& u, ScalarField3D& outSpeed) {
  matchLayout(u, outSpeed);
  for (int k = 0; k < u.nz; ++k) {
    for (int j = 0; j < u.ny; ++j) {
      for (int i = 0; i < u.nx; ++i) {
        const Vector3& v = u.at(i, j, k);
        outSpeed.at(i, j, k) = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
      }
    }
  }
}

void computeDivergence(const VecField3D& u, ScalarField3D& outDiv) {
  matchLayout(u, outDiv);
  for (int k = 0; k < u.nz; ++k) {
    for (int j = 0; j < u.ny; ++j) {
      for (int i = 0; i < u.nx; ++i) {
        if (i == 0 || i == u.nx - 1 || j == 0 || j == u.ny - 1 || k == 0 || k == u.nz - 1) {
          outDiv.at(i, j, k) = 0.0f;
          continue;
        }
        float dux = (u.at(i + 1, j, k).x - u.at(i - 1, j, k).x) / (2.0f * u.dx);
        float duy = (u.at(i, j + 1, k).y - u.at(i, j - 1, k).y) / (2.0f * u.dy);
        float duz = (u.at(i, j, k + 1).z - u.at(i, j, k - 1).z) / (2.0f * u.dz);
        outDiv.at(i, j, k) = dux + duy + duz;
      }
    }
  }
}

void computeCurl(const VecField3D& u, VecField3D& outCurl) {
  outCurl.resize(u.nx, u.ny, u.nz, u.xmin, u.xmax, u.ymin, u.ymax, u.zmin, u.zmax);
  for (int k = 0; k < u.nz; ++k) {
    for (int j = 0; j < u.ny; ++j) {
      for (int i = 0; i < u.nx; ++i) {
        if (i == 0 || i == u.nx - 1 || j == 0 || j == u.ny - 1 || k == 0 || k == u.nz - 1) {
          outCurl.at(i, j, k) = {0, 0, 0};
          continue;
        }
        // ω = ∇×u
        float dw_dy = (u.at(i, j + 1, k).z - u.at(i, j - 1, k).z) / (2.0f * u.dy);
        float dv_dz = (u.at(i, j, k + 1).y - u.at(i, j, k - 1).y) / (2.0f * u.dz);
        float du_dz = (u.at(i, j, k + 1).x - u.at(i, j, k - 1).x) / (2.0f * u.dz);
        float dw_dx = (u.at(i + 1, j, k).z - u.at(i - 1, j, k).z) / (2.0f * u.dx);
        float dv_dx = (u.at(i + 1, j, k).y - u.at(i - 1, j, k).y) / (2.0f * u.dx);
        float du_dy = (u.at(i, j + 1, k).x - u.at(i, j - 1, k).x) / (2.0f * u.dy);
        float wx = dw_dy - dv_dz;
        float wy = du_dz - dw_dx;
        float wz = dv_dx - du_dy;
        outCurl.at(i, j, k) = {wx, wy, wz};
      }
    }
  }
}

void computeVorticityMagnitude(const VecField3D& curl, ScalarField3D& out) {
  matchLayout(curl, out);
  for (int k = 0; k < curl.nz; ++k) {
    for (int j = 0; j < curl.ny; ++j) {
      for (int i = 0; i < curl.nx; ++i) {
        const Vector3& w = curl.at(i, j, k);
        out.at(i, j, k) = std::sqrt(w.x * w.x + w.y * w.y + w.z * w.z);
      }
    }
  }
}
