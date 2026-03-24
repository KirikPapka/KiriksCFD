#include "cases/OceanCirculationCase.hpp"

#include "render/ArrowRenderer.hpp"
#include "render/ColorMap.hpp"

#include "imgui.h"
#include "raylib.h"

#include <algorithm>
#include <cmath>

static float psiGyre(float x, float z, float Lx, float Lz, float wbW, float wbc) {
  float sx = std::sin(PI * x / Lx);
  float sz = std::sin(PI * z / Lz);
  float interior = sx * sz;
  float w = wbW * Lx;
  float intens = wbc * std::exp(-x / std::max(0.05f, w));
  return interior * (1.0f - x / Lx) + intens * sz;
}

void OceanCirculationCase::update(float dt) { time_ += dt * 0.2f; }

void OceanCirculationCase::draw3D() {
  float y = 0.2f;
  int nx = 12;
  int nz = 10;
  for (int j = 0; j < nz; ++j) {
    for (int i = 0; i < nx; ++i) {
      float fx = 0.08f + 0.84f * static_cast<float>(i) / static_cast<float>(nx - 1);
      float fz = 0.08f + 0.84f * static_cast<float>(j) / static_cast<float>(nz - 1);
      float x = fx * Lx_;
      float z = fz * Lz_;
      float dx = 0.04f * Lx_;
      float dz = 0.04f * Lz_;
      float p = psiGyre(x, z, Lx_, Lz_, wbW_, wbcAmp_);
      float pxp = psiGyre(x + dx, z, Lx_, Lz_, wbW_, wbcAmp_);
      float pzm = psiGyre(x, z - dz, Lx_, Lz_, wbW_, wbcAmp_);
      float pzp = psiGyre(x, z + dz, Lx_, Lz_, wbW_, wbcAmp_);
      float u = -(pzp - pzm) / (2.0f * dz);
      float w = (pxp - psiGyre(x - dx, z, Lx_, Lz_, wbW_, wbcAmp_)) / (2.0f * dx);
      Vector3 base{x, y + 0.02f * std::sin(time_ + x + z), z};
      Vector3 dir{u, 0.0f, w};
      float mag = std::sqrt(u * u + w * w);
      if (mag > 1e-4f) {
        DrawArrow3D(base, dir, 0.35f * mag, 0.04f, 0.014f, ColorFromScalar(mag, 0.0f, 0.6f));
      }
    }
  }
  int m = 40;
  for (int i = 0; i < m; ++i) {
    float fz = static_cast<float>(i) / static_cast<float>(m - 1);
    float z = 0.1f * Lz_ + 0.8f * fz * Lz_;
    float x = 0.12f * Lx_;
    float p = psiGyre(x, z, Lx_, Lz_, wbW_, wbcAmp_);
    Color c = ColorFromScalar(p, -0.5f, 0.5f);
    DrawSphere({x, y, z}, 0.05f, c);
  }
}

void OceanCirculationCase::drawUI() {
  ImGui::TextUnformatted("psi ~ sin(pi x/Lx) sin(pi z/Lz) interior + western exponential spike.");
  ImGui::SliderFloat("Lx (basin east-west)", &Lx_, 1.5f, 5.0f);
  ImGui::SliderFloat("Lz (basin north-south)", &Lz_, 1.0f, 4.0f);
  ImGui::SliderFloat("western layer width (frac Lx)", &wbW_, 0.05f, 0.4f);
  ImGui::SliderFloat("WBC strength", &wbcAmp_, 0.0f, 1.2f);
  ImGui::TextUnformatted("MTH3001: Ch.6.1-6.3 ocean circulation. MTH3007: (n/a).");
}
