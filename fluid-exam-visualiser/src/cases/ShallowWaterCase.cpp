#include "cases/ShallowWaterCase.hpp"

#include "cases/TinyExprHelpers.hpp"
#include "render/ArrowRenderer.hpp"
#include "render/ColorMap.hpp"
#include "render/SurfaceRenderer.hpp"

#include "imgui.h"
#include "raylib.h"
#include "tinyexpr.h"

#include <cmath>
#include <cstdio>
#include <cstring>

ShallowWaterCase::ShallowWaterCase() = default;

ShallowWaterCase::~ShallowWaterCase() { releaseTe(); }

void ShallowWaterCase::releaseTe() { teutil::freeExpr(teEta_); }

void ShallowWaterCase::rebuildHeightsBuiltin() {
  heights_.assign(static_cast<size_t>(gridN_ * gridN_), 0.0f);
  float k = 2.0f * PI / std::max(0.25f, wavelength_);
  float x0 = -domainHalf_;
  float x1 = domainHalf_;
  float y0 = -domainHalf_;
  float y1 = domainHalf_;
  float dx = (x1 - x0) / static_cast<float>(gridN_ - 1);
  float dy = (y1 - y0) / static_cast<float>(gridN_ - 1);
  for (int j = 0; j < gridN_; ++j) {
    for (int i = 0; i < gridN_; ++i) {
      float x = x0 + static_cast<float>(i) * dx;
      float y = y0 + static_cast<float>(j) * dy;
      float phase = k * (x - phaseSpeed_ * time_);
      heights_[static_cast<size_t>(j * gridN_ + i)] =
          amplitude_ * std::sin(phase) * std::cos(0.35f * k * y);
    }
  }
}

void ShallowWaterCase::rebuildHeightsCustom() {
  heights_.assign(static_cast<size_t>(gridN_ * gridN_), 0.0f);
  if (teEta_ == nullptr) {
    return;
  }
  float x0 = -domainHalf_;
  float x1 = domainHalf_;
  float y0 = -domainHalf_;
  float y1 = domainHalf_;
  float dx = (x1 - x0) / static_cast<float>(gridN_ - 1);
  float dy = (y1 - y0) / static_cast<float>(gridN_ - 1);
  teA_ = amplitude_;
  teK_ = 2.0 * PI / std::max(0.25, static_cast<double>(wavelength_));
  teC_ = phaseSpeed_;
  for (int j = 0; j < gridN_; ++j) {
    for (int i = 0; i < gridN_; ++i) {
      teX_ = x0 + static_cast<double>(i) * dx;
      teY_ = y0 + static_cast<double>(j) * dy;
      teT_ = time_;
      double v = te_eval(static_cast<te_expr*>(teEta_));
      if (!std::isfinite(v)) {
        v = 0.0;
      }
      heights_[static_cast<size_t>(j * gridN_ + i)] = static_cast<float>(v);
    }
  }
}

void ShallowWaterCase::tryCompileCustom() {
  teA_ = amplitude_;
  teK_ = 2.0 * PI / std::max(0.25, static_cast<double>(wavelength_));
  teC_ = phaseSpeed_;
  te_variable vars[] = {
      {"x", &teX_, TE_VARIABLE, nullptr}, {"y", &teY_, TE_VARIABLE, nullptr},
      {"t", &teT_, TE_VARIABLE, nullptr}, {"A", &teA_, TE_VARIABLE, nullptr},
      {"k", &teK_, TE_VARIABLE, nullptr}, {"c", &teC_, TE_VARIABLE, nullptr},
  };
  void* tmp = nullptr;
  int col = 0;
  const bool had = (teEta_ != nullptr);
  if (!teutil::compileOne(exprEta_, vars, 6, &tmp, &col)) {
    char err[160];
    std::snprintf(err, sizeof err, "eta: error near column %d.", col);
    std::strncpy(exprStatus_, err, sizeof(exprStatus_) - 1);
    exprStatus_[sizeof(exprStatus_) - 1] = '\0';
    teutil::freeExpr(tmp);
    if (!had) {
      releaseTe();
    }
    return;
  }
  releaseTe();
  teEta_ = tmp;
  std::strncpy(exprStatus_, "Custom eta OK.", sizeof(exprStatus_) - 1);
  exprStatus_[sizeof(exprStatus_) - 1] = '\0';
}

