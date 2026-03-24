#include "cases/RotatingForceBalanceCase.hpp"

#include <algorithm>

#include "render/ArrowRenderer.hpp"

#include "imgui.h"
#include "raylib.h"

#include <cmath>

// Cartoon magnitudes: |advective| ~ U^2/L ~ Ro |Coriolis|; |viscous| ~ nu U/L^2 ~ 1/Re |inertial|.

void RotatingForceBalanceCase::update(float /*dt*/) {}

void RotatingForceBalanceCase::draw3D() {
  float Ro = std::max(0.02f, Ro_);
  float Re = std::max(1.0f, Re_);
  float cor = 1.0f;
  float adv = Ro;
  float visc = 1.0f / Re;
  float pres = 1.0f;
  Vector3 o{0, 0.3f, 0};
  DrawArrow3D(o, {1, 0, 0}, 0.5f * pres, 0.07f, 0.025f, {255, 120, 120, 255});
  DrawArrow3D(o, {0, 0, 1}, 0.5f * cor, 0.07f, 0.025f, {120, 200, 255, 255});
  DrawArrow3D(o, {-1, 0, 0}, 0.5f * adv, 0.07f, 0.025f, {200, 255, 120, 255});
  DrawArrow3D(o, {0, 0, -1}, 0.5f * visc, 0.07f, 0.025f, {220, 180, 255, 255});
}

void RotatingForceBalanceCase::drawUI() {
  ImGui::TextUnformatted("Relative arrow lengths (schematic): pressure gradient, Coriolis, inertia, viscosity.");
  ImGui::SliderFloat("Rossby number Ro", &Ro_, 0.02f, 2.0f);
  ImGui::SliderFloat("Reynolds number Re", &Re_, 1.0f, 500.0f);
  ImGui::Text("Ro small => rotation dominates; Re large => viscous terms subdominant in bulk.");
}
