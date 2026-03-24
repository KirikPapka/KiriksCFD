#include "cases/ThermalWindCase.hpp"

#include "cases/TinyExprHelpers.hpp"

#include <algorithm>
#include "render/ArrowRenderer.hpp"
#include "render/ColorMap.hpp"

#include "imgui.h"
#include "raylib.h"
#include "tinyexpr.h"

#include <cmath>
#include <cstdio>
#include <cstring>

ThermalWindCase::ThermalWindCase() = default;

ThermalWindCase::~ThermalWindCase() { teutil::freeExpr(teU_); }

void ThermalWindCase::tryCompileCustom() {
  float coef = (std::abs(f_) > 1e-12f) ? -(9.81f / (f_ * std::max(10.0f, T0_))) * dTdy_ : 0.0f;
  teK_ = static_cast<double>(scale_) * static_cast<double>(coef);
  te_variable vars[] = {
      {"z", &teZ_, TE_VARIABLE, nullptr},
      {"K", &teK_, TE_VARIABLE, nullptr},
  };
  void* tmp = nullptr;
  int col = 0;
  if (!teutil::compileOne(exprU_, vars, 2, &tmp, &col)) {
    std::snprintf(exprStatus_, sizeof exprStatus_, "u: error near column %d.", col);
    teutil::freeExpr(tmp);
    return;
  }
  teutil::freeExpr(teU_);
  teU_ = tmp;
  std::strncpy(exprStatus_, "Custom u(z) OK.", sizeof(exprStatus_) - 1);
  exprStatus_[sizeof(exprStatus_) - 1] = '\0';
}

void ThermalWindCase::update(float /*dt*/) {}

void ThermalWindCase::draw3D() {
  float coef = (std::abs(f_) > 1e-12f) ? -(9.81f / (f_ * std::max(10.0f, T0_))) * dTdy_ : 0.0f;
  teK_ = static_cast<double>(scale_) * static_cast<double>(coef);
  float zn = static_cast<float>(std::max(1, nZ_ - 1));
  for (int k = 0; k < nZ_; ++k) {
    float z = static_cast<float>(k) / zn * zMax_;
    float u = scale_ * coef * z;
    if (fieldMode_ == 1 && teU_ != nullptr) {
      teZ_ = z;
      double ev = te_eval(static_cast<te_expr*>(teU_));
      u = std::isfinite(ev) ? static_cast<float>(ev) : 0.0f;
    }
    for (int i = -2; i <= 2; ++i) {
      Vector3 base{static_cast<float>(i) * 0.8f, z, 0.0f};
      Vector3 dir{1.0f, 0.0f, 0.0f};
      float mag = std::abs(u);
      Color c = ColorFromScalar(u, -2.0f, 2.0f);
      DrawArrow3D(base, dir, 0.2f + 0.35f * mag, 0.06f, 0.025f, c);
    }
  }
  DrawLine3D({0, 0, 0}, {0, zMax_, 0}, {150, 150, 150, 200});
}

void ThermalWindCase::drawUI() {
  float coef = (std::abs(f_) > 1e-12f) ? -(9.81f / (f_ * std::max(10.0f, T0_))) * dTdy_ : 0.0f;
  ImGui::TextUnformatted("Shear du/dz = - (g/(f T0)) dT/dy (signs: exam conventions vary).");
  ImGui::Text("du/dz (scaled) coef = %.3e; K = scale*coef used in custom u(z).", coef);
  const char* modes[] = {"Built-in K*z", "Custom u(z)"};
  if (ImGui::Combo("field mode", &fieldMode_, modes, 2)) {
    if (fieldMode_ == 1) {
      exprHash_ = 0;
    }
  }

  ImGui::SliderFloat("f (Coriolis)", &f_, 1e-5f, 5e-4f, "%.5f", ImGuiSliderFlags_Logarithmic);
  ImGui::SliderFloat("dT/dy (north)", &dTdy_, -8.0f, 8.0f);
  ImGui::SliderFloat("T0 (K)", &T0_, 200.0f, 310.0f);
  ImGui::SliderFloat("arrow scale", &scale_, 1.0f, 20.0f);

  if (fieldMode_ == 1) {
    ImGui::TextWrapped("Variables: z, K (K = arrow_scale * du/dz coefficient from sliders above).");
    ImGui::InputTextMultiline("u(z)", exprU_, sizeof(exprU_), ImVec2(-1, 36));
    uint64_t h = teutil::fnv1a64(exprU_);
    if (h != exprHash_) {
      exprHash_ = h;
      tryCompileCustom();
    }
    if (exprStatus_[0] != '\0') {
      ImGui::TextUnformatted(exprStatus_);
    }
  }

  ImGui::SliderFloat("z max", &zMax_, 1.0f, 8.0f);
  ImGui::SliderInt("z levels", &nZ_, 6, 32);
}
