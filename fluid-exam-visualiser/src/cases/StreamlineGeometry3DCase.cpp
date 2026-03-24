#include "cases/StreamlineGeometry3DCase.hpp"

#include "math/Streamline.hpp"
#include "render/ArrowRenderer.hpp"
#include "render/ColorMap.hpp"

#include "imgui.h"
#include "raylib.h"

#include <cmath>

// u = (z, a, -x); sign of a changes streamline topology (exam classic).

void StreamlineGeometry3DCase::rebuild() {
  field_.resize(gridN_, gridN_, gridN_, -box_, box_, -box_, box_, -box_, box_);
  field_.fillAnalytic(AnalyticFieldId::Helical, paramA_);
  lines_.clear();
  StreamlineParams p;
  p.stepSize = slStep_;
  p.maxSteps = slMax_;
  float lo = -box_ * 0.8f;
  float hi = box_ * 0.8f;
  int n = std::max(2, seeds_);
  for (int k = 0; k < n; ++k) {
    for (int j = 0; j < n; ++j) {
      for (int i = 0; i < n; ++i) {
        float fx = lo + (hi - lo) * static_cast<float>(i) / static_cast<float>(n - 1);
        float fy = lo + (hi - lo) * static_cast<float>(j) / static_cast<float>(n - 1);
        float fz = lo + (hi - lo) * static_cast<float>(k) / static_cast<float>(n - 1);
        auto ln = traceStreamline(field_, {fx, fy, fz}, p);
        if (ln.size() > 2) {
          lines_.push_back(std::move(ln));
        }
      }
    }
  }
  dirty_ = false;
}

void StreamlineGeometry3DCase::update(float /*dt*/) {
  if (dirty_) {
    rebuild();
  }
}

void StreamlineGeometry3DCase::draw3D() {
  for (int k = 0; k < field_.nz; k += 2) {
    for (int j = 0; j < field_.ny; j += 2) {
      for (int i = 0; i < field_.nx; i += 2) {
        Vector3 p = field_.nodePosition(i, j, k);
        Vector3 v = field_.at(i, j, k);
        float vm = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        if (vm < 1e-5f) {
          continue;
        }
        Color c = ColorFromScalar(v.y, -2.0f, 2.0f);
        DrawArrow3D(p, v, 0.22f * vm, 0.06f, 0.02f, c);
      }
    }
  }
  for (const auto& ln : lines_) {
    for (size_t t = 1; t < ln.size(); ++t) {
      DrawLine3D(ln[t - 1], ln[t], {255, 220, 180, 220});
    }
  }
}

void StreamlineGeometry3DCase::drawUI() {
  ImGui::TextUnformatted("u = z * i + a * j - x * k; vary a to change streamline geometry.");
  if (ImGui::SliderFloat("a", &paramA_, -2.0f, 2.0f)) {
    dirty_ = true;
  }
  if (ImGui::SliderInt("grid (arrows)", &gridN_, 6, 16)) {
    dirty_ = true;
  }
  if (ImGui::SliderFloat("half-box", &box_, 1.0f, 3.0f)) {
    dirty_ = true;
  }
  if (ImGui::SliderFloat("RK step", &slStep_, 0.03f, 0.12f)) {
    dirty_ = true;
  }
  if (ImGui::SliderInt("RK max steps", &slMax_, 80, 400)) {
    dirty_ = true;
  }
  if (ImGui::SliderInt("seeds / axis", &seeds_, 2, 7)) {
    dirty_ = true;
  }
}
