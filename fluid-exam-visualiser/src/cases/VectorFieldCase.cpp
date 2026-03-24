#include "cases/VectorFieldCase.hpp"

#include "math/Streamline.hpp"
#include "render/ArrowRenderer.hpp"
#include "render/ColorMap.hpp"

#include "imgui.h"
#include "raylib.h"

#include "tinyexpr.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>

namespace {

uint64_t hashExprTriplet(const char* a, const char* b, const char* c) {
  uint64_t h = 1469598103934665603ull;
  auto feed = [&](const char* s) {
    for (const char* p = s; *p != '\0'; ++p) {
      h ^= static_cast<uint64_t>(static_cast<unsigned char>(*p));
      h *= 1099511628211ull;
    }
    h ^= 0xFFull;
    h *= 1099511628211ull;
  };
  feed(a);
  feed(b);
  feed(c);
  return h;
}

}  // namespace

VectorFieldCase::VectorFieldCase() = default;

VectorFieldCase::~VectorFieldCase() { releaseCompiledExprs(); }

void VectorFieldCase::releaseCompiledExprs() {
  for (void*& p : teExpr_) {
    te_free(static_cast<te_expr*>(p));
    p = nullptr;
  }
}

bool VectorFieldCase::tryCompileCustomExprs(char* errBuf, int errBufLen) {
  te_variable vars[] = {
      {"x", &teVarX_, TE_VARIABLE, nullptr},
      {"y", &teVarY_, TE_VARIABLE, nullptr},
      {"z", &teVarZ_, TE_VARIABLE, nullptr},
  };
  const char* labels[] = {"u_x", "u_y", "u_z"};
  const char* sources[] = {exprUx_, exprUy_, exprUz_};
  te_expr* tmp[3] = {nullptr, nullptr, nullptr};
  for (int c = 0; c < 3; ++c) {
    int err = 0;
    te_expr* ex = te_compile(sources[c], vars, 3, &err);
    if (!ex) {
      exprCompileError_ = err;
      std::snprintf(errBuf, static_cast<size_t>(errBufLen),
                    "Cannot compile %s: error near column %d.", labels[c], err);
      for (int i = 0; i < c; ++i) {
        te_free(tmp[i]);
      }
      return false;
    }
    tmp[c] = ex;
  }
  releaseCompiledExprs();
  teExpr_[0] = tmp[0];
  teExpr_[1] = tmp[1];
  teExpr_[2] = tmp[2];
  exprCompileError_ = 0;
  exprStatus_[0] = '\0';
  return true;
}

void VectorFieldCase::fillGridFromCompiledExprs() {
  for (int k = 0; k < field_.nz; ++k) {
    for (int j = 0; j < field_.ny; ++j) {
      for (int i = 0; i < field_.nx; ++i) {
        Vector3 p = field_.nodePosition(i, j, k);
        teVarX_ = static_cast<double>(p.x);
        teVarY_ = static_cast<double>(p.y);
        teVarZ_ = static_cast<double>(p.z);
        double ux = te_eval(static_cast<te_expr*>(teExpr_[0]));
        double uy = te_eval(static_cast<te_expr*>(teExpr_[1]));
        double uz = te_eval(static_cast<te_expr*>(teExpr_[2]));
        if (!std::isfinite(ux)) {
          ux = 0;
        }
        if (!std::isfinite(uy)) {
          uy = 0;
        }
        if (!std::isfinite(uz)) {
          uz = 0;
        }
        field_.at(i, j, k) = {static_cast<float>(ux), static_cast<float>(uy), static_cast<float>(uz)};
      }
    }
  }
}

