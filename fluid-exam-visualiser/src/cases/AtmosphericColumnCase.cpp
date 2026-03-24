#include "cases/AtmosphericColumnCase.hpp"

#include <algorithm>

#include "render/ColorMap.hpp"

#include "imgui.h"
#include "raylib.h"

#include <cmath>

// Isothermal: p = p0 exp(-z/H), H = R T / g sketch. Adiabatic: T = T0 - Gamma z, p from hydrostatic.

void AtmosphericColumnCase::update(float /*dt*/) {}

void AtmosphericColumnCase::recomputeProfiles() {
  zPlot_.resize(static_cast<size_t>(nZ_));
  pPlot_.resize(static_cast<size_t>(nZ_));
  tPlot_.resize(static_cast<size_t>(nZ_));
  const float g = 9.81f;
  const float R = 287.0f;
  for (int i = 0; i < nZ_; ++i) {
    float z = static_cast<float>(i) / static_cast<float>(nZ_ - 1) * zTop_;
    zPlot_[static_cast<size_t>(i)] = z;
    if (mode_ == 0) {
      float H = std::max(100.0f, Hscale_);
      float p = p0_ * std::exp(-z / H);
      float T = T0_;
      pPlot_[static_cast<size_t>(i)] = p / p0_;
      tPlot_[static_cast<size_t>(i)] = T;
    } else {
      float T = std::max(150.0f, T0_ - Gamma_ * z);
      float p = p0_ * std::pow(T / T0_, g / (R * std::max(1e-4f, Gamma_)));
      pPlot_[static_cast<size_t>(i)] = p / p0_;
      tPlot_[static_cast<size_t>(i)] = T;
    }
  }
}

void AtmosphericColumnCase::draw3D() {
  recomputeProfiles();
  float zScale = 2.5e-4f;
  DrawLine3D({0.0f, 0.0f, 0.0f}, {0.0f, zTop_ * zScale, 0.0f}, {100, 100, 130, 200});
  int n = std::min(nZ_, 24);
  for (int i = 0; i < n; ++i) {
    int idx = static_cast<int>((static_cast<float>(i) / static_cast<float>(n - 1)) * static_cast<float>(nZ_ - 1));
    float z = zPlot_[static_cast<size_t>(idx)] * zScale;
    float T = tPlot_[static_cast<size_t>(idx)];
    Color c = ColorFromScalar(T, 220.0f, 310.0f);
    DrawSphere({0.0f, z, 0.0f}, 0.06f, c);
  }
}

void AtmosphericColumnCase::drawUI() {
  recomputeProfiles();
  ImGui::TextUnformatted("Hydrostatic column: isothermal p(z) or dry adiabatic T(z) with Gamma_d ~ g/cp.");
  const char* items[] = {"isothermal T0, scale height H", "adiabatic lapse Gamma"};
  ImGui::Combo("mode", &mode_, items, 2);
  ImGui::SliderFloat("T0 (K)", &T0_, 250.0f, 310.0f);
  ImGui::SliderFloat("p0 (Pa)", &p0_, 80000.0f, 110000.0f);
  if (mode_ == 0) {
    ImGui::SliderFloat("scale height H (m)", &Hscale_, 5000.0f, 12000.0f);
  } else {
    ImGui::SliderFloat("Gamma (K/m)", &Gamma_, 0.005f, 0.012f);
  }
  ImGui::SliderFloat("z_top (m)", &zTop_, 5000.0f, 20000.0f);
  ImGui::PlotLines("p/p0", pPlot_.data(), nZ_, 0, nullptr, 0.0f, 1.0f, ImVec2(0, 80));
  ImGui::PlotLines("T (K)", tPlot_.data(), nZ_, 0, nullptr, 200.0f, 320.0f, ImVec2(0, 80));
}
