#include "cases/ReynoldsScalingCase.hpp"

#include "render/ArrowRenderer.hpp"
#include "render/ColorMap.hpp"

#include "imgui.h"
#include "raylib.h"

#include <algorithm>
#include <cmath>

void ReynoldsScalingCase::update(float /*dt*/) {}

void ReynoldsScalingCase::draw3D() {
  float Re = U_ * L_ / std::max(1e-6f, nu_);
  float regime = std::log10(std::max(0.1f, Re));
  float tReg = std::clamp((regime - 0.0f) / 3.0f, 0.0f, 1.0f);

  float halfH = 1.2f;
  int ny = 18;
  for (int k = 0; k < 3; ++k) {
    float xoff = -2.2f + static_cast<float>(k) * 2.2f;
    const char* labels[3] = {"low Re", "mid Re", "high Re"};
    (void)labels;
    float blend = static_cast<float>(k) / 2.0f;
    float width = halfH * (0.95f - 0.55f * blend * tReg);
    for (int j = 0; j <= ny; ++j) {
      float s = static_cast<float>(j) / static_cast<float>(ny);
      float y = -halfH + 2.0f * halfH * s;
      float dist = std::abs(y) / std::max(0.05f, width);
      float uprof = std::max(0.0f, 1.0f - dist * dist);
      if (k == 0) {
        uprof = std::exp(-dist * dist * 2.5f);
      } else if (k == 2) {
        uprof = dist < 1.0f ? std::sqrt(std::max(0.0f, 1.0f - dist * dist)) : 0.0f;
      }
      Vector3 base{xoff, y, 0.0f};
      Color c = ColorFromScalar(uprof, 0.0f, 1.0f);
      DrawArrow3D(base, {1.0f, 0.0f, 0.0f}, 0.45f * uprof, 0.05f, 0.016f, c);
    }
    DrawLine3D({xoff, -halfH, 0.0f}, {xoff, halfH, 0.0f}, {100, 100, 130, 200});
  }

  DrawArrow3D({2.8f, 0.0f, 0.0f}, {U_, 0.0f, 0.0f}, 0.5f, 0.08f, 0.024f, {255, 255, 255, 200});
}

void ReynoldsScalingCase::drawUI() {
  float Re = U_ * L_ / std::max(1e-6f, nu_);
  ImGui::TextUnformatted("Re = U L / nu. Three panels: diffuse (left), mixed (mid), thin-layer-like (right).");
  ImGui::SliderFloat("U", &U_, 0.1f, 5.0f);
  ImGui::SliderFloat("L", &L_, 0.2f, 3.0f);
  ImGui::SliderFloat("nu", &nu_, 0.001f, 1.0f);
  ImGui::Text("Re = %.4g", Re);
  float inertia = U_ * U_ / std::max(1e-6f, L_);
  float visc = nu_ * U_ / (L_ * L_);
  float s = inertia + visc;
  float bi = s > 1e-8f ? inertia / s : 0.5f;
  ImGui::Text("Order sketch |U grad U| ~ %.3g  |  |nu nabla^2 u| ~ %.3g", inertia, visc);
  ImGui::ProgressBar(bi, ImVec2(-1, 0), "inertia share (crude)");
  ImGui::TextUnformatted("MTH3007: Ch.5.4 BL scaling, Ch.6.1 Stokes vs inertia. MTH3001: indirect scaling only.");
}
