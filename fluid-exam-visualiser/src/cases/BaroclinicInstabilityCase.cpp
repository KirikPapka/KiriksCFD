#include "cases/BaroclinicInstabilityCase.hpp"

#include "render/ArrowRenderer.hpp"
#include "render/ColorMap.hpp"

#include "imgui.h"
#include "raylib.h"

#include <algorithm>
#include <cmath>

void BaroclinicInstabilityCase::update(float dt) { time_ += dt * timeScale_; }

void BaroclinicInstabilityCase::draw3D() {
  float amp = 0.15f * (1.0f - std::exp(-growth_ * time_));
  amp = std::min(amp, 0.35f);
  float x0 = -2.0f, x1 = 2.0f;
  int nx = 10;
  int ny = 8;
  for (int j = 0; j <= ny; ++j) {
    float fj = static_cast<float>(j) / static_cast<float>(ny);
    float y = 0.15f + fj * 1.1f;
    float u = Lambda_ * (y - 0.15f);
    for (int i = 0; i <= nx; ++i) {
      float fi = static_cast<float>(i) / static_cast<float>(nx);
      float x = x0 + (x1 - x0) * fi;
      float phase = k_ * x - 0.4f * time_;
      float ztilt = -tilt_ * (y - 0.6f) * std::sin(phase);
      Vector3 base{x, y, ztilt};
      DrawArrow3D(base, {u, 0.0f, 0.0f}, 0.22f * std::abs(u) / std::max(0.2f, Lambda_), 0.04f, 0.014f,
                  ColorFromScalar(u, -1.0f, 1.0f));
    }
  }
  for (int s = 0; s < 5; ++s) {
    float ys = 0.2f + static_cast<float>(s) * 0.22f;
    int m = 36;
    for (int i = 0; i < m; ++i) {
      float fi = static_cast<float>(i) / static_cast<float>(m - 1);
      float x = x0 + (x1 - x0) * fi;
      float phase = k_ * x - 0.4f * time_;
      float z0 = -tilt_ * (ys - 0.6f) * std::sin(phase);
      float x1s = x + (x1 - x0) / static_cast<float>(m - 1);
      float ph1 = k_ * x1s - 0.4f * time_;
      float z1s = -tilt_ * (ys - 0.6f) * std::sin(ph1) + amp * std::sin(phase);
      DrawLine3D({x, ys, z0}, {x1s, ys, z1s}, {255, 180, 140, 160});
    }
  }
}

void BaroclinicInstabilityCase::drawUI() {
  ImGui::TextUnformatted("Background shear u ~ Lambda * y; sloping isentrope sketch in xz sections.");
  ImGui::SliderFloat("Lambda (shear)", &Lambda_, 0.15f, 1.2f);
  ImGui::SliderFloat("k (zonal wavenumber)", &k_, 0.6f, 3.5f);
  ImGui::SliderFloat("tilt with height", &tilt_, 0.1f, 0.8f);
  ImGui::SliderFloat("growth (visual envelope)", &growth_, 0.05f, 0.6f);
  ImGui::SliderFloat("time scale", &timeScale_, 0.0f, 2.0f);
  ImGui::TextUnformatted("MTH3001: Ch.5.6 baroclinic / Eady. MTH3007: (n/a).");
}
