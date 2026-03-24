#include "cases/EkmanCase.hpp"

#include "cases/TinyExprHelpers.hpp"
#include "render/ArrowRenderer.hpp"

#include "imgui.h"
#include "raylib.h"
#include "tinyexpr.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>

EkmanCase::EkmanCase() = default;

EkmanCase::~EkmanCase() {
  teutil::freeExpr(teU_);
  teutil::freeExpr(teV_);
}

void EkmanCase::tryCompileCustom() {
  teD_ = std::max(0.15, static_cast<double>(decayScale_));
  teF_ = rotationFactor_;
  teU0_ = u0_;
  te_variable vars[] = {
      {"z", &teZ_, TE_VARIABLE, nullptr},
      {"d", &teD_, TE_VARIABLE, nullptr},
      {"f", &teF_, TE_VARIABLE, nullptr},
      {"u0", &teU0_, TE_VARIABLE, nullptr},
  };
  void* tu = nullptr;
  void* tv = nullptr;
  int cu = 0;
  int cv = 0;
  if (!teutil::compileOne(exprU_, vars, 4, &tu, &cu)) {
    std::snprintf(exprStatus_, sizeof exprStatus_, "u: error near column %d.", cu);
    teutil::freeExpr(tu);
    return;
  }
  if (!teutil::compileOne(exprV_, vars, 4, &tv, &cv)) {
    std::snprintf(exprStatus_, sizeof exprStatus_, "v: error near column %d.", cv);
    teutil::freeExpr(tu);
    teutil::freeExpr(tv);
    return;
  }
  teutil::freeExpr(teU_);
  teutil::freeExpr(teV_);
  teU_ = tu;
  teV_ = tv;
  std::strncpy(exprStatus_, "Custom u,v OK.", sizeof(exprStatus_) - 1);
  exprStatus_[sizeof(exprStatus_) - 1] = '\0';
}

void EkmanCase::update(float /*dt*/) {}

void EkmanCase::draw3D() {
  float d = std::max(0.15f, decayScale_);
  float f = rotationFactor_;
  teD_ = d;
  teF_ = f;
  teU0_ = u0_;
  for (int c = 0; c < nColumns_; ++c) {
    float cx = (static_cast<float>(c) - 0.5f * static_cast<float>(nColumns_ - 1)) * columnSpacing_;
    for (int k = 0; k < nLevels_; ++k) {
      float z = static_cast<float>(k) / static_cast<float>(nLevels_ - 1) * zMax_;
      float u = 0.0f;
      float v = 0.0f;
      float magDecay = 1.0f;
      if (fieldMode_ == 0) {
        float s = z / d;
        magDecay = std::exp(-s);
        float ang = f * s;
        u = u0_ * magDecay * std::cos(ang);
        v = u0_ * magDecay * std::sin(ang);
      } else if (teU_ != nullptr && teV_ != nullptr) {
        teZ_ = z;
        double eu = te_eval(static_cast<te_expr*>(teU_));
        double ev = te_eval(static_cast<te_expr*>(teV_));
        u = std::isfinite(eu) ? static_cast<float>(eu) : 0.0f;
        v = std::isfinite(ev) ? static_cast<float>(ev) : 0.0f;
        magDecay = std::clamp(std::sqrt(u * u + v * v) / std::max(1e-4f, u0_), 0.0f, 1.5f);
      }
      Vector3 base{cx, z, 0.0f};
      Vector3 dir{u, 0.0f, v};
      float dm = std::sqrt(u * u + v * v);
      Color col = {static_cast<unsigned char>(80 + 175 * std::clamp(magDecay, 0.0f, 1.0f)), 200,
                   static_cast<unsigned char>(255 - 120 * std::clamp(magDecay, 0.0f, 1.0f)), 255};
      if (dm > 1e-4f) {
        DrawArrow3D(base, dir, 0.55f * dm, 0.07f, 0.03f, col);
      }
    }
    DrawLine3D({cx, 0, 0}, {cx, zMax_, 0}, {100, 100, 100, 180});
  }
}

void EkmanCase::drawUI() {
  ImGui::TextUnformatted(
      "u(z), v(z) ~ e^{-z/d} (cos, sin)(f z/d): decay + turning (schematic Ekman layer).");
  const char* modes[] = {"Built-in spiral", "Custom u(z), v(z)"};
  if (ImGui::Combo("field mode", &fieldMode_, modes, 2)) {
    if (fieldMode_ == 1) {
      exprHash_ = 0;
    }
  }

  if (fieldMode_ == 0) {
    ImGui::SliderFloat("decay scale d", &decayScale_, 0.2f, 3.0f);
    ImGui::SliderFloat("rotation factor f", &rotationFactor_, 0.2f, 3.0f);
    ImGui::SliderFloat("surface scale U0", &u0_, 0.2f, 2.0f);
  } else {
    ImGui::TextWrapped("Variables: z, d, f, u0 (d,f,u0 come from sliders).");
    ImGui::InputTextMultiline("u (x-h)", exprU_, sizeof(exprU_), ImVec2(-1, 36));
    ImGui::InputTextMultiline("v (y-h)", exprV_, sizeof(exprV_), ImVec2(-1, 36));
    ImGui::SliderFloat("decay scale d", &decayScale_, 0.2f, 3.0f);
    ImGui::SliderFloat("rotation factor f", &rotationFactor_, 0.2f, 3.0f);
    ImGui::SliderFloat("surface scale U0", &u0_, 0.2f, 2.0f);
    uint64_t h = teutil::hashPair(exprU_, exprV_);
    if (h != exprHash_) {
      exprHash_ = h;
      tryCompileCustom();
    }
    if (exprStatus_[0] != '\0') {
      ImGui::TextUnformatted(exprStatus_);
    }
  }

  ImGui::SliderFloat("vertical extent", &zMax_, 1.0f, 8.0f);
  ImGui::SliderInt("z samples", &nLevels_, 8, 48);
  ImGui::SliderInt("columns", &nColumns_, 1, 6);
  ImGui::SliderFloat("column spacing", &columnSpacing_, 0.5f, 3.0f);
}