void VectorFieldCase::rebuildField() {
  field_.resize(gridN_, gridN_, gridN_, -box_, box_, -box_, box_, -box_, box_);

  if (fieldSource_ == 0) {
    releaseCompiledExprs();
    exprCompileError_ = 0;
    exprStatus_[0] = '\0';
    AnalyticFieldId id = AnalyticFieldId::Shear2D;
    if (fieldPreset_ == 1) {
      id = AnalyticFieldId::Helical;
    } else if (fieldPreset_ == 2) {
      id = AnalyticFieldId::SinCos2D;
    }
    field_.fillAnalytic(id, paramA_);
  } else {
    char err[192];
    const bool haveOld = (teExpr_[0] != nullptr && teExpr_[1] != nullptr && teExpr_[2] != nullptr);
    if (tryCompileCustomExprs(err, static_cast<int>(sizeof err))) {
      std::strncpy(exprStatus_, "Live update OK.", sizeof(exprStatus_) - 1);
      exprStatus_[sizeof(exprStatus_) - 1] = '\0';
      fillGridFromCompiledExprs();
    } else {
      std::strncpy(exprStatus_, err, sizeof(exprStatus_) - 1);
      exprStatus_[sizeof(exprStatus_) - 1] = '\0';
      if (haveOld) {
        fillGridFromCompiledExprs();
      } else {
        for (int k = 0; k < field_.nz; ++k) {
          for (int j = 0; j < field_.ny; ++j) {
            for (int i = 0; i < field_.nx; ++i) {
              field_.at(i, j, k) = {0, 0, 0};
            }
          }
        }
      }
    }
  }

  computeSpeed(field_, speed_);
  computeDivergence(field_, div_);
  computeCurl(field_, curl_);
  computeVorticityMagnitude(curl_, vortMag_);
  dirty_ = false;
}

void VectorFieldCase::rebuildStreamlines() {
  streamlines_.clear();
  if (!showStreamlines_) {
    return;
  }
  StreamlineParams p;
  p.stepSize = slStep_;
  p.maxSteps = slMaxSteps_;
  p.forward = true;
  int n = std::max(2, seedsPerAxis_);
  float lo = -box_ * 0.85f;
  float hi = box_ * 0.85f;
  for (int k = 0; k < n; ++k) {
    for (int j = 0; j < n; ++j) {
      for (int i = 0; i < n; ++i) {
        float fx = lo + (hi - lo) * static_cast<float>(i) / static_cast<float>(n - 1);
        float fy = lo + (hi - lo) * static_cast<float>(j) / static_cast<float>(n - 1);
        float fz = lo + (hi - lo) * static_cast<float>(k) / static_cast<float>(n - 1);
        auto line = traceStreamline(field_, {fx, fy, fz}, p);
        if (line.size() > 2) {
          streamlines_.push_back(std::move(line));
        }
      }
    }
  }
}

float VectorFieldCase::scalarAtNode(int i, int j, int k) const {
  switch (colorMode_) {
    case VectorColorMode::Divergence:
      return div_.at(i, j, k);
    case VectorColorMode::Vorticity:
      return vortMag_.at(i, j, k);
    case VectorColorMode::Speed:
    default:
      return speed_.at(i, j, k);
  }
}

void VectorFieldCase::update(float /*dt*/) {
  if (dirty_) {
    rebuildField();
    rebuildStreamlines();
  }
}

void VectorFieldCase::draw3D() {
  float smin = 1e9f;
  float smax = -1e9f;
  for (int k = 0; k < field_.nz; ++k) {
    for (int j = 0; j < field_.ny; ++j) {
      for (int i = 0; i < field_.nx; ++i) {
        float s = scalarAtNode(i, j, k);
        smin = std::min(smin, s);
        smax = std::max(smax, s);
      }
    }
  }
  if (smax <= smin) {
    smax = smin + 1.0f;
  }

  for (int k = 0; k < field_.nz; ++k) {
    for (int j = 0; j < field_.ny; ++j) {
      for (int i = 0; i < field_.nx; ++i) {
        Vector3 p = field_.nodePosition(i, j, k);
        Vector3 v = field_.at(i, j, k);
        float vm = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        if (vm < 1e-5f) {
          continue;
        }
        float s = scalarAtNode(i, j, k);
        Color col = ColorFromScalar(s, smin, smax);
        DrawArrow3D(p, v, arrowScale_ * vm, 0.08f + 0.04f * vm, 0.02f, col);
      }
    }
  }

  if (showStreamlines_) {
    for (const auto& line : streamlines_) {
      for (size_t t = 1; t < line.size(); ++t) {
        DrawLine3D(line[t - 1], line[t], {220, 220, 255, 200});
      }
    }
  }
}