void ShallowWaterCase::update(float dt) {
  time_ += dt * timeScale_;
  if (fieldMode_ == 0) {
    rebuildHeightsBuiltin();
  } else {
    rebuildHeightsCustom();
  }
}

void ShallowWaterCase::draw3D() {
  float x0 = -domainHalf_;
  float x1 = domainHalf_;
  float y0 = -domainHalf_;
  float y1 = domainHalf_;
  DrawHeightFieldSurface(gridN_, gridN_, heights_.data(), x0, x1, y0, y1, 1.0f,
                         {180, 220, 255, 255});

  float k = 2.0f * PI / std::max(0.25f, wavelength_);
  float dx = (x1 - x0) / static_cast<float>(gridN_ - 1);
  float dy = (y1 - y0) / static_cast<float>(gridN_ - 1);
  int st = std::max(1, arrowStride_);
  for (int j = 0; j < gridN_; j += st) {
    for (int i = 0; i < gridN_; i += st) {
      float x = x0 + static_cast<float>(i) * dx;
      float y = y0 + static_cast<float>(j) * dy;
      float h = heights_[static_cast<size_t>(j * gridN_ + i)];
      Vector3 base{x, h, y};
      float ux = 0.0f;
      float uy = 0.0f;
      if (fieldMode_ == 0) {
        float phase = k * (x - phaseSpeed_ * time_);
        ux = velScale_ * std::cos(phase);
        uy = velScale_ * 0.35f * std::sin(0.35f * k * y) * std::cos(phase);
      } else {
        if (i > 0 && i < gridN_ - 1 && j > 0 && j < gridN_ - 1) {
          float hx = (heights_[static_cast<size_t>(j * gridN_ + i + 1)] -
                      heights_[static_cast<size_t>(j * gridN_ + i - 1)]) /
                     (2.0f * dx);
          float hy = (heights_[static_cast<size_t>((j + 1) * gridN_ + i)] -
                      heights_[static_cast<size_t>((j - 1) * gridN_ + i)]) /
                     (2.0f * dy);
          ux = velScale_ * hx;
          uy = velScale_ * hy;
        }
      }
      Vector3 dir{ux, 0.0f, uy};
      float mag = std::sqrt(ux * ux + uy * uy);
      Color col = ColorFromScalar(mag, 0.0f, velScale_ * 1.2f);
      if (mag > 1e-4f) {
        DrawArrow3D(base, dir, 0.45f * mag, 0.06f, 0.025f, col);
      }
    }
  }
}

void ShallowWaterCase::drawUI() {
  ImGui::TextUnformatted("eta(x,y,t) on the surface; arrows sketch horizontal flow.");
  const char* modes[] = {"Built-in travelling wave", "Custom eta(x,y,t)"};
  if (ImGui::Combo("field mode", &fieldMode_, modes, 2)) {
    if (fieldMode_ == 1) {
      exprHash_ = 0;
    }
  }

  if (fieldMode_ == 0) {
    ImGui::SliderFloat("amplitude A", &amplitude_, 0.05f, 1.2f);
    ImGui::SliderFloat("wavelength", &wavelength_, 0.5f, 6.0f);
    ImGui::SliderFloat("phase speed c", &phaseSpeed_, 0.0f, 3.0f);
  } else {
    ImGui::TextWrapped("Variables: x, y, t, A, k, c (k = 2*pi/wavelength from slider below).");
    ImGui::InputTextMultiline("eta", exprEta_, sizeof(exprEta_), ImVec2(-1, 44));
    ImGui::SliderFloat("amplitude A", &amplitude_, 0.05f, 1.2f);
    ImGui::SliderFloat("wavelength", &wavelength_, 0.5f, 6.0f);
    ImGui::SliderFloat("phase speed c", &phaseSpeed_, 0.0f, 3.0f);
    uint64_t h = teutil::fnv1a64(exprEta_);
    if (h != exprHash_) {
      exprHash_ = h;
      tryCompileCustom();
    }
    if (exprStatus_[0] != '\0') {
      ImGui::TextUnformatted(exprStatus_);
    }
  }

  ImGui::SliderFloat("time scale", &timeScale_, 0.0f, 2.5f);
  ImGui::SliderFloat("domain half-width", &domainHalf_, 1.0f, 5.0f);
  ImGui::SliderInt("grid N", &gridN_, 12, 64);
  ImGui::SliderFloat("arrow |u| scale", &velScale_, 0.05f, 1.5f);
  ImGui::SliderInt("arrow stride", &arrowStride_, 1, 8);
}
