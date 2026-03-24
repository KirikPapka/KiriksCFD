#include "cases/PipeFlowCase.hpp"

#include "cases/TinyExprHelpers.hpp"
#include "render/TubeRenderer.hpp"

#include "imgui.h"
#include "raylib.h"
#include "tinyexpr.h"

#include <cmath>
#include <cstdio>
#include <cstring>

PipeFlowCase::PipeFlowCase() = default;

PipeFlowCase::~PipeFlowCase() { teutil::freeExpr(teU_); }

float PipeFlowCase::evalProfile(float r, float R, void* user) {
  auto* self = static_cast<PipeFlowCase*>(user);
  if (self->fieldMode_ == 0 || self->teU_ == nullptr) {
    if (R < 1e-6f) {
      return 0.0f;
    }
    float rr = r / R;
    return self->uMax_ * (1.0f - rr * rr);
  }
  self->teR_ = r;
  self->teRpipe_ = R;
  self->teUmaxBind_ = self->uMax_;
  double v = te_eval(static_cast<te_expr*>(self->teU_));
  return std::isfinite(v) ? static_cast<float>(v) : 0.0f;
}

void PipeFlowCase::tryCompileCustom() {
  teR_ = 0;
  teRpipe_ = radius_;
  teUmaxBind_ = uMax_;
  te_variable vars[] = {
      {"r", &teR_, TE_VARIABLE, nullptr},
      {"R", &teRpipe_, TE_VARIABLE, nullptr},
      {"umax", &teUmaxBind_, TE_VARIABLE, nullptr},
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
  std::strncpy(exprStatus_, "Custom profile OK.", sizeof(exprStatus_) - 1);
  exprStatus_[sizeof(exprStatus_) - 1] = '\0';
}

void PipeFlowCase::update(float /*dt*/) {}

void PipeFlowCase::draw3D() {
  float z0 = -0.5f * length_;
  float z1 = 0.5f * length_;
  Color cold = {30, 40, 120, 255};
  Color hot = {255, 240, 80, 255};
  DrawPoiseuilleCylinderFn(radius_, z0, z1, radialSegs_, thetaSegs_, uMax_, cold, hot, &PipeFlowCase::evalProfile,
                           this);
  if (showSlice_) {
    DrawPoiseuilleSliceFn(radius_, 0.0f, thetaSegs_, uMax_, cold, hot, &PipeFlowCase::evalProfile, this);
  }
}

void PipeFlowCase::drawUI() {
  ImGui::TextUnformatted("Axisymmetric Hagen-Poiseuille: u_z(r) = Umax (1 - r^2/R^2) by default.");
  const char* modes[] = {"Built-in parabolic", "Custom u(r)"};
  if (ImGui::Combo("field mode", &fieldMode_, modes, 2)) {
    if (fieldMode_ == 1) {
      exprHash_ = 0;
    }
  }

  ImGui::SliderFloat("radius R", &radius_, 0.2f, 2.5f);
  ImGui::SliderFloat("length", &length_, 0.5f, 6.0f);
  ImGui::SliderFloat("Umax (colour scale)", &uMax_, 0.1f, 2.0f);

  if (fieldMode_ == 1) {
    ImGui::TextWrapped("Variables: r (radial distance), R (pipe radius), umax (slider above).");
    ImGui::InputTextMultiline("u_z", exprU_, sizeof(exprU_), ImVec2(-1, 36));
    uint64_t h = teutil::fnv1a64(exprU_);
    if (h != exprHash_) {
      exprHash_ = h;
      tryCompileCustom();
    }
    if (exprStatus_[0] != '\0') {
      ImGui::TextUnformatted(exprStatus_);
    }
  }

  ImGui::Checkbox("cross-section slice at z=0", &showSlice_);
  ImGui::SliderInt("radial shells", &radialSegs_, 4, 24);
  ImGui::SliderInt("theta segments", &thetaSegs_, 8, 48);
}
