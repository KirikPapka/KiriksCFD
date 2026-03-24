#include "cases/StreamfunctionVorticityCase.hpp"

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

StreamfunctionVorticityCase::StreamfunctionVorticityCase() = default;

StreamfunctionVorticityCase::~StreamfunctionVorticityCase() { teutil::freeExpr(tePsi_); }

void StreamfunctionVorticityCase::tryCompileCustom() {
  teAmp_ = amp_;
  teK_ = k_;
  te_variable vars[] = {
      {"x", &teX_, TE_VARIABLE, nullptr},
      {"z", &teZ_, TE_VARIABLE, nullptr},
      {"amp", &teAmp_, TE_VARIABLE, nullptr},
      {"k", &teK_, TE_VARIABLE, nullptr},
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

void StreamfunctionVorticityCase::rebuild() {
  psi_.assign(static_cast<size_t>(n_ * n_), 0.0f);
  omega_.assign(static_cast<size_t>(n_ * n_), 0.0f);
  float dx = (2.0f * L_) / static_cast<float>(n_ - 1);
  teAmp_ = amp_;
  teK_ = k_;
  for (int j = 0; j < n_; ++j) {
    for (int i = 0; i < n_; ++i) {
      float x = -L_ + static_cast<float>(i) * dx;
      float z = -L_ + static_cast<float>(j) * dx;
      float p = 0.0f;
      if (fieldMode_ == 0) {
        p = psiAt(x, z);
      } else if (tePsi_ != nullptr) {
        teX_ = x;
        teZ_ = z;
        double v = te_eval(static_cast<te_expr*>(tePsi_));
        p = std::isfinite(v) ? static_cast<float>(v) : 0.0f;
      }
      psi_[static_cast<size_t>(j * n_ + i)] = p;
    }
  }
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
  dirty_ = false;
}

float StreamfunctionVorticityCase::psiAt(float x, float z) const {
  switch (preset_) {
    case 1: {
      float r2 = x * x + z * z;
      return -amp_ * std::exp(-r2 / (0.6f * 0.6f));
    }
    case 2:
      return amp_ * 0.3f * std::log(std::max(0.08f, std::sqrt(x * x + z * z)));
    case 0:
    default:
      return amp_ * std::sin(k_ * x) * std::sin(k_ * z);
  }
}

void StreamfunctionVorticityCase::update(float /*dt*/) {
  if (dirty_) {
    rebuild();
  }
}

void StreamfunctionVorticityCase::draw3D() {
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
      auto p = [&](int ii, int jj) {
        float x = -L_ + static_cast<float>(ii) * dx;
        float z = -L_ + static_cast<float>(jj) * dx;
        return Vector3{x, 0.02f, z};
      };
      float ps00 = psi_[static_cast<size_t>(j * n_ + i)];
      Color c00 = ColorFromScalar(ps00, pmin, pmax);
      DrawTriangle3D(p(i, j), p(i + 1, j + 1), p(i + 1, j), c00);
      DrawTriangle3D(p(i, j), p(i, j + 1), p(i + 1, j + 1), c00);
    }
  }

  int st = std::max(1, n_ / 12);
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
      Vector3 base{x, 0.08f, z};
      Vector3 dir{ux, 0.0f, uz};
      float om = omega_[static_cast<size_t>(j * n_ + i)];
      Color ac = ColorFromScalar(om, omin, omax);
      if (mag > 1e-4f) {
        DrawArrow3D(base, dir, 0.2f + 0.25f * mag, 0.05f, 0.018f, ac);
      }
    }
  }
}

void StreamfunctionVorticityCase::drawUI() {
  ImGui::TextUnformatted("u_h = curl(psi * y_hat); div u = 0; omega = -nabla_h^2 psi (scalar).");
  const char* modes[] = {"Built-in presets", "Custom psi(x,z)"};
  if (ImGui::Combo("field mode", &fieldMode_, modes, 2)) {
    dirty_ = true;
    exprHash_ = 0;
  }

  if (ImGui::SliderInt("grid N", &n_, 16, 64)) {
    dirty_ = true;
  }
  if (ImGui::SliderFloat("half-domain L", &L_, 0.8f, 3.5f)) {
    dirty_ = true;
  }

  if (fieldMode_ == 0) {
    const char* items[] = {"sin(kx)sin(kz)", "Gaussian vortex", "monopole ~ log r"};
    if (ImGui::Combo("psi preset", &preset_, items, 3)) {
      dirty_ = true;
    }
  } else {
    ImGui::TextWrapped("Variables: x, z, amp, k.");
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

  if (ImGui::SliderFloat("amplitude", &amp_, 0.1f, 1.5f)) {
    dirty_ = true;
  }
  if (fieldMode_ == 0 && preset_ == 0 && ImGui::SliderFloat("k", &k_, 0.4f, 3.0f)) {
    dirty_ = true;
  }
  if (fieldMode_ == 1 && ImGui::SliderFloat("k", &k_, 0.4f, 3.0f)) {
    dirty_ = true;
  }
}
