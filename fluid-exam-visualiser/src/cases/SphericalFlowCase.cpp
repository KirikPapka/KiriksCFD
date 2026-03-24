#include "cases/SphericalFlowCase.hpp"

#include <algorithm>

#include "cases/FlowCommon.hpp"
#include "render/ArrowRenderer.hpp"
#include "render/ColorMap.hpp"

#include "imgui.h"
#include "raylib.h"

#include <cmath>

// u = (sin theta / r) theta_hat (polar axis +z); incompressible; check div, curl, advection in UI.

void SphericalFlowCase::update(float /*dt*/) {}

void SphericalFlowCase::draw3D() {
  for (int it = 1; it < nTheta_; ++it) {
    float t = PI * static_cast<float>(it) / static_cast<float>(nTheta_);
    for (int ip = 0; ip < nPhi_; ++ip) {
      float p = 2.0f * PI * static_cast<float>(ip) / static_cast<float>(nPhi_);
      float x, y, z;
      CartesianFromSpherical(r0_, t, p, &x, &y, &z);
      Vector3 eTh = SphericalThetaHat(t, p);
      float umag = std::sin(t) / std::max(1e-4f, r0_);
      Vector3 base{x, y, z};
      Color c = ColorFromScalar(umag, 0.0f, 1.0f / std::max(0.2f, r0_));
      DrawArrow3D(base, eTh, 0.35f * umag, 0.05f, 0.02f, c);
    }
  }
  DrawSphereWires({0, 0, 0}, r0_, 12, 16, {80, 80, 120, 160});
}

void SphericalFlowCase::drawUI() {
  ImGui::TextUnformatted("u = (sin theta / r) theta_hat; exam checks: div u = 0, curl u != 0.");
  ImGui::BulletText("Material derivative (u dot nabla)u has both r and theta components.");
  ImGui::SliderFloat("radius r", &r0_, 0.4f, 2.5f);
  ImGui::SliderInt("theta samples", &nTheta_, 4, 20);
  ImGui::SliderInt("phi samples", &nPhi_, 8, 32);
  ImGui::Text("analytic div u = 0; curl magnitude scales as 1/r^2 (see notes).");
}
