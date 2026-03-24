#include "cases/RossbyWaveCase.hpp"

#include "cases/TinyExprHelpers.hpp"
#include "render/SurfaceRenderer.hpp"

#include "imgui.h"
#include "raylib.h"
#include "tinyexpr.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>

RossbyWaveCase::RossbyWaveCase() = default;

RossbyWaveCase::~RossbyWaveCase() { releaseTe(); }

void RossbyWaveCase::releaseTe() { teutil::freeExpr(teEta_); }

void RossbyWaveCase::ensureGrid() {
  eta_.resize(static_cast<size_t>(n_ * n_));
}

void RossbyWaveCase::fillEtaBuiltin() {
  float Ld2 = (std::abs(f0_) > 1e-6f) ? (g_ * H_) / (f0_ * f0_) : 1e6f;
  float invLd2 = 1.0f / std::max(1e-6f, Ld2);
  float K2 = kx_ * kx_ + ky_ * ky_;
  float omega = -beta_ * kx_ / std::max(1e-6f, K2 + invLd2);
  float dx = (2.0f * L_) / static_cast<float>(n_ - 1);
  for (int j = 0; j < n_; ++j) {
    for (int i = 0; i < n_; ++i) {
      float x = -L_ + static_cast<float>(i) * dx;
      float y = -L_ + static_cast<float>(j) * dx;
      float phase = kx_ * x + ky_ * y - omega * time_;
      eta_[static_cast<size_t>(j * n_ + i)] = eta0_ * std::sin(phase);
    }
  }
}

void RossbyWaveCase::fillEtaCustom() {
  const bool have = (teEta_ != nullptr);
  teKx_ = kx_;
  teKy_ = ky_;
  teBeta_ = beta_;
  teG_ = g_;
  teH_ = H_;
  teF0_ = f0_;
  teEta0_ = eta0_;
  float dx = (2.0f * L_) / static_cast<float>(n_ - 1);
  for (int j = 0; j < n_; ++j) {
    for (int i = 0; i < n_; ++i) {
      float x = -L_ + static_cast<float>(i) * dx;
      float y = -L_ + static_cast<float>(j) * dx;
      teX_ = x;
      teY_ = y;
      teT_ = time_;
      float v = 0.0f;
      if (have) {
        double ev = te_eval(static_cast<te_expr*>(teEta_));
        v = static_cast<float>(ev);
        if (!std::isfinite(v)) {
          v = 0.0f;
        }
      }
      eta_[static_cast<size_t>(j * n_ + i)] = v;
    }
  }
}

void RossbyWaveCase::tryCompileCustom() {
  char err[192];
  teEta0_ = eta0_;
  te_variable vars[] = {
      {"x", &teX_, TE_VARIABLE, nullptr},       {"y", &teY_, TE_VARIABLE, nullptr},
      {"t", &teT_, TE_VARIABLE, nullptr},       {"kx", &teKx_, TE_VARIABLE, nullptr},
      {"ky", &teKy_, TE_VARIABLE, nullptr},     {"beta", &teBeta_, TE_VARIABLE, nullptr},
      {"g", &teG_, TE_VARIABLE, nullptr},       {"H", &teH_, TE_VARIABLE, nullptr},
      {"f0", &teF0_, TE_VARIABLE, nullptr},    {"eta0", &teEta0_, TE_VARIABLE, nullptr},
  };
  void* tmp = nullptr;
  int col = 0;
  const bool had = (teEta_ != nullptr);
  if (!teutil::compileOne(exprEta_, vars, 10, &tmp, &col)) {
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

void RossbyWaveCase::update(float dt) {
  time_ += dt * timeScale_;
  ensureGrid();
  if (fieldMode_ == 0) {
    fillEtaBuiltin();
  } else {
    fillEtaCustom();
  }
}

void RossbyWaveCase::draw3D() {
  DrawHeightFieldSurface(n_, n_, eta_.data(), -L_, L_, -L_, L_, 1.0f, {120, 180, 255, 255});
}

void RossbyWaveCase::drawUI() {
  float Ld = (std::abs(f0_) > 1e-6f) ? std::sqrt(g_ * H_) / std::abs(f0_) : 0.0f;
  float Ld2 = Ld * Ld;
  float invLd2 = (Ld2 > 1e-6f) ? 1.0f / Ld2 : 0.0f;
  float K2 = kx_ * kx_ + ky_ * ky_;
  float omega = -beta_ * kx_ / std::max(1e-6f, K2 + invLd2);
  float cp = (std::abs(kx_) > 1e-6f) ? omega / kx_ : 0.0f;
  ImGui::TextUnformatted("omega = - beta kx / (K^2 + Ld^{-2}),  Ld = sqrt(g H)/|f0|.");
  ImGui::Text("Ld = %.4f   omega = %.5f   c_p,x = %.4f (negative => westward)", Ld, omega, cp);

  const char* modes[] = {"Built-in Rossby plane wave", "Custom eta(x,y,t)"};
  if (ImGui::Combo("field mode", &fieldMode_, modes, 2)) {
    if (fieldMode_ == 1) {
      exprHash_ = 0;
    }
  }

  if (fieldMode_ == 0) {
    ImGui::SliderFloat("beta", &beta_, 0.1f, 3.0f);
    ImGui::SliderFloat("f0", &f0_, 0.2f, 3.0f);
    ImGui::SliderFloat("g", &g_, 1.0f, 15.0f);
    ImGui::SliderFloat("H", &H_, 0.2f, 3.0f);
    ImGui::SliderFloat("kx", &kx_, 0.2f, 4.0f);
    ImGui::SliderFloat("ky", &ky_, 0.0f, 3.0f);
    ImGui::SliderFloat("eta0", &eta0_, 0.05f, 0.6f);
  } else {
    ImGui::TextWrapped(
        "Variables: x, y, t, kx, ky, beta, g, H, f0, eta0 (sliders below feed the formula). Live update.");
    ImGui::InputTextMultiline("eta", exprEta_, sizeof(exprEta_), ImVec2(-1, 48));
    ImGui::SliderFloat("kx", &kx_, 0.2f, 4.0f);
    ImGui::SliderFloat("ky", &ky_, 0.0f, 3.0f);
    ImGui::SliderFloat("beta", &beta_, 0.1f, 3.0f);
    ImGui::SliderFloat("f0", &f0_, 0.2f, 3.0f);
    ImGui::SliderFloat("g", &g_, 1.0f, 15.0f);
    ImGui::SliderFloat("H", &H_, 0.2f, 3.0f);
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

  ImGui::SliderFloat("half-domain", &L_, 1.0f, 5.0f);
  ImGui::SliderInt("grid N", &n_, 20, 64);
  ImGui::SliderFloat("time scale", &timeScale_, 0.0f, 2.0f);
}
