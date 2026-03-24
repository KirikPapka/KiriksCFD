#include "cases/RossbyRadiusCase.hpp"

#include <algorithm>

#include "imgui.h"
#include "raylib.h"

#include <cmath>

void RossbyRadiusCase::update(float /*dt*/) {}

void RossbyRadiusCase::draw3D() {
  float Ld = (std::abs(f0_) > 1e-6f) ? std::sqrt(g_ * H_) / std::abs(f0_) : 0.0f;
  float visR = std::clamp(Ld * 0.38f, 0.35f, 2.85f);
  DrawCircle3D({0.0f, 0.14f, 0.0f}, visR, {0.0f, 1.0f, 0.0f}, 0.0f, {160, 190, 255, 220});

  const int nSeg = 96;
  const float x0 = -3.6f;
  const float x1 = 3.6f;
  const float amp = 0.14f;
  auto drawSineZ = [&](float zPlane, float k, Color col) {
    for (int i = 0; i < nSeg; ++i) {
      float t0 = static_cast<float>(i) / static_cast<float>(nSeg);
      float t1 = static_cast<float>(i + 1) / static_cast<float>(nSeg);
      float xa = x0 + (x1 - x0) * t0;
      float xb = x0 + (x1 - x0) * t1;
      float ya = amp * std::sin(k * xa);
      float yb = amp * std::sin(k * xb);
      DrawLine3D({xa, ya + 0.08f, zPlane}, {xb, yb + 0.08f, zPlane}, col);
    }
  };
  drawSineZ(-1.05f, kShort_, {255, 160, 80, 255});
  drawSineZ(1.05f, kLong_, {120, 200, 255, 255});
}

void RossbyRadiusCase::drawUI() {
  float Ld = (std::abs(f0_) > 1e-6f) ? std::sqrt(g_ * H_) / std::abs(f0_) : 0.0f;
  float invLd2 = (Ld > 1e-6f) ? 1.0f / (Ld * Ld) : 0.0f;
  float beta = 1.0f;
  auto rossOmega = [&](float k) { return -beta * k / std::max(1e-6f, k * k + invLd2); };
  float wS = rossOmega(kShort_);
  float wL = rossOmega(kLong_);
  float cpS = (std::abs(kShort_) > 1e-6f) ? wS / kShort_ : 0.0f;
  float cpL = (std::abs(kLong_) > 1e-6f) ? wL / kLong_ : 0.0f;
  ImGui::TextUnformatted("Ld = sqrt(g H)/|f0|; Rossby wave omega ~ -beta k / (k^2 + Ld^{-2}).");
  ImGui::Text("Ld = %.4f", Ld);
  ImGui::Separator();
  ImGui::Text("Short wave (k >> 1/Ld): k = %.3f  ->  omega = %.4f  c_p = %.4f", kShort_, wS, cpS);
  ImGui::Text("Long wave (k << 1/Ld): k = %.3f  ->  omega = %.4f  c_p = %.4f", kLong_, wL, cpL);
  ImGui::Separator();
  ImGui::SliderFloat("g", &g_, 1.0f, 15.0f);
  ImGui::SliderFloat("H", &H_, 0.2f, 3.0f);
  ImGui::SliderFloat("f0", &f0_, 0.2f, 3.0f);
  ImGui::SliderFloat("k short", &kShort_, 0.5f, 6.0f);
  ImGui::SliderFloat("k long", &kLong_, 0.05f, 1.5f);
  ImGui::TextUnformatted("Tune k so short-wave limit is dispersive; long-wave limit approaches -beta Ld^2 k.");
  ImGui::Separator();
  ImGui::TextUnformatted("3D: cyan ring ~ Ld (scaled); orange sine = k_short; blue sine = k_long.");
}
