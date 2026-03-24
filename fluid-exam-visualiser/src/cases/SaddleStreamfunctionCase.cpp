#include "cases/SaddleStreamfunctionCase.hpp"

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

SaddleStreamfunctionCase::SaddleStreamfunctionCase() = default;

SaddleStreamfunctionCase::~SaddleStreamfunctionCase() { teutil::freeExpr(tePsi_); }

float SaddleStreamfunctionCase::psiBuiltin(float x, float z) const {
  float aa = std::max(0.15f, a_);
  float bb = std::max(0.15f, b_);
  return (x * x) / (aa * aa) - (z * z) / (bb * bb);
}

void SaddleStreamfunctionCase::tryCompileCustom() {
  teA_ = std::max(0.15, static_cast<double>(a_));
  teB_ = std::max(0.15, static_cast<double>(b_));
  te_variable vars[] = {
      {"x", &teX_, TE_VARIABLE, nullptr},
      {"z", &teZ_, TE_VARIABLE, nullptr},
      {"a", &teA_, TE_VARIABLE, nullptr},
      {"b", &teB_, TE_VARIABLE, nullptr},
  };
  void* tmp = nullptr;
  int col = 0;
  if (!teutil::compileOne(exprPsi_, vars, 4, &tmp, &col)) {
    std::snprintf(exprStatus_, sizeof exprStatus_, "psi: error near column %d.", col);
    teutil::freeExpr(tmp);
    return;
  }
  teutil::freeExpr(tePsi_);
  tePsi_ = tmp;
  std::strncpy(exprStatus_, "Custom psi OK.", sizeof(exprStatus_) - 1);
  exprStatus_[sizeof(exprStatus_) - 1] = '\0';
}

void SaddleStreamfunctionCase::rebuild() {
  psi_.assign(static_cast<size_t>(n_ * n_), 0.0f);
  omega_.assign(static_cast<size_t>(n_ * n_), 0.0f);
  float dx = (2.0f * L_) / static_cast<float>(n_ - 1);
  teA_ = std::max(0.15, static_cast<double>(a_));
  teB_ = std::max(0.15, static_cast<double>(b_));
  for (int j = 0; j < n_; ++j) {
    for (int i = 0; i < n_; ++i) {
      float x = -L_ + static_cast<float>(i) * dx;
      float z = -L_ + static_cast<float>(j) * dx;
      float p = 0.0f;
      if (fieldMode_ == 0) {
        p = psiBuiltin(x, z);
      } else if (tePsi_ != nullptr) {
        teX_ = x;
        teZ_ = z;
        double v = te_eval(static_cast<te_expr*>(tePsi_));
        p = std::isfinite(v) ? static_cast<float>(v) : 0.0f;
      }
      psi_[static_cast<size_t>(j * n_ + i)] = p;
    }
  }
  if (fieldMode_ == 0) {
    float aa = std::max(0.15f, a_);
    float bb = std::max(0.15f, b_);
    float lapPsi = 2.0f / (aa * aa) - 2.0f / (bb * bb);
    float omegaZ = -lapPsi;
    for (size_t k = 0; k < omega_.size(); ++k) {
      omega_[k] = omegaZ;
    }
  } else {
    for (int j = 1; j < n_ - 1; ++j) {
      for (int i = 1; i < n_ - 1; ++i) {
        float pxx = (psi_[static_cast<size_t>(j * n_ + i + 1)] - 2.0f * psi_[static_cast<size_t>(j * n_ + i)] +
                     psi_[static_cast<size_t>(j * n_ + i - 1)]) /
                    (dx * dx);
        float pzz = (psi_[static_cast<size_t>((j + 1) * n_ + i)] - 2.0f * psi_[static_cast<size_t>(j * n_ + i)] +
                     psi_[static_cast<size_t>((j - 1) * n_ + i)]) /
                    (dx * dx);
        omega_[static_cast<size_t>(j * n_ + i)] = -(pxx + pzz);
      }
    }
  }
  dirty_ = false;
}

void SaddleStreamfunctionCase::update(float /*dt*/) {
  if (dirty_) {
    rebuild();
  }
}

