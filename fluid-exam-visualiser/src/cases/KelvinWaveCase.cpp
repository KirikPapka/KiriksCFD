#include "cases/KelvinWaveCase.hpp"

#include "render/ArrowRenderer.hpp"
#include "render/ColorMap.hpp"
#include "render/SurfaceRenderer.hpp"

#include "imgui.h"
#include "raylib.h"

#include <cmath>

void KelvinWaveCase::update(float dt) { time_ += dt * timeScale_; }

void KelvinWaveCase::draw3D() {
  float c = std::sqrt(std::max(1e-6f, g_ * H_));
  float Rd = c / std::max(1e-8f, std::abs(f_));
  if (mode_ == 1) {
    Rd *= 3.5f;
  }
  float omega = c * k_;
  float x0 = -xHalf_;
  float x1 = xHalf_;
  float y0 = 0.0f;
  float y1 = yMax_;
  float dx = (x1 - x0) / static_cast<float>(gridN_ - 1);
  float dy = (y1 - y0) / static_cast<float>(gridN_ - 1);
  heights_.resize(static_cast<size_t>(gridN_ * gridN_));
  for (int j = 0; j < gridN_; ++j) {
    for (int i = 0; i < gridN_; ++i) {
      float x = x0 + static_cast<float>(i) * dx;
      float y = y0 + static_cast<float>(j) * dy;
      float phase = k_ * x - omega * time_;
      float env = std::exp(-y / std::max(0.15f, Rd));
      heights_[static_cast<size_t>(j * gridN_ + i)] = A_ * env * std::cos(phase);
    }
  }
  DrawHeightFieldSurface(gridN_, gridN_, heights_.data(), x0, x1, y0, y1, 2.2f, {120, 200, 255, 255});

  DrawLine3D({x0, 0.0f, y0}, {x1, 0.0f, y0}, {255, 200, 120, 255});
  int st = 5;
  for (int j = 0; j < gridN_; j += st) {
    for (int i = 0; i < gridN_; i += st) {
      float x = x0 + static_cast<float>(i) * dx;
      float y = y0 + static_cast<float>(j) * dy;
      float phase = k_ * x - omega * time_;
      float env = std::exp(-y / std::max(0.15f, Rd));
      float eta = A_ * env * std::cos(phase);
      // Leading-order SW Kelvin: along-shore u is O(c eta / H); cross-shore v is smaller (omitted).
      float vx = (c / std::max(0.05f, H_)) * eta;
      float vy = 0.0f;
      float h = 0.05f + eta * 2.2f;
      Vector3 base{x, h, y};
      Vector3 dir{vx, 0.0f, vy};
      float mag = std::sqrt(vx * vx + vy * vy);
      if (mag > 1e-5f) {
        DrawArrow3D(base, dir, 0.25f * mag / (A_ * k_ + 1e-4f), 0.04f, 0.014f,
                    ColorFromScalar(mag, 0.0f, 0.5f));
      }
    }
  }
}

void KelvinWaveCase::drawUI() {
  ImGui::TextUnformatted(
      "Wall at y=0; eta ~ exp(-y/Rd) cos(kx - omega t), omega = c k; u ~(c/H) eta along wall.");
  const char* items[] = {"coastal (Rd = c/|f|)", "wide decay (equatorial-style sketch)"};
  ImGui::Combo("mode", &mode_, items, 2);
  ImGui::SliderFloat("g", &g_, 1.0f, 15.0f);
  ImGui::SliderFloat("H", &H_, 0.1f, 3.0f);
  ImGui::SliderFloat("|f|", &f_, 1e-5f, 5e-4f);
  ImGui::SliderFloat("k", &k_, 0.2f, 2.0f);
  ImGui::SliderFloat("amplitude A", &A_, 0.02f, 0.25f);
  ImGui::SliderFloat("time scale", &timeScale_, 0.0f, 2.0f);
  ImGui::SliderFloat("fetch x half-width", &xHalf_, 1.5f, 5.0f);
  ImGui::SliderFloat("offshore y extent", &yMax_, 1.5f, 6.0f);
  float c = std::sqrt(std::max(1e-6f, g_ * H_));
  float Rd = c / std::max(1e-8f, std::abs(f_));
  if (mode_ == 1) {
    Rd *= 3.5f;
  }
  ImGui::Text("c = sqrt(gH) ~ %.3f  |  Rd (sketch) ~ %.2f km (if m,s units)", c, Rd / 1000.0f);
  ImGui::TextUnformatted("MTH3001: Sec.5.3.3-5.3.4 Kelvin waves. MTH3007: (n/a).");
}
