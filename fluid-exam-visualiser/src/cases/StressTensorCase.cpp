#include "cases/StressTensorCase.hpp"

#include "render/ArrowRenderer.hpp"

#include "imgui.h"
#include "raylib.h"
#include "raymath.h"

// Simple shear u = S y. Newtonian deviatoric stress sigma' = 2 mu e, e_xy = e_yx = S/2.

static Vector3 tractionPressure(Vector3 n, float p) {
  return {-p * n.x, -p * n.y, -p * n.z};
}

static Vector3 tractionDeviatoric(Vector3 n, float mu, float S) {
  float txy = mu * S;
  float sx = txy * n.y;
  float sy = txy * n.x;
  float sz = 0.0f;
  return {sx, sy, sz};
}

void StressTensorCase::update(float /*dt*/) {}

void StressTensorCase::draw3D() {
  Vector3 c{0.0f, 0.45f, 0.0f};
  float h = cube_ * 0.5f;
  DrawCubeWires(c, cube_, cube_, cube_, {200, 200, 220, 255});

  auto face = [&](int faceIdx, Vector3* center, Vector3* nOut) {
    *center = c;
    *nOut = {0, 0, 0};
    switch (faceIdx) {
      case 0:
        nOut->x = 1;
        center->x += h;
        break;
      case 1:
        nOut->x = -1;
        center->x -= h;
        break;
      case 2:
        nOut->y = 1;
        center->y += h;
        break;
      case 3:
        nOut->y = -1;
        center->y -= h;
        break;
      case 4:
        nOut->z = 1;
        center->z += h;
        break;
      default:
        nOut->z = -1;
        center->z -= h;
        break;
    }
  };

  for (int f = 0; f < 6; ++f) {
    Vector3 fc, n;
    face(f, &fc, &n);
    Vector3 t{0, 0, 0};
    if (mode_ == 0) {
      t = tractionPressure(n, p_);
    } else if (mode_ == 1) {
      t = tractionDeviatoric(n, mu_, shearRate_);
    } else {
      Vector3 tp = tractionPressure(n, p_);
      Vector3 tv = tractionDeviatoric(n, mu_, shearRate_);
      t = {tp.x + tv.x, tp.y + tv.y, tp.z + tv.z};
    }
    float mag = Vector3Length(t);
    if (mag < 1e-5f) {
      continue;
    }
    Vector3 start = {fc.x + n.x * h * 0.08f, fc.y + n.y * h * 0.08f, fc.z + n.z * h * 0.08f};
    Color col = (mode_ == 1) ? Color{255, 200, 120, 255}
                             : (mode_ == 2 ? Color{160, 255, 160, 255} : Color{140, 190, 255, 255});
    DrawArrow3D(start, t, mag * 0.5f, 0.06f, 0.02f, col);
  }
}

void StressTensorCase::drawUI() {
  ImGui::TextUnformatted("Shear u = S y. Traction t = sigma n, sigma = -p I + 2 mu e (incompressible).");
  const char* items[] = {"pressure -p I only", "deviatoric 2mu e only", "full Cauchy stress"};
  ImGui::Combo("mode", &mode_, items, 3);
  ImGui::SliderFloat("pressure p", &p_, 0.2f, 2.0f);
  ImGui::SliderFloat("shear rate S (du/dy)", &shearRate_, 0.0f, 1.5f);
  ImGui::SliderFloat("viscosity mu", &mu_, 0.1f, 1.0f);
  ImGui::SliderFloat("cube size", &cube_, 0.2f, 0.55f);
  ImGui::TextUnformatted("MTH3007: traction with NS / BL. MTH3001: (n/a).");
}
