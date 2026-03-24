#include "cases/DiffusionSimilarityCase.hpp"

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

DiffusionSimilarityCase::DiffusionSimilarityCase() = default;

DiffusionSimilarityCase::~DiffusionSimilarityCase() { teutil::freeExpr(teU_); }

void DiffusionSimilarityCase::tryCompileCustom() {
  teT_ = t_;
  teUcap_ = U_;
  teNu_ = nu_;
  te_variable vars[] = {
      {"z", &teZ_, TE_VARIABLE, nullptr},
      {"t", &teT_, TE_VARIABLE, nullptr},
      {"U", &teUcap_, TE_VARIABLE, nullptr},
      {"nu", &teNu_, TE_VARIABLE, nullptr},
  };
  void* tmp = nullptr;
  int col = 0;
  if (!teutil::compileOne(exprU_, vars, 4, &tmp, &col)) {
    std::snprintf(exprStatus_, sizeof exprStatus_, "u: error near column %d.", col);
    teutil::freeExpr(tmp);
    return;
  }
  teutil::freeExpr(teU_);
  teU_ = tmp;
  std::strncpy(exprStatus_, "Custom u OK.", sizeof(exprStatus_) - 1);
  exprStatus_[sizeof(exprStatus_) - 1] = '\0';
}

void DiffusionSimilarityCase::update(float dt) { t_ += dt * 0.15f; }

void DiffusionSimilarityCase::draw3D() {
  float den = std::sqrt(std::max(1e-6f, 4.0f * nu_ * std::max(1e-3f, t_)));
  int nz = 24;
  int nx = 5;
  teT_ = t_;
  teUcap_ = U_;
  teNu_ = nu_;
  for (int j = 0; j <= nz; ++j) {
    float z = -L_ + (2.0f * L_) * static_cast<float>(j) / static_cast<float>(nz);
    float eta = z / den;
    float u = U_ * std::erf(eta);
    if (fieldMode_ == 1 && teU_ != nullptr) {
      teZ_ = z;
      double ev = te_eval(static_cast<te_expr*>(teU_));
      u = std::isfinite(ev) ? static_cast<float>(ev) : 0.0f;
    }
    for (int i = 0; i <= nx; ++i) {
      float x = -1.0f + 2.0f * static_cast<float>(i) / static_cast<float>(nx);
      Vector3 base{x, z, 0.0f};
      Vector3 dir{1.0f, 0.0f, 0.0f};
      Color c = ColorFromScalar(u, -U_, U_);
      DrawArrow3D(base, dir, 0.15f + 0.35f * std::abs(u), 0.05f, 0.02f, c);
    }
  }
}

void DiffusionSimilarityCase::drawUI() {
  float den = std::sqrt(std::max(1e-6f, 4.0f * nu_ * std::max(1e-3f, t_)));
  ImGui::TextUnformatted("u = U erf(z / sqrt(4 nu t)); self-similar solution of u_t = nu u_zz.");
  ImGui::Text("delta ~ sqrt(4 nu t) = %.4f", den);
  const char* modes[] = {"Built-in erf", "Custom u(z,t)"};
  if (ImGui::Combo("field mode", &fieldMode_, modes, 2)) {
    if (fieldMode_ == 1) {
      exprHash_ = 0;
    }
  }

  ImGui::SliderFloat("nu", &nu_, 0.01f, 0.3f);
  ImGui::SliderFloat("time t", &t_, 0.05f, 3.0f);
  ImGui::SliderFloat("U", &U_, 0.2f, 2.0f);
  ImGui::SliderFloat("half-thickness L (viz)", &L_, 1.0f, 4.0f);

  if (fieldMode_ == 1) {
    ImGui::TextWrapped("Variables: z, t, U, nu.");
    ImGui::InputTextMultiline("u", exprU_, sizeof(exprU_), ImVec2(-1, 36));
    uint64_t h = teutil::fnv1a64(exprU_);
    if (h != exprHash_) {
      exprHash_ = h;
      tryCompileCustom();
    }
    if (exprStatus_[0] != '\0') {
      ImGui::TextUnformatted(exprStatus_);
    }
  }
}
