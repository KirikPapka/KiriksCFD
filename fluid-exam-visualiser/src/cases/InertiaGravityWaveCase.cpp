#include "cases/InertiaGravityWaveCase.hpp"

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

InertiaGravityWaveCase::InertiaGravityWaveCase() = default;

InertiaGravityWaveCase::~InertiaGravityWaveCase() { teutil::freeExpr(teEta_); }

void InertiaGravityWaveCase::tryCompileCustom() {
  float K = std::sqrt(std::max(0.0f, kx_ * kx_ + ky_ * ky_));
  float w2 = f0_ * f0_ + g_ * H_ * K * K;
  float omega = std::sqrt(std::max(1e-8f, w2));
  teKx_ = kx_;
  teKy_ = ky_;
  teF0_ = f0_;
  teG_ = g_;
  teH_ = H_;
  teOmega_ = omega;
  te_variable vars[] = {
      {"x", &teX_, TE_VARIABLE, nullptr},     {"z", &teZ_, TE_VARIABLE, nullptr},
      {"t", &teT_, TE_VARIABLE, nullptr},     {"kx", &teKx_, TE_VARIABLE, nullptr},
      {"ky", &teKy_, TE_VARIABLE, nullptr},   {"f0", &teF0_, TE_VARIABLE, nullptr},
      {"g", &teG_, TE_VARIABLE, nullptr},      {"H", &teH_, TE_VARIABLE, nullptr},
      {"omega", &teOmega_, TE_VARIABLE, nullptr},
  };
  void* tmp = nullptr;
  int col = 0;
  if (!teutil::compileOne(exprEta_, vars, 9, &tmp, &col)) {
    std::snprintf(exprStatus_, sizeof exprStatus_, "eta: error near column %d.", col);
    teutil::freeExpr(tmp);
    return;
  }
  teutil::freeExpr(teEta_);
  teEta_ = tmp;
  std::strncpy(exprStatus_, "Custom eta OK.", sizeof(exprStatus_) - 1);
  exprStatus_[sizeof(exprStatus_) - 1] = '\0';
}

void InertiaGravityWaveCase::update(float dt) { time_ += dt * timeScale_; }

void InertiaGravityWaveCase::draw3D() {
  float K = std::sqrt(std::max(0.0f, kx_ * kx_ + ky_ * ky_));
  float w2 = f0_ * f0_ + g_ * H_ * K * K;
  float omega = std::sqrt(std::max(1e-8f, w2));

  if (fieldMode_ == 0) {
    float L = halfL_;
    int n = 10;
    for (int j = 0; j <= n; ++j) {
      for (int i = 0; i <= n; ++i) {
        float x = -L + (2.0f * L) * static_cast<float>(i) / static_cast<float>(n);
        float z = -L + (2.0f * L) * static_cast<float>(j) / static_cast<float>(n);
        float phase = K * (x * kx_ / std::max(1e-5f, K) + z * ky_ / std::max(1e-5f, K)) - omega * time_;
        float eta = 0.15f * std::cos(phase);
        Vector3 base{x, 0.05f + eta, z};
        float uh = 0.25f * std::sin(phase) * (kx_ / std::max(1e-5f, K));
        float wh = 0.25f * std::sin(phase) * (ky_ / std::max(1e-5f, K));
        Vector3 dir{uh, 0.0f, wh};
        float mag = std::sqrt(uh * uh + wh * wh);
        Color c = ColorFromScalar(mag, 0.0f, 0.35f);
        if (mag > 1e-4f) {
          DrawArrow3D(base, dir, 0.4f * mag, 0.05f, 0.02f, c);
        }
      }
    }
    DrawLine3D({-halfL_, 0, 0}, {halfL_, 0, 0}, {80, 80, 120, 120});
    return;
  }

  int n = std::max(4, gridN_);
  eta_.assign(static_cast<size_t>((n + 1) * (n + 1)), 0.0f);
  float L = halfL_;
  float dx = (2.0f * L) / static_cast<float>(n);
  teKx_ = kx_;
  teKy_ = ky_;
  teF0_ = f0_;
  teG_ = g_;
  teH_ = H_;
  teOmega_ = omega;
  teT_ = time_;
  if (teEta_ != nullptr) {
    for (int j = 0; j <= n; ++j) {
      for (int i = 0; i <= n; ++i) {
        teX_ = -L + static_cast<double>(i) * dx;
        teZ_ = -L + static_cast<double>(j) * dx;
        double v = te_eval(static_cast<te_expr*>(teEta_));
        float fv = std::isfinite(v) ? static_cast<float>(v) : 0.0f;
        eta_[static_cast<size_t>(j * (n + 1) + i)] = fv;
      }
    }
  }

  for (int j = 1; j < n; ++j) {
    for (int i = 1; i < n; ++i) {
      float x = -L + static_cast<float>(i) * dx;
      float z = -L + static_cast<float>(j) * dx;
      float h = eta_[static_cast<size_t>(j * (n + 1) + i)];
      Vector3 base{x, 0.05f + h, z};
      float dhdx = (eta_[static_cast<size_t>(j * (n + 1) + i + 1)] - eta_[static_cast<size_t>(j * (n + 1) + i - 1)]) /
                   (2.0f * dx);
      float dhdz = (eta_[static_cast<size_t>((j + 1) * (n + 1) + i)] - eta_[static_cast<size_t>((j - 1) * (n + 1) + i)]) /
                   (2.0f * dx);
      float uh = 0.35f * dhdx;
      float wh = 0.35f * dhdz;
      Vector3 dir{uh, 0.0f, wh};
      float mag = std::sqrt(uh * uh + wh * wh);
      Color c = ColorFromScalar(mag, 0.0f, 0.35f);
      if (mag > 1e-4f) {
        DrawArrow3D(base, dir, 0.4f * mag, 0.05f, 0.02f, c);
      }
    }
  }
  DrawLine3D({-L, 0, 0}, {L, 0, 0}, {80, 80, 120, 120});
}