void VectorFieldCase::drawUI() {
  ImGui::TextUnformatted("u(x,y,z) on a cube; divergence and curl by central differences on the grid.");
  const char* srcItems[] = {"Built-in presets (dropdown only)", "Custom: type u_x, u_y, u_z (tinyexpr)"};
  if (ImGui::Combo("field source", &fieldSource_, srcItems, 2)) {
    dirty_ = true;
    if (fieldSource_ == 1) {
      customExprHash_ = 0;
    }
  }

  if (fieldSource_ == 0) {
    if (ImGui::SliderInt("grid N", &gridN_, 6, 24)) {
      dirty_ = true;
    }
    if (ImGui::SliderFloat("half-box size", &box_, 0.5f, 4.0f)) {
      dirty_ = true;
    }
    const char* presets[] = {"u = (y, x, 0)", "u = (z, a, -x)", "u = (sin y, cos x, 0)"};
    if (ImGui::Combo("preset", &fieldPreset_, presets, 3)) {
      dirty_ = true;
    }
    if (fieldPreset_ == 1) {
      if (ImGui::SliderFloat("parameter a", &paramA_, -2.0f, 2.0f)) {
        dirty_ = true;
      }
    }
  } else {
    ImGui::TextWrapped(
        "Formulas update live as you type. If the current text does not parse, the last good field is "
        "kept and an error is shown. Coordinates x, y, z match the scene (Y up). Examples: y, -0.5*x+z, "
        "sin(y), pow(x,2)+z. Ops: + - * / ^. Functions: sin cos tan sqrt exp ln log (base 10) pow abs "
        "floor ceil asin acos atan atan2. Constants: pi() and e().");
    ImGui::InputTextMultiline("u_x", exprUx_, sizeof(exprUx_), ImVec2(-1, 36));
    ImGui::InputTextMultiline("u_y", exprUy_, sizeof(exprUy_), ImVec2(-1, 36));
    ImGui::InputTextMultiline("u_z", exprUz_, sizeof(exprUz_), ImVec2(-1, 36));
    if (ImGui::Button("Load preset into editors")) {
      if (fieldPreset_ == 0) {
        std::strcpy(exprUx_, "y");
        std::strcpy(exprUy_, "x");
        std::strcpy(exprUz_, "0");
      } else if (fieldPreset_ == 1) {
        std::snprintf(exprUx_, sizeof(exprUx_), "z");
        std::snprintf(exprUy_, sizeof(exprUy_), "%.4g", static_cast<double>(paramA_));
        std::strcpy(exprUz_, "-x");
      } else {
        std::strcpy(exprUx_, "sin(y)");
        std::strcpy(exprUy_, "cos(x)");
        std::strcpy(exprUz_, "0");
      }
    }
    if (exprStatus_[0] != '\0') {
      if (exprCompileError_ != 0) {
        ImGui::TextColored(ImVec4(1.0f, 0.45f, 0.35f, 1.0f), "%s", exprStatus_);
      } else {
        ImGui::TextUnformatted(exprStatus_);
      }
    }
    if (ImGui::SliderInt("grid N", &gridN_, 6, 24)) {
      dirty_ = true;
    }
    if (ImGui::SliderFloat("half-box size", &box_, 0.5f, 4.0f)) {
      dirty_ = true;
    }
    ImGui::TextUnformatted("Preset picker (for \"Load preset into editors\" only):");
    const char* presets[] = {"(y, x, 0)", "(z, a, -x)", "(sin y, cos x, 0)"};
    ImGui::Combo("##presetcopy", &fieldPreset_, presets, 3);
    if (fieldPreset_ == 1) {
      ImGui::SliderFloat("parameter a##pcopy", &paramA_, -2.0f, 2.0f);
    }
    uint64_t h = hashExprTriplet(exprUx_, exprUy_, exprUz_);
    if (h != customExprHash_) {
      customExprHash_ = h;
      rebuildField();
      rebuildStreamlines();
      dirty_ = false;
    }
  }

  const char* modes[] = {"speed |u|", "divergence", "vorticity |curl u|"};
  int cm = static_cast<int>(colorMode_);
  if (ImGui::Combo("colour mode", &cm, modes, 3)) {
    colorMode_ = static_cast<VectorColorMode>(cm);
  }
  ImGui::SliderFloat("arrow scale", &arrowScale_, 0.05f, 1.2f);
  ImGui::Separator();
  if (ImGui::Checkbox("streamlines (RK4)", &showStreamlines_)) {
    dirty_ = true;
  }
  if (ImGui::SliderInt("seeds / axis", &seedsPerAxis_, 2, 9)) {
    dirty_ = true;
  }
  if (ImGui::SliderFloat("RK step", &slStep_, 0.02f, 0.15f)) {
    dirty_ = true;
  }
  if (ImGui::SliderInt("max steps", &slMaxSteps_, 40, 400)) {
    dirty_ = true;
  }
  if (ImGui::Button("rebuild streamlines")) {
    rebuildField();
    rebuildStreamlines();
  }
}
