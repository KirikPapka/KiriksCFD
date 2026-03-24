#include "cases/GeostrophicBalanceCase.hpp"

#include "cases/TinyExprHelpers.hpp"
#include "render/ArrowRenderer.hpp"
#include "render/SurfaceRenderer.hpp"

#include "imgui.h"
#include "raylib.h"
#include "tinyexpr.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>

GeostrophicBalanceCase::GeostrophicBalanceCase() = default;

GeostrophicBalanceCase::~GeostrophicBalanceCase() { releaseTe(); }

void GeostrophicBalanceCase::releaseTe() { teutil::freeExpr(teEta_); }

void GeostrophicBalanceCase::rebuildEtaBuiltin() {
  eta_.assign(static_cast<size_t>(n_ * n_), 0.0f);
  float dx = (2.0f * L_) / static_cast<float>(n_ - 1);
  float s2 = std::max(0.1f, sigma_) * std::max(0.1f, sigma_);
  for (int j = 0; j < n_; ++j) {
    for (int i = 0; i < n_; ++i) {
      float x = -L_ + static_cast<float>(i) * dx;
      float z = -L_ + static_cast<float>(j) * dx;
      float r2 = x * x + z * z;
      eta_[static_cast<size_t>(j * n_ + i)] = eta0_ * std::exp(-r2 / (2.0f * s2));
    }
  }
}

void GeostrophicBalanceCase::rebuildEtaCustom() {
  eta_.assign(static_cast<size_t>(n_ * n_), 0.0f);
  if (teEta_ == nullptr) {
    return;
  }
  float dx = (2.0f * L_) / static_cast<float>(n_ - 1);
  teEta0_ = eta0_;
  teSigma_ = sigma_;
  teF_ = f_;
  teG_ = g_;
  for (int j = 0; j < n_; ++j) {
    for (int i = 0; i < n_; ++i) {
      teX_ = -L_ + static_cast<double>(i) * dx;
      teZ_ = -L_ + static_cast<double>(j) * dx;
      double v = te_eval(static_cast<te_expr*>(teEta_));
      if (!std::isfinite(v)) {
        v = 0.0;
      }
      eta_[static_cast<size_t>(j * n_ + i)] = static_cast<float>(v);
    }
  }
}

void GeostrophicBalanceCase::tryCompileCustom() {
  teEta0_ = eta0_;
  teSigma_ = sigma_;
  teF_ = f_;
  teG_ = g_;
  te_variable vars[] = {
      {"x", &teX_, TE_VARIABLE, nullptr},     {"z", &teZ_, TE_VARIABLE, nullptr},
      {"eta0", &teEta0_, TE_VARIABLE, nullptr}, {"sigma", &teSigma_, TE_VARIABLE, nullptr},
      {"f", &teF_, TE_VARIABLE, nullptr},     {"g", &teG_, TE_VARIABLE, nullptr},
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

void GeostrophicBalanceCase::update(float /*dt*/) {
  if (fieldMode_ == 0) {
    rebuildEtaBuiltin();
  } else {
    rebuildEtaCustom();
  }
}

void GeostrophicBalanceCase::draw3D() {
  DrawHeightFieldSurface(n_, n_, eta_.data(), -L_, L_, -L_, L_, 1.0f, {100, 200, 160, 255});
  float dx = (2.0f * L_) / static_cast<float>(n_ - 1);
  int st = std::max(1, n_ / 10);
  float invf = (std::abs(f_) > 1e-6f) ? 1.0f / f_ : 0.0f;
  for (int j = st; j < n_ - 1; j += st) {
    for (int i = st; i < n_ - 1; i += st) {
      float x = -L_ + static_cast<float>(i) * dx;
      float z = -L_ + static_cast<float>(j) * dx;
      float detadx = (eta_[static_cast<size_t>(j * n_ + i + 1)] - eta_[static_cast<size_t>(j * n_ + i - 1)]) /
                     (2.0f * dx);
      float detadz = (eta_[static_cast<size_t>((j + 1) * n_ + i)] - eta_[static_cast<size_t>((j - 1) * n_ + i)]) /
                     (2.0f * dx);
      float ux = -g_ * invf * detadz;
      float uz = g_ * invf * detadx;
      float mag = std::sqrt(ux * ux + uz * uz);
      Vector3 base{x, 0.12f, z};
      Vector3 dir{ux, 0.0f, uz};
      if (mag > 1e-5f) {
        DrawArrow3D(base, dir, 0.25f + 0.4f * mag, 0.05f, 0.02f, {255, 220, 100, 230});
      }
    }
  }
}

void GeostrophicBalanceCase::drawUI() {
  ImGui::TextUnformatted("eta Gaussian hill; u_g from f u = (-g d eta/dz, 0, g d eta/dx) (signs as used here).");
  const char* modes[] = {"Built-in Gaussian", "Custom eta(x,z)"};
  if (ImGui::Combo("field mode", &fieldMode_, modes, 2)) {
    if (fieldMode_ == 1) {
      exprHash_ = 0;
    }
  }

  ImGui::SliderFloat("f", &f_, 0.2f, 3.0f);
  ImGui::SliderFloat("g", &g_, 1.0f, 15.0f);

  if (fieldMode_ == 0) {
    ImGui::SliderFloat("sigma", &sigma_, 0.3f, 2.0f);
    ImGui::SliderFloat("eta0", &eta0_, 0.05f, 0.6f);
  } else {
    ImGui::TextWrapped("Variables: x, z, eta0, sigma, f, g.");
    ImGui::InputTextMultiline("eta", exprEta_, sizeof(exprEta_), ImVec2(-1, 44));
    ImGui::SliderFloat("sigma", &sigma_, 0.3f, 2.0f);
    ImGui::SliderFloat("eta0", &eta0_, 0.05f, 0.6f);
    uint64_t h = teutil::fnv1a64(exprEta_);
    if (h != exprHash_) {
      exprHash_ = h;
      tryCompileCustom();
    }
    if (exprStatus_[0] != '\0') {
      ImGui::TextUnformatted(exprStatus_);
    }
  }

  ImGui::SliderFloat("half-domain L", &L_, 1.5f, 5.0f);
  ImGui::SliderInt("grid N", &n_, 24, 56);
}
