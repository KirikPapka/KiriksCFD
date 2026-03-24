#include "cases/BarotropicInstabilityCase.hpp"

#include "render/ArrowRenderer.hpp"
#include "render/ColorMap.hpp"

#include "imgui.h"
#include "raylib.h"

#include <algorithm>
#include <cmath>

static float sech2(float x) {
  float c = 1.0f / std::cosh(x);
  return c * c;
}

void BarotropicInstabilityCase::update(float dt) { time_ += dt * timeScale_; }

void BarotropicInstabilityCase::draw3D() {
  float y = 0.35f;
  float amp = 0.45f * (1.0f - std::exp(-growth_ * time_));
  amp = std::min(amp, 0.55f);
  int nx = 14;
  int nz = 20;
  float x0 = -2.5f, x1 = 2.5f, z0 = -1.4f, z1 = 1.4f;
  for (int j = 0; j < nz; ++j) {
    for (int i = 0; i < nx; ++i) {
      float fx = static_cast<float>(i) / static_cast<float>(nx - 1);
      float fz = static_cast<float>(j) / static_cast<float>(nz - 1);
      float x = x0 + (x1 - x0) * fx;
      float z = z0 + (z1 - z0) * fz;
      float zj = z / std::max(0.08f, jetW_);
      float U = U0_ * sech2(zj);
      float phase = k_ * x + growth_ * 0.15f * time_;
      float zpert = z + amp * std::sin(phase);
      Vector3 base{x, y, zpert};
      Vector3 dir{U, 0.0f, 0.0f};
      Color c = ColorFromScalar(U, 0.0f, U0_);
      DrawArrow3D(base, dir, 0.28f * U / U0_, 0.045f, 0.015f, c);
    }
  }
  for (int i = 0; i < 40; ++i) {
    float fx = static_cast<float>(i) / 39.0f;
    float x = x0 + (x1 - x0) * fx;
    float phase = k_ * x + growth_ * 0.15f * time_;
    float zc = amp * 1.2f * std::sin(phase);
    DrawLine3D({x, y + 0.02f, zc - 0.05f}, {x, y + 0.02f, zc + 0.05f}, {255, 200, 100, 180});
  }
}

void BarotropicInstabilityCase::drawUI() {
  ImGui::TextUnformatted("Jet U(z) ~ sech^2(z/w); meander grows ~ exp(sigma t) (toy sigma slider).");
  ImGui::SliderFloat("U0", &U0_, 0.4f, 2.5f);
  ImGui::SliderFloat("jet half-width w", &jetW_, 0.15f, 0.9f);
  ImGui::SliderFloat("k (meander)", &k_, 0.8f, 4.0f);
  ImGui::SliderFloat("growth rate (visual)", &growth_, 0.05f, 0.8f);
  ImGui::SliderFloat("time scale", &timeScale_, 0.0f, 2.0f);
  ImGui::TextUnformatted("MTH3001: Ch.5.5 barotropic instability. MTH3007: (n/a).");
}
