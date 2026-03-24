#include "cases/InclinedFilmFlowCase.hpp"

#include "render/ArrowRenderer.hpp"
#include "render/ColorMap.hpp"

#include "imgui.h"
#include "raylib.h"

#include <algorithm>
#include <cmath>

// Nusselt film: u(y) = (rho g sin alpha / mu) (h y - y^2/2), y from wall; tau_free=0 at y=h.
// Q = rho g sin alpha h^3 / (3 mu).

static float uFilm(float y, float h, float rho, float g, float sa, float mu) {
  if (h < 1e-5f || mu < 1e-8f) {
    return 0.0f;
  }
  float coef = rho * g * sa / mu;
  return coef * (h * y - 0.5f * y * y);
}

void InclinedFilmFlowCase::update(float /*dt*/) {}

void InclinedFilmFlowCase::draw3D() {
  float hw = wallW_ * 0.5f;
  Color wallCol = {100, 95, 85, 255};
  DrawCube({0, h_ * 0.5f, -0.15f}, 0.06f, h_, wallW_, wallCol);

  float umax = uFilm(h_, h_, rho_, g_, sinAlpha_, mu_);
  int ny = 14;
  int nx = 6;
  for (int j = 0; j <= ny; ++j) {
    float y = h_ * static_cast<float>(j) / static_cast<float>(ny);
    float u = uFilm(y, h_, rho_, g_, sinAlpha_, mu_);
    for (int i = 0; i <= nx; ++i) {
      float z = -hw + (2.0f * hw) * static_cast<float>(i) / static_cast<float>(nx);
      Vector3 base{0.12f, y, z};
      Vector3 dir{1.0f, 0.0f, 0.0f};
      Color c = ColorFromScalar(u, 0.0f, std::max(1e-4f, umax));
      DrawArrow3D(base, dir, 0.12f + 0.45f * (u / std::max(1e-4f, umax)), 0.05f, 0.02f, c);
    }
  }
  DrawLine3D({0, h_, 0}, {0.8f, h_, 0}, {200, 200, 255, 200});
}

void InclinedFilmFlowCase::drawUI() {
  ImGui::TextUnformatted("u(y) = (rho g sin alpha / mu)(h y - y^2/2), no-slip at wall, zero shear at y=h.");
  ImGui::SliderFloat("film thickness h", &h_, 0.2f, 2.0f);
  ImGui::SliderFloat("sin(alpha)", &sinAlpha_, 0.05f, 1.0f);
  ImGui::SliderFloat("rho", &rho_, 0.5f, 2.0f);
  ImGui::SliderFloat("mu", &mu_, 0.02f, 0.5f);
  ImGui::SliderFloat("g", &g_, 1.0f, 15.0f);
  ImGui::SliderFloat("wall width (viz)", &wallW_, 1.0f, 6.0f);
  float Q = rho_ * g_ * sinAlpha_ * h_ * h_ * h_ / (3.0f * std::max(1e-8f, mu_));
  ImGui::Text("flux / width Q = %.5f", Q);
}
