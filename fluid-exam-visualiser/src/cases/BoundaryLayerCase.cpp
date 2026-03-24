#include "cases/BoundaryLayerCase.hpp"

#include "render/ArrowRenderer.hpp"
#include "render/ColorMap.hpp"

#include "imgui.h"
#include "raylib.h"

#include <algorithm>
#include <cmath>

// Plate in xz at y=0; boundary layer grows in +y; free stream +x.

static float pohlhausenUoverU(float y, float delta) {
  if (delta < 1e-6f) {
    return 0.0f;
  }
  float eta = std::clamp(y / delta, 0.0f, 1.0f);
  return 2.0f * eta - 2.0f * eta * eta * eta + eta * eta * eta * eta;
}

static float wallShearScale(float U, float delta) {
  if (delta < 1e-6f) {
    return 0.0f;
  }
  return U * 2.0f / delta;
}

void BoundaryLayerCase::update(float /*dt*/) {}

void BoundaryLayerCase::draw3D() {
  float x0 = 0.05f;
  float x1 = xPlate_;
  float z0 = -zPlate_ * 0.5f;
  float z1 = zPlate_ * 0.5f;
  DrawLine3D({x0, 0.0f, z0}, {x1, 0.0f, z0}, {90, 90, 110, 255});
  DrawLine3D({x0, 0.0f, z1}, {x1, 0.0f, z1}, {90, 90, 110, 255});
  DrawLine3D({x0, 0.0f, z0}, {x0, 0.0f, z1}, {90, 90, 110, 255});
  DrawLine3D({x1, 0.0f, z0}, {x1, 0.0f, z1}, {90, 90, 110, 255});

  int nx = std::max(4, static_cast<int>(xPlate_) * 3);
  for (int i = 0; i <= nx; ++i) {
    float t = static_cast<float>(i) / static_cast<float>(nx);
    float x = x0 + t * (x1 - x0);
    float Rex = U_ * x / std::max(1e-6f, nu_);
    float delta = 5.0f * x / std::sqrt(std::max(1.0f, Rex));
    float tau = wallShearScale(U_, delta);
    Color wc = ColorFromScalar(tau, 0.0f, 8.0f * U_ / std::max(1e-3f, x1));
    float zz = z0 + 0.02f;
    DrawCube({x, 0.02f, zz}, 0.12f, 0.04f, 0.08f, wc);
  }

  for (int k = 0; k < profileCount_; ++k) {
    float t = (profileCount_ <= 1) ? 0.5f : static_cast<float>(k) / static_cast<float>(profileCount_ - 1);
    float x = x0 + t * (x1 - x0);
    float Rex = U_ * x / std::max(1e-6f, nu_);
    float delta = 5.0f * x / std::sqrt(std::max(1.0f, Rex));
    int ny = 24;
    for (int j = 0; j < ny; ++j) {
      float s0 = static_cast<float>(j) / static_cast<float>(ny);
      float s1 = static_cast<float>(j + 1) / static_cast<float>(ny);
      float y0p = s0 * 2.2f * delta;
      float y1p = s1 * 2.2f * delta;
      float u0 = U_ * pohlhausenUoverU(y0p, delta);
      float u1 = U_ * pohlhausenUoverU(y1p, delta);
      Vector3 a{x, y0p, 0.0f};
      Vector3 b{x, y1p, 0.0f};
      Color c0 = ColorFromScalar(u0, 0.0f, U_);
      DrawLine3D(a, b, c0);
      if (j % 4 == 0 && u0 > 1e-4f) {
        DrawArrow3D(a, {1.0f, 0.0f, 0.0f}, 0.35f * u0 / U_, 0.05f, 0.018f, {200, 220, 255, 200});
      }
    }
  }

  float yFree = 1.4f;
  for (int i = 0; i <= 8; ++i) {
    float t = static_cast<float>(i) / 8.0f;
    float x = x0 + t * (x1 - x0);
    DrawArrow3D({x, yFree, 0.0f}, {U_, 0.0f, 0.0f}, 0.55f, 0.07f, 0.022f, {255, 255, 255, 180});
  }
}

void BoundaryLayerCase::drawUI() {
  ImGui::TextUnformatted("Flat plate (xz at y=0); BL thickness ~ 5x/sqrt(Re_x); Pohlhausen u/U profile.");
  ImGui::SliderFloat("U (free stream)", &U_, 0.3f, 3.0f);
  ImGui::SliderFloat("nu (kinematic viscosity)", &nu_, 0.002f, 0.2f);
  ImGui::SliderFloat("plate length (x)", &xPlate_, 1.5f, 6.0f);
  ImGui::SliderFloat("plate width (z)", &zPlate_, 0.5f, 2.5f);
  ImGui::SliderInt("profile stations", &profileCount_, 2, 10);
  float xmid = 0.5f * xPlate_;
  float Rex = U_ * xmid / std::max(1e-6f, nu_);
  float delta = 5.0f * xmid / std::sqrt(std::max(1.0f, Rex));
  ImGui::Text("Re_x at x=L/2 ~ %.1f  |  delta ~ %.3f", Rex, delta);
  ImGui::TextUnformatted("MTH3007: Ch.5.4-5.5 boundary layer / Blasius. MTH3001: (n/a).");
}
