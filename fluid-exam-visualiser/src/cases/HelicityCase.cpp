#include "cases/HelicityCase.hpp"

#include "render/ArrowRenderer.hpp"
#include "render/ColorMap.hpp"

#include "imgui.h"
#include "raylib.h"

#include <algorithm>
#include <cmath>

// h = u dot omega; coloured arrows for u, optional smaller omega arrows.

void HelicityCase::rebuild() {
  u_.resize(gridN_, gridN_, gridN_, -box_, box_, -box_, box_, -box_, box_);
  AnalyticFieldId id = AnalyticFieldId::Shear2D;
  if (preset_ == 1) {
    id = AnalyticFieldId::Helical;
  } else if (preset_ == 2) {
    id = AnalyticFieldId::SinCos2D;
  }
  u_.fillAnalytic(id, paramA_);
  computeCurl(u_, w_);
  hel_.nx = u_.nx;
  hel_.ny = u_.ny;
  hel_.nz = u_.nz;
  hel_.data.assign(static_cast<size_t>(u_.nx * u_.ny * u_.nz), 0.0f);
  for (int k = 0; k < u_.nz; ++k) {
    for (int j = 0; j < u_.ny; ++j) {
      for (int i = 0; i < u_.nx; ++i) {
        const Vector3& a = u_.at(i, j, k);
        const Vector3& b = w_.at(i, j, k);
        hel_.at(i, j, k) = a.x * b.x + a.y * b.y + a.z * b.z;
      }
    }
  }
  dirty_ = false;
}

void HelicityCase::update(float /*dt*/) {
  if (dirty_) {
    rebuild();
  }
}

void HelicityCase::draw3D() {
  float hmin = hel_.at(0, 0, 0), hmax = hmin;
  for (int k = 0; k < u_.nz; ++k) {
    for (int j = 0; j < u_.ny; ++j) {
      for (int i = 0; i < u_.nx; ++i) {
        float h = hel_.at(i, j, k);
        hmin = std::min(hmin, h);
        hmax = std::max(hmax, h);
      }
    }
  }
  if (hmax <= hmin) {
    hmax = hmin + 1.0f;
  }
  for (int k = 0; k < u_.nz; ++k) {
    for (int j = 0; j < u_.ny; ++j) {
      for (int i = 0; i < u_.nx; ++i) {
        Vector3 p = u_.nodePosition(i, j, k);
        Vector3 v = u_.at(i, j, k);
        float vm = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        if (vm < 1e-5f) {
          continue;
        }
        float h = hel_.at(i, j, k);
        Color c = ColorFromScalar(h, hmin, hmax);
        DrawArrow3D(p, v, 0.25f * vm, 0.07f, 0.02f, c);
        if (showOmega_) {
          Vector3 om = w_.at(i, j, k);
          float omag = std::sqrt(om.x * om.x + om.y * om.y + om.z * om.z);
          if (omag > 1e-4f) {
            DrawArrow3D({p.x + 0.05f, p.y + 0.05f, p.z}, om, 0.12f * omag, 0.04f, 0.015f,
                        {200, 200, 255, 180});
          }
        }
      }
    }
  }
}

void HelicityCase::drawUI() {
  ImGui::TextUnformatted("h = u dot omega; helicity conserved in ideal barotropic flow (exam context).");
  if (ImGui::SliderInt("grid N", &gridN_, 6, 16)) {
    dirty_ = true;
  }
  if (ImGui::SliderFloat("half-box", &box_, 0.8f, 2.5f)) {
    dirty_ = true;
  }
  const char* items[] = {"(y,x,0) shear", "(z,a,-x) helical", "(sin y, cos x, 0)"};
  if (ImGui::Combo("field", &preset_, items, 3)) {
    dirty_ = true;
  }
  if (preset_ == 1) {
    if (ImGui::SliderFloat("parameter a", &paramA_, -2.0f, 2.0f)) {
      dirty_ = true;
    }
  }
  ImGui::Checkbox("show omega arrows (cyan)", &showOmega_);
}
