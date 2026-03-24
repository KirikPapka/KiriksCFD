#include "cases/VorticityEquationCase.hpp"

#include "render/ArrowRenderer.hpp"

#include "imgui.h"
#include "raylib.h"

#include <algorithm>
#include <cmath>

// D omega/Dt = (omega dot nabla) u + nu nabla^2 omega; 2D exam form often D zeta/Dt = ... without tilting.

void VorticityEquationCase::update(float /*dt*/) {}

void VorticityEquationCase::draw3D() {
  const Vector3 shearBase{-0.55f, 0.0f, 0.0f};
  const Vector3 omegaBase{-0.45f, 0.28f, 0.0f};
  const float shearShaft = std::clamp(0.25f + 0.55f * shear_, 0.2f, 1.35f);
  DrawArrow3D(shearBase, {shear_, 0.0f, 0.0f}, shearShaft, 0.09f, 0.032f, {140, 170, 255, 255});

  float omegaLen =
      0.22f + (showStretch_ ? 0.55f * std::abs(shear_) : 0.12f);
  omegaLen = std::clamp(omegaLen, 0.18f, 1.2f);
  Color omegaCol = showStretch_ ? Color{255, 190, 100, 255} : Color{255, 190, 100, 120};
  DrawArrow3D(omegaBase, {0.0f, 1.0f, 0.0f}, omegaLen, 0.08f, 0.028f, omegaCol);

  if (showDiff_) {
    float diffLen = std::clamp(0.12f + 0.95f * nu_, 0.1f, 0.95f);
    Vector3 diffBase{0.42f, 0.28f, 0.0f};
    DrawArrow3D(diffBase, {0.0f, -1.0f, 0.0f}, diffLen, 0.07f, 0.024f, {120, 230, 160, 230});
  }
  if (showAdv_) {
    float advLen = std::clamp(0.18f + 0.45f * shear_, 0.15f, 1.0f);
    DrawArrow3D(omegaBase, {1.0f, 0.0f, 0.0f}, advLen, 0.07f, 0.024f, {220, 140, 255, 240});
  }
}

void VorticityEquationCase::drawUI() {
  ImGui::TextUnformatted("Qualitative term toggles for vector vorticity equation (exam form).");
  ImGui::BulletText("(omega dot nabla) u  - vortex stretching / tilting");
  ImGui::BulletText("nu nabla^2 omega  - viscous diffusion");
  ImGui::BulletText("Du/Dt cross terms  - advection of vorticity (vector form)");
  ImGui::Separator();
  ImGui::Checkbox("discuss stretching / tilting", &showStretch_);
  ImGui::Checkbox("discuss diffusion", &showDiff_);
  ImGui::Checkbox("discuss advection", &showAdv_);
  ImGui::SliderFloat("nu (scale)", &nu_, 0.0f, 1.0f);
  ImGui::SliderFloat("simple shear |du/dy|", &shear_, 0.2f, 2.0f);
  ImGui::TextUnformatted("Arrows: blue = shear flow; orange = omega (stretch on); green = diffusion; purple = advection.");
  float stretchScale = showStretch_ ? std::abs(shear_) : 0.0f;
  float diffScale = showDiff_ ? nu_ : 0.0f;
  ImGui::Text("illustrative relative weights (not a simulation): |stretch| ~ %.2f  |diff| ~ %.2f",
              stretchScale, diffScale);
  if (showAdv_) {
    ImGui::TextUnformatted("Advection redistributes omega without creating it (barotropic ideal).");
  }
}
