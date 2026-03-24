#include "cases/CouetteFlowCase.hpp"

#include "cases/TinyExprHelpers.hpp"
#include "render/ArrowRenderer.hpp"
#include "render/ColorMap.hpp"

#include "imgui.h"
#include "raylib.h"
#include "tinyexpr.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>

CouetteFlowCase::CouetteFlowCase() = default;

CouetteFlowCase::~CouetteFlowCase() { teutil::freeExpr(teU_); }

void CouetteFlowCase::tryCompileCustom() {
  teH_ = h_;
  teUcap_ = U_;
  te_variable vars[] = {
      {"z", &teZ_, TE_VARIABLE, nullptr},
      {"h", &teH_, TE_VARIABLE, nullptr},
      {"U", &teUcap_, TE_VARIABLE, nullptr},
  };
  void* tmp = nullptr;
  int col = 0;
  if (!teutil::compileOne(exprU_, vars, 3, &tmp, &col)) {
    std::snprintf(exprStatus_, sizeof exprStatus_, "u: error near column %d.", col);
    teutil::freeExpr(tmp);
    return;
  }
  teutil::freeExpr(teU_);
  teU_ = tmp;
  std::strncpy(exprStatus_, "Custom u(z) OK.", sizeof(exprStatus_) - 1);
  exprStatus_[sizeof(exprStatus_) - 1] = '\0';
}

void CouetteFlowCase::update(float /*dt*/) {}

void CouetteFlowCase::draw3D() {
  float hw = plateW_ * 0.5f;
  Color plateCol = {90, 90, 110, 255};
  DrawCube({0, 0.02f, 0}, plateW_, 0.04f, plateD_, plateCol);
  DrawCube({0, h_ + 0.02f, 0}, plateW_, 0.04f, plateD_, {140, 100, 100, 255});

  int nz = 10;
  int nx = 8;
  teH_ = h_;
  teUcap_ = U_;
  for (int j = 0; j <= nz; ++j) {
    float z = h_ * static_cast<float>(j) / static_cast<float>(nz);
    float u = U_ * z / std::max(1e-4f, h_);
    if (fieldMode_ == 1 && teU_ != nullptr) {
      teZ_ = z;
      double ev = te_eval(static_cast<te_expr*>(teU_));
      u = std::isfinite(ev) ? static_cast<float>(ev) : 0.0f;
    }
    for (int i = 0; i <= nx; ++i) {
      float x = -hw + (2.0f * hw) * static_cast<float>(i) / static_cast<float>(nx);
      Vector3 base{x, z, 0.0f};
      Vector3 dir{1.0f, 0.0f, 0.0f};
      Color c = ColorFromScalar(u, 0.0f, std::max(1e-4f, std::max(U_, std::abs(u))));
      DrawArrow3D(base, dir, 0.15f + 0.5f * std::abs(u), 0.06f, 0.02f, c);
    }
  }
}

void CouetteFlowCase::drawUI() {
  ImGui::TextUnformatted("u_x(z) = U z/h, no-slip both walls; Q = integral_0^h u dz = U h / 2.");
  const char* modes[] = {"Built-in linear", "Custom u(z)"};
  if (ImGui::Combo("field mode", &fieldMode_, modes, 2)) {
    if (fieldMode_ == 1) {
      exprHash_ = 0;
    }
  }

  ImGui::SliderFloat("gap h", &h_, 0.3f, 2.5f);
  ImGui::SliderFloat("top plate U", &U_, 0.1f, 2.0f);

  if (fieldMode_ == 1) {
    ImGui::TextWrapped("Variables: z, h, U.");
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

  ImGui::SliderFloat("plate width (viz)", &plateW_, 1.0f, 6.0f);
  ImGui::SliderFloat("plate depth (viz)", &plateD_, 1.0f, 6.0f);
  float Q = 0.5f * U_ * h_;
  ImGui::Text("flux / width Q = %.4f (built-in reference)", Q);
}
