#include "cases/QGPVCase.hpp"

#include "render/ColorMap.hpp"
#include "render/SurfaceRenderer.hpp"

#include "imgui.h"
#include "raylib.h"

#include <cmath>

void QGPVCase::update(float dt) { time_ += dt * 0.4f; }

void QGPVCase::draw3D() {
  float x0 = -halfL_, x1 = halfL_, z0 = -halfL_, z1 = halfL_;
  float dx = (x1 - x0) / static_cast<float>(nx_ - 1);
  float dz = (z1 - z0) / static_cast<float>(nx_ - 1);
  const size_t n = static_cast<size_t>(nx_ * nx_);
  psi0_.resize(n);
  psi1_.resize(n);
  std::vector<float> draw0(n);
  std::vector<float> draw1(n);
  const float zScale = 1.8f;
  const float base0 = 0.22f / zScale;
  const float base1 = 0.72f / zScale;

  for (int j = 0; j < nx_; ++j) {
    for (int i = 0; i < nx_; ++i) {
      float x = x0 + static_cast<float>(i) * dx;
      float z = z0 + static_cast<float>(j) * dz;
      float ph = k_ * x + 0.7f * k_ * z - time_;
      float p0 = std::sin(ph) * std::cos(0.5f * k_ * z);
      float p1 = std::cos(ph * 0.9f) * std::sin(0.5f * k_ * x) + coupling_ * p0;
      size_t id = static_cast<size_t>(j * nx_ + i);
      psi0_[id] = 0.12f * p0;
      psi1_[id] = 0.12f * (p1 + 0.02f * std::sin(k_ * time_));
      draw0[id] = base0 + psi0_[id];
      draw1[id] = base1 + psi1_[id];
    }
  }

  DrawHeightFieldSurface(nx_, nx_, draw0.data(), x0, x1, z0, z1, zScale, {200, 220, 255, 255});
  if (layers_ >= 2) {
    DrawHeightFieldSurface(nx_, nx_, draw1.data(), x0, x1, z0, z1, zScale, {255, 210, 190, 255});
  }

  for (int j = 0; j < nx_ - 1; j += 3) {
    for (int i = 0; i < nx_ - 1; i += 3) {
      size_t id = static_cast<size_t>(j * nx_ + i);
      float x = x0 + static_cast<float>(i) * dx;
      float z = z0 + static_cast<float>(j) * dz;
      float yA = zScale * draw0[id];
      float yB = layers_ >= 2 ? zScale * draw1[id] : yA + 0.15f;
      DrawLine3D({x, yA, z}, {x, yB, z}, {160, 160, 200, 100});
    }
  }
}

void QGPVCase::drawUI() {
  ImGui::TextUnformatted("Two stacked psi(x,z) surfaces; vertical lines hint layer coupling.");
  ImGui::SliderFloat("k (wavenumber)", &k_, 0.4f, 2.5f);
  ImGui::SliderFloat("layer coupling", &coupling_, 0.0f, 0.9f);
  ImGui::SliderInt("layers", &layers_, 1, 2);
  ImGui::TextUnformatted("MTH3001: Ch.5.4 QGPV. MTH3007: vorticity / Rossby ideas only.");
}