void InertiaGravityWaveCase::drawUI() {
  float K = std::sqrt(std::max(0.0f, kx_ * kx_ + ky_ * ky_));
  float w2 = f0_ * f0_ + g_ * H_ * K * K;
  float omega = std::sqrt(std::max(1e-8f, w2));
  float cp = (K > 1e-5f) ? omega / K : 0.0f;
  float cg = (K > 1e-5f) ? (g_ * H_ * K) / std::max(1e-5f, omega) : std::sqrt(g_ * H_);
  float Ld = (std::abs(f0_) > 1e-6f) ? std::sqrt(g_ * H_) / std::abs(f0_) : 0.0f;
  ImGui::TextUnformatted("omega = sqrt(f0^2 + g H K^2); compare K to 1/Ld.");
  ImGui::Text("Ld = sqrt(gH)/|f0| = %.4f   K = %.4f   omega = %.4f", Ld, K, omega);
  ImGui::Text("phase c_p = omega/K = %.4f   group c_g ~ d omega/dK = %.4f", cp, cg);

  const char* modes[] = {"Built-in plane wave", "Custom eta(x,z,t)"};
  if (ImGui::Combo("field mode", &fieldMode_, modes, 2)) {
    if (fieldMode_ == 1) {
      exprHash_ = 0;
    }
  }

  ImGui::SliderFloat("f0", &f0_, 0.0f, 3.0f);
  ImGui::SliderFloat("g", &g_, 1.0f, 15.0f);
  ImGui::SliderFloat("H", &H_, 0.2f, 3.0f);
  ImGui::SliderFloat("kx", &kx_, 0.1f, 4.0f);
  ImGui::SliderFloat("ky", &ky_, 0.0f, 4.0f);
  ImGui::SliderFloat("time scale", &timeScale_, 0.0f, 2.0f);

  if (fieldMode_ == 1) {
    ImGui::TextWrapped("Variables: x, z, t, kx, ky, f0, g, H, omega (omega from dispersion above).");
    ImGui::InputTextMultiline("eta", exprEta_, sizeof(exprEta_), ImVec2(-1, 40));
    ImGui::SliderInt("grid N", &gridN_, 8, 28);
    ImGui::SliderFloat("half-domain L", &halfL_, 1.5f, 4.0f);
    uint64_t h = teutil::fnv1a64(exprEta_);
    if (h != exprHash_) {
      exprHash_ = h;
      tryCompileCustom();
    }
    if (exprStatus_[0] != '\0') {
      ImGui::TextUnformatted(exprStatus_);
    }
  }
}
