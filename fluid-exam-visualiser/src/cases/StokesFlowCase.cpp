#include "cases/StokesFlowCase.hpp"

#include "render/ArrowRenderer.hpp"
#include "render/ColorMap.hpp"

#include "imgui.h"
#include "raylib.h"
#include "raymath.h"

#include <algorithm>
#include <cmath>

// Uniform flow +z past sphere at origin; Stokes: theta from +z axis (colatitude).

static Vector3 stokesVelocity(float x, float y, float z, float U, float a) {
  float r = std::sqrt(x * x + y * y + z * z);
  if (r < a * 1.01f) {
    return {0, 0, 0};
  }
  float rp = std::sqrt(x * x + y * y);
  float cth = z / r;
  float sth = rp / r;
  float vr = U * cth * (1.0f - 1.5f * a / r + 0.5f * a * a * a / (r * r * r));
  float vth = -U * sth * (1.0f - 0.75f * a / r - 0.25f * a * a * a / (r * r * r));
  Vector3 er{x / r, y / r, z / r};
  Vector3 et{0.0f, 0.0f, 0.0f};
  if (rp > 1e-4f) {
    et.x = (z * x) / (r * rp);
    et.y = (z * y) / (r * rp);
    et.z = -rp / r;
  } else {
    et.z = -1.0f;
  }
  return {vr * er.x + vth * et.x, vr * er.y + vth * et.y, vr * er.z + vth * et.z};
}

// Inviscid potential flow, uniform U in +z past sphere radius a: phi = U z (1 + a^3/(2 r^3)).
static Vector3 potentialSphere(float x, float y, float z, float U, float a) {
  float r = std::sqrt(x * x + y * y + z * z);
  if (r < a * 1.01f) {
    return {0, 0, 0};
  }
  float r3 = r * r * r;
  float fac = 0.5f * a * a * a / r3;
  float rr = r * r;
  float uz = U * (1.0f + fac - 3.0f * fac * z * z / rr);
  float ux = U * (-3.0f * fac * x * z / rr);
  float uy = U * (-3.0f * fac * y * z / rr);
  return {ux, uy, uz};
}

void StokesFlowCase::update(float /*dt*/) {}

void StokesFlowCase::draw3D() {
  DrawSphereWires({0.0f, 0.35f, 0.0f}, a_, 12, 12, {200, 200, 220, 200});
  float Re = 2.0f * a_ * U_ / std::max(1e-6f, nu_);
  int grid = 7;
  float L = 1.8f;
  for (int j = 0; j < grid; ++j) {
    for (int i = 0; i < grid; ++i) {
      float sx = -L + (2.0f * L) * static_cast<float>(i) / static_cast<float>(grid - 1);
      float sz = -L + (2.0f * L) * static_cast<float>(j) / static_cast<float>(grid - 1);
      float py = 0.35f;
      Vector3 v = stokesVelocity(sx, py, sz, U_, a_);
      float mag = Vector3Length(v);
      if (mag > 1e-4f) {
        Color c = ColorFromScalar(mag, 0.0f, 1.2f * U_);
        DrawArrow3D({sx, py, sz}, v, 0.22f, 0.045f, 0.015f, c);
      }
    }
  }
  if (showCompare_ && Re > 2.0f) {
    float py = 0.35f + 0.55f;
    for (int j = 0; j < grid; j += 2) {
      for (int i = 0; i < grid; i += 2) {
        float sx = -L + (2.0f * L) * static_cast<float>(i) / static_cast<float>(grid - 1);
        float sz = -L + (2.0f * L) * static_cast<float>(j) / static_cast<float>(grid - 1);
        Vector3 v = potentialSphere(sx, py, sz, U_, a_);
        float mag = Vector3Length(v);
        if (mag > 1e-4f) {
          DrawArrow3D({sx, py, sz}, v, 0.18f, 0.04f, 0.014f, {255, 160, 120, 140});
        }
      }
    }
    DrawLine3D({-L, py, -L}, {L, py, L}, {255, 160, 120, 80});
  }
}

void StokesFlowCase::drawUI() {
  ImGui::TextUnformatted("Stokes solution (+z uniform past sphere at y=0.35); Re = 2 a U / nu.");
  ImGui::SliderFloat("U", &U_, 0.2f, 2.0f);
  ImGui::SliderFloat("sphere radius a", &a_, 0.15f, 0.55f);
  ImGui::SliderFloat("nu", &nu_, 0.05f, 2.0f);
  ImGui::Checkbox("hint inertial (potential) arrows when Re>2", &showCompare_);
  float Re = 2.0f * a_ * U_ / std::max(1e-6f, nu_);
  ImGui::Text("Re ~ %.3f  |  Stokes drag F = 6 pi mu a U (mu = rho nu; rho=1 sketch).", Re);
  ImGui::TextUnformatted("MTH3007: Ch.6 Stokes flow. MTH3001: (n/a).");
}