void SaddleStreamfunctionCase::draw3D() {
  float dx = (2.0f * L_) / static_cast<float>(n_ - 1);
  float pmin = psi_[0], pmax = psi_[0];
  float omin = omega_[0], omax = omega_[0];
  for (float p : psi_) {
    pmin = std::min(pmin, p);
    pmax = std::max(pmax, p);
  }
  for (float o : omega_) {
    omin = std::min(omin, o);
    omax = std::max(omax, o);
  }
  if (pmax <= pmin) {
    pmax = pmin + 1.0f;
  }

  for (int j = 0; j < n_ - 1; ++j) {
    for (int i = 0; i < n_ - 1; ++i) {
      auto corner = [&](int ii, int jj) {
        float x = -L_ + static_cast<float>(ii) * dx;
        float z = -L_ + static_cast<float>(jj) * dx;
        return Vector3{x, 0.02f, z};
      };
      float ps = psi_[static_cast<size_t>(j * n_ + i)];
      Color c = ColorFromScalar(ps, pmin, pmax);
      DrawTriangle3D(corner(i, j), corner(i + 1, j + 1), corner(i + 1, j), c);
      DrawTriangle3D(corner(i, j), corner(i, j + 1), corner(i + 1, j + 1), c);
    }
  }
  int st = std::max(1, n_ / 14);
  for (int j = st; j < n_ - 1; j += st) {
    for (int i = st; i < n_ - 1; i += st) {
      float x = -L_ + static_cast<float>(i) * dx;
      float z = -L_ + static_cast<float>(j) * dx;
      float dpsidx = (psi_[static_cast<size_t>(j * n_ + i + 1)] - psi_[static_cast<size_t>(j * n_ + i - 1)]) /
                     (2.0f * dx);
      float dpsidz = (psi_[static_cast<size_t>((j + 1) * n_ + i)] - psi_[static_cast<size_t>((j - 1) * n_ + i)]) /
                     (2.0f * dx);
      float ux = dpsidz;
      float uz = -dpsidx;
      float mag = std::sqrt(ux * ux + uz * uz);
      Vector3 base{x, 0.1f, z};
      Vector3 dir{ux, 0.0f, uz};
      if (mag > 1e-4f) {
        float om = omega_[static_cast<size_t>(j * n_ + i)];
        Color ac = ColorFromScalar(om, omin, omax);
        DrawArrow3D(base, dir, 0.12f + 0.08f * std::log(1.0f + mag), 0.04f, 0.015f, ac);
      }
    }
  }
}

void SaddleStreamfunctionCase::drawUI() {
  ImGui::TextUnformatted("psi surface; u = d psi/dz, w = -d psi/dx.");
  const char* modes[] = {"Built-in saddle", "Custom psi(x,z)"};
  if (ImGui::Combo("field mode", &fieldMode_, modes, 2)) {
    dirty_ = true;
    exprHash_ = 0;
  }

  if (fieldMode_ == 0) {
    float aa = std::max(0.15f, a_);
    float bb = std::max(0.15f, b_);
    float lapPsi = 2.0f / (aa * aa) - 2.0f / (bb * bb);
    float omegaZ = -lapPsi;
    ImGui::Text("omega_z = %.4f (uniform)", omegaZ);
  } else {
    ImGui::TextWrapped("Variables: x, z, a, b.");
    ImGui::InputTextMultiline("psi", exprPsi_, sizeof(exprPsi_), ImVec2(-1, 40));
    uint64_t h = teutil::fnv1a64(exprPsi_);
    if (h != exprHash_) {
      exprHash_ = h;
      tryCompileCustom();
      dirty_ = true;
    }
    if (exprStatus_[0] != '\0') {
      ImGui::TextUnformatted(exprStatus_);
    }
  }

  if (ImGui::SliderInt("grid N", &n_, 24, 64)) {
    dirty_ = true;
  }
  if (ImGui::SliderFloat("half-domain L", &L_, 1.0f, 3.5f)) {
    dirty_ = true;
  }
  if (ImGui::SliderFloat("a", &a_, 0.2f, 2.0f)) {
    dirty_ = true;
  }
  if (ImGui::SliderFloat("b", &b_, 0.2f, 2.0f)) {
    dirty_ = true;
  }
}
