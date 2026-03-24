#include "cases/CoordinateOperatorsCase.hpp"

#include "cases/TinyExprHelpers.hpp"

#include <algorithm>

#include "render/ArrowRenderer.hpp"

#include "imgui.h"
#include "raylib.h"
#include "raymath.h"
#include "tinyexpr.h"

#include <cmath>
#include <cstdio>
#include <cstring>

namespace {

constexpr double kDiffH = 1e-4;

}  // namespace

CoordinateOperatorsCase::CoordinateOperatorsCase() {
  std::strncpy(exprPhi_, "x*x+y*y+z*z", sizeof(exprPhi_) - 1);
  exprPhi_[sizeof(exprPhi_) - 1] = '\0';
  std::strncpy(exprFx_, "y", sizeof(exprFx_) - 1);
  exprFx_[sizeof(exprFx_) - 1] = '\0';
  std::strncpy(exprFy_, "-x", sizeof(exprFy_) - 1);
  exprFy_[sizeof(exprFy_) - 1] = '\0';
  std::strncpy(exprFz_, "0", sizeof(exprFz_) - 1);
  exprFz_[sizeof(exprFz_) - 1] = '\0';
  tryCompileCustomScalar();
  tryCompileCustomVector();
}

CoordinateOperatorsCase::~CoordinateOperatorsCase() {
  teutil::freeExpr(tePhi_);
  teutil::freeExpr(teFx_);
  teutil::freeExpr(teFy_);
  teutil::freeExpr(teFz_);
}

void CoordinateOperatorsCase::tryCompileCustomScalar() {
  te_variable vars[] = {{"x", &teX_, TE_VARIABLE, nullptr},
                        {"y", &teY_, TE_VARIABLE, nullptr},
                        {"z", &teZ_, TE_VARIABLE, nullptr}};
  void* tmp = nullptr;
  int col = 0;
  if (!teutil::compileOne(exprPhi_, vars, 3, &tmp, &col)) {
    std::snprintf(phiStatus_, sizeof phiStatus_, "phi: parse error near column %d", col);
    teutil::freeExpr(tmp);
    teutil::freeExpr(tePhi_);
    tePhi_ = nullptr;
    return;
  }
  teutil::freeExpr(tePhi_);
  tePhi_ = tmp;
  std::strncpy(phiStatus_, "phi: OK (numeric grad / lap below)", sizeof(phiStatus_) - 1);
  phiStatus_[sizeof(phiStatus_) - 1] = '\0';
}

void CoordinateOperatorsCase::tryCompileCustomVector() {
  te_variable vars[] = {{"x", &teX_, TE_VARIABLE, nullptr},
                        {"y", &teY_, TE_VARIABLE, nullptr},
                        {"z", &teZ_, TE_VARIABLE, nullptr}};
  auto compileSlot = [&](const char* src, void** slot, const char* label, int* errCol) -> bool {
    void* t = nullptr;
    int col = 0;
    if (!teutil::compileOne(src, vars, 3, &t, &col)) {
      *errCol = col;
      teutil::freeExpr(t);
      teutil::freeExpr(*slot);
      *slot = nullptr;
      std::snprintf(vecStatus_, sizeof vecStatus_, "%s: error near column %d", label, col);
      return false;
    }
    teutil::freeExpr(*slot);
    *slot = t;
    return true;
  };
  int col = 0;
  if (!compileSlot(exprFx_, &teFx_, "Fx", &col)) {
    return;
  }
  if (!compileSlot(exprFy_, &teFy_, "Fy", &col)) {
    return;
  }
  if (!compileSlot(exprFz_, &teFz_, "Fz", &col)) {
    return;
  }
  std::strncpy(vecStatus_, "F: OK (numeric div / curl below)", sizeof(vecStatus_) - 1);
  vecStatus_[sizeof(vecStatus_) - 1] = '\0';
}

void CoordinateOperatorsCase::numericGradPhi(float x, float y, float z, float* gx, float* gy,
                                             float* gz) {
  *gx = *gy = *gz = 0.0f;
  if (tePhi_ == nullptr) {
    return;
  }
  const double xd = x, yd = y, zd = z;
  const double h = kDiffH;
  auto phi = [this](double px, double py, double pz) {
    teX_ = px;
    teY_ = py;
    teZ_ = pz;
    return te_eval(static_cast<te_expr*>(tePhi_));
  };
  double c = phi(xd, yd, zd);
  if (!std::isfinite(c)) {
    return;
  }
  double pxp = phi(xd + h, yd, zd), pxm = phi(xd - h, yd, zd);
  double pyp = phi(xd, yd + h, zd), pym = phi(xd, yd - h, zd);
  double pzp = phi(xd, yd, zd + h), pzm = phi(xd, yd, zd - h);
  if (!std::isfinite(pxp) || !std::isfinite(pxm) || !std::isfinite(pyp) || !std::isfinite(pym) ||
      !std::isfinite(pzp) || !std::isfinite(pzm)) {
    return;
  }
  *gx = static_cast<float>((pxp - pxm) / (2.0 * h));
  *gy = static_cast<float>((pyp - pym) / (2.0 * h));
  *gz = static_cast<float>((pzp - pzm) / (2.0 * h));
}

float CoordinateOperatorsCase::numericLapPhi(float x, float y, float z) {
  if (tePhi_ == nullptr) {
    return 0.0f;
  }
  const double xd = x, yd = y, zd = z;
  const double h = kDiffH;
  auto phi = [this](double px, double py, double pz) {
    teX_ = px;
    teY_ = py;
    teZ_ = pz;
    return te_eval(static_cast<te_expr*>(tePhi_));
  };
  double c = phi(xd, yd, zd);
  if (!std::isfinite(c)) {
    return 0.0f;
  }
  double d2x = (phi(xd + h, yd, zd) - 2.0 * c + phi(xd - h, yd, zd)) / (h * h);
  double d2y = (phi(xd, yd + h, zd) - 2.0 * c + phi(xd, yd - h, zd)) / (h * h);
  double d2z = (phi(xd, yd, zd + h) - 2.0 * c + phi(xd, yd, zd - h)) / (h * h);
  if (!std::isfinite(d2x) || !std::isfinite(d2y) || !std::isfinite(d2z)) {
    return 0.0f;
  }
  return static_cast<float>(d2x + d2y + d2z);
}

bool CoordinateOperatorsCase::evalCustomF(double x, double y, double z, double* ox, double* oy,
                                          double* oz) {
  if (teFx_ == nullptr || teFy_ == nullptr || teFz_ == nullptr) {
    return false;
  }
  teX_ = x;
  teY_ = y;
  teZ_ = z;
  double vx = te_eval(static_cast<te_expr*>(teFx_));
  double vy = te_eval(static_cast<te_expr*>(teFy_));
  double vz = te_eval(static_cast<te_expr*>(teFz_));
  if (!std::isfinite(vx) || !std::isfinite(vy) || !std::isfinite(vz)) {
    return false;
  }
  *ox = vx;
  *oy = vy;
  *oz = vz;
  return true;
}

void CoordinateOperatorsCase::numericDivF(float x, float y, float z, float* div) {
  *div = 0.0f;
  if (teFx_ == nullptr || teFy_ == nullptr || teFz_ == nullptr) {
    return;
  }
  const double xd = x, yd = y, zd = z;
  const double h = kDiffH;
  auto evalX = [this](double px, double py, double pz) {
    teX_ = px;
    teY_ = py;
    teZ_ = pz;
    return te_eval(static_cast<te_expr*>(teFx_));
  };
  auto evalY = [this](double px, double py, double pz) {
    teX_ = px;
    teY_ = py;
    teZ_ = pz;
    return te_eval(static_cast<te_expr*>(teFy_));
  };
  auto evalZ = [this](double px, double py, double pz) {
    teX_ = px;
    teY_ = py;
    teZ_ = pz;
    return te_eval(static_cast<te_expr*>(teFz_));
  };
  double dFxdx = (evalX(xd + h, yd, zd) - evalX(xd - h, yd, zd)) / (2.0 * h);
  double dFdydy = (evalY(xd, yd + h, zd) - evalY(xd, yd - h, zd)) / (2.0 * h);
  double dFzdz = (evalZ(xd, yd, zd + h) - evalZ(xd, yd, zd - h)) / (2.0 * h);
  if (!std::isfinite(dFxdx) || !std::isfinite(dFdydy) || !std::isfinite(dFzdz)) {
    return;
  }
  *div = static_cast<float>(dFxdx + dFdydy + dFzdz);
}

void CoordinateOperatorsCase::numericCurlF(float x, float y, float z, float* cx, float* cy,
                                             float* cz) {
  *cx = *cy = *cz = 0.0f;
  if (teFx_ == nullptr || teFy_ == nullptr || teFz_ == nullptr) {
    return;
  }
  const double xd = x, yd = y, zd = z;
  const double h = kDiffH;
  auto Fx = [this](double px, double py, double pz) {
    teX_ = px;
    teY_ = py;
    teZ_ = pz;
    return te_eval(static_cast<te_expr*>(teFx_));
  };
  auto Fy = [this](double px, double py, double pz) {
    teX_ = px;
    teY_ = py;
    teZ_ = pz;
    return te_eval(static_cast<te_expr*>(teFy_));
  };
  auto Fz = [this](double px, double py, double pz) {
    teX_ = px;
    teY_ = py;
    teZ_ = pz;
    return te_eval(static_cast<te_expr*>(teFz_));
  };
  double dFz_dy = (Fz(xd, yd + h, zd) - Fz(xd, yd - h, zd)) / (2.0 * h);
  double dFy_dz = (Fy(xd, yd, zd + h) - Fy(xd, yd, zd - h)) / (2.0 * h);
  double dFx_dz = (Fx(xd, yd, zd + h) - Fx(xd, yd, zd - h)) / (2.0 * h);
  double dFz_dx = (Fz(xd + h, yd, zd) - Fz(xd - h, yd, zd)) / (2.0 * h);
  double dFy_dx = (Fy(xd + h, yd, zd) - Fy(xd - h, yd, zd)) / (2.0 * h);
  double dFx_dy = (Fx(xd, yd + h, zd) - Fx(xd, yd - h, zd)) / (2.0 * h);
  if (!std::isfinite(dFz_dy) || !std::isfinite(dFy_dz) || !std::isfinite(dFx_dz) ||
      !std::isfinite(dFz_dx) || !std::isfinite(dFy_dx) || !std::isfinite(dFx_dy)) {
    return;
  }
  *cx = static_cast<float>(dFz_dy - dFy_dz);
  *cy = static_cast<float>(dFx_dz - dFz_dx);
  *cz = static_cast<float>(dFy_dx - dFx_dy);
}

void CoordinateOperatorsCase::update(float /*dt*/) {}

void CoordinateOperatorsCase::draw3D() {
  float x = xr_, y = yr_, z = zr_;
  DrawLine3D({0, 0, 0}, {x, y, z}, {140, 140, 160, 200});
  DrawSphere({x, y, z}, 0.08f, {255, 200, 100, 255});

  Vector3 F{0, 0, 0};
  if (field_ == 1) {
    F = {vecFy_ * y, -vecFx_ * x, 0.0f};
  } else if (field_ == 3) {
    double fx, fy, fz;
    if (evalCustomF(x, y, z, &fx, &fy, &fz)) {
      F = {static_cast<float>(fx), static_cast<float>(fy), static_cast<float>(fz)};
    }
  }

  if (field_ == 1 || field_ == 3) {
    float fmag = Vector3Length(F);
    if (fmag > 1e-6f) {
      float shaft = std::clamp(0.35f * fmag, 0.2f, 1.0f);
      DrawArrow3D({0, 0, 0}, F, shaft, 0.07f, 0.024f, {150, 200, 255, 220});
    }
  }

  if (field_ == 0 && op_ == 0) {
    Vector3 g{2.0f * quadA_ * x, 2.0f * quadB_ * y, 2.0f * quadC_ * z};
    float glen = Vector3Length(g);
    if (glen > 1e-6f) {
      float shaft = std::clamp(0.22f * glen, 0.15f, 1.15f);
      DrawArrow3D({x, y, z}, g, shaft, 0.08f, 0.026f, {255, 120, 120, 240});
    }
  } else if (field_ == 2 && op_ == 0 && tePhi_ != nullptr) {
    float gx, gy, gz;
    numericGradPhi(x, y, z, &gx, &gy, &gz);
    Vector3 g{gx, gy, gz};
    float glen = Vector3Length(g);
    if (glen > 1e-6f) {
      float shaft = std::clamp(0.22f * glen, 0.15f, 1.15f);
      DrawArrow3D({x, y, z}, g, shaft, 0.08f, 0.026f, {255, 120, 120, 240});
    }
  } else if (field_ == 1 && op_ == 2) {
    Vector3 curl{0.0f, 0.0f, -(vecFx_ + vecFy_)};
    float cz = std::fabs(curl.z);
    if (cz > 1e-6f) {
      float shaft = std::clamp(0.28f * cz, 0.15f, 1.0f);
      DrawArrow3D({x, y, z}, curl, shaft, 0.08f, 0.026f, {255, 200, 100, 240});
    }
  } else if (field_ == 3 && op_ == 2) {
    float cx, cy, cz;
    numericCurlF(x, y, z, &cx, &cy, &cz);
    Vector3 c{cx, cy, cz};
    float clen = Vector3Length(c);
    if (clen > 1e-6f) {
      float shaft = std::clamp(0.28f * clen, 0.15f, 1.0f);
      DrawArrow3D({x, y, z}, c, shaft, 0.08f, 0.026f, {255, 200, 100, 240});
    }
  }
}

void CoordinateOperatorsCase::drawUI() {
  float x = xr_, y = yr_, z = zr_;
  float r = std::sqrt(std::max(1e-8f, x * x + y * y));
  float R = std::sqrt(std::max(1e-8f, x * x + y * y + z * z));
  float theta = std::atan2(r, z);
  float phiAng = std::atan2(y, x);
  float cph = std::cos(phiAng);
  float sph = std::sin(phiAng);

  ImGui::TextUnformatted("Evaluate common operators on analytic test fields at (x,y,z).");
  const char* sys[] = {"Cartesian display", "also show cylindrical r,theta,z", "also show spherical R,theta,phi"};
  ImGui::Combo("system", &system_, sys, 3);
  const char* fld[] = {"phi = a x^2 + b y^2 + c z^2",
                       "F = (Fy y, -Fx x, 0)",
                       "custom phi(x,y,z)",
                       "custom F = (Fx,Fy,Fz)"};
  ImGui::Combo("field", &field_, fld, 4);
  const char* ops[] = {"grad phi", "div F", "curl F", "laplacian phi"};
  ImGui::Combo("operator", &op_, ops, 4);

  if (field_ == 0) {
    ImGui::SliderFloat("a (x^2)", &quadA_, -2.0f, 2.0f);
    ImGui::SliderFloat("b (y^2)", &quadB_, -2.0f, 2.0f);
    ImGui::SliderFloat("c (z^2)", &quadC_, -2.0f, 2.0f);
  } else if (field_ == 1) {
    ImGui::SliderFloat("Fx (-Fx x in F_y)", &vecFx_, -2.0f, 2.0f);
    ImGui::SliderFloat("Fy (Fy y in F_x)", &vecFy_, -2.0f, 2.0f);
  } else if (field_ == 2) {
    ImGui::TextUnformatted("Variables x, y, z. Example: x*x+y*y, sin(x)*z, sqrt(x*x+y*y+z*z)");
    bool edited = ImGui::InputText("phi(x,y,z)", exprPhi_, sizeof(exprPhi_));
    if (edited) {
      tryCompileCustomScalar();
    }
    if (ImGui::Button("Recompile phi")) {
      tryCompileCustomScalar();
    }
    ImGui::SameLine();
    ImGui::TextUnformatted(phiStatus_);
  } else if (field_ == 3) {
    ImGui::TextUnformatted("Components use x, y, z. Defaults reproduce (y,-x,0).");
    bool e = false;
    e |= ImGui::InputText("Fx", exprFx_, sizeof(exprFx_));
    e |= ImGui::InputText("Fy", exprFy_, sizeof(exprFy_));
    e |= ImGui::InputText("Fz", exprFz_, sizeof(exprFz_));
    if (e) {
      tryCompileCustomVector();
    }
    if (ImGui::Button("Recompile F")) {
      tryCompileCustomVector();
    }
    ImGui::SameLine();
    ImGui::TextUnformatted(vecStatus_);
  }

  ImGui::SliderFloat("x", &xr_, -2.0f, 2.0f);
  ImGui::SliderFloat("y", &yr_, -2.0f, 2.0f);
  ImGui::SliderFloat("z", &zr_, -2.0f, 2.0f);

  if (field_ == 0) {
    float gcx = 2.0f * quadA_ * x, gcy = 2.0f * quadB_ * y, gcz = 2.0f * quadC_ * z;
    float lap = 2.0f * (quadA_ + quadB_ + quadC_);
    if (op_ == 0) {
      ImGui::Text("grad phi (Cartesian) = (%.3f, %.3f, %.3f)", gcx, gcy, gcz);
    } else if (op_ == 1) {
      ImGui::Text("div F not selected (scalar field).");
    } else if (op_ == 2) {
      ImGui::Text("curl F not selected.");
    } else {
      ImGui::Text("nabla^2 phi = 2(a+b+c) = %.3f", lap);
    }
    if (system_ >= 1) {
      ImGui::Text("Cyl: r=%.3f phi=%.3f z=%.3f", r, phiAng, z);
      float dphidr = 2.0f * r * (quadA_ * cph * cph + quadB_ * sph * sph);
      ImGui::Text("grad_r phi = d phi/d r = %.3f", dphidr);
    }
    if (system_ >= 2) {
      ImGui::Text("Sph: R=%.3f theta=%.3f phi=%.3f", R, theta, phiAng);
      ImGui::TextUnformatted("grad_R phi = d phi/d R = 2 R (a sin^2 th cos^2 ph + b sin^2 th sin^2 ph + c cos^2 th)");
      float st = std::sin(theta);
      float ct = std::cos(theta);
      float dphidR =
          2.0f * R * (quadA_ * st * st * cph * cph + quadB_ * st * st * sph * sph + quadC_ * ct * ct);
      ImGui::Text("= %.3f (evaluated)", dphidR);
    }
  } else if (field_ == 1) {
    if (op_ == 0) {
      ImGui::Text("grad not defined for vector field pick.");
    } else if (op_ == 1) {
      ImGui::Text("div F = d/dx(Fy y) + d/dy(-Fx x) = 0");
    } else if (op_ == 2) {
      ImGui::Text("curl F = (0, 0, -(Fx + Fy)) = (0, 0, %.3f)", -(vecFx_ + vecFy_));
    } else {
      ImGui::Text("laplacian not selected.");
    }
    if (system_ >= 1) {
      ImGui::TextUnformatted("Cylindrical: linear in x,y -> combine with metric factors on your sheet.");
    }
  } else if (field_ == 2) {
    ImGui::TextUnformatted("Custom scalar: Cartesian numeric derivatives (central differences).");
    float gcx, gcy, gcz;
    numericGradPhi(x, y, z, &gcx, &gcy, &gcz);
    float lap = numericLapPhi(x, y, z);
    if (op_ == 0) {
      ImGui::Text("grad phi (numeric) ~ (%.4f, %.4f, %.4f)", gcx, gcy, gcz);
    } else if (op_ == 1) {
      ImGui::Text("div F not selected (scalar field).");
    } else if (op_ == 2) {
      ImGui::Text("curl F not selected.");
    } else {
      ImGui::Text("nabla^2 phi (numeric) ~ %.4f", lap);
    }
    if (system_ >= 1) {
      ImGui::TextUnformatted("Cyl/sph columns: use your chain rules from grad phi above.");
    }
  } else {
    ImGui::TextUnformatted("Custom vector: Cartesian numeric div / curl.");
    float dv, cx, cy, cz;
    numericDivF(x, y, z, &dv);
    numericCurlF(x, y, z, &cx, &cy, &cz);
    if (op_ == 0) {
      ImGui::Text("grad not defined for vector field pick.");
    } else if (op_ == 1) {
      ImGui::Text("div F (numeric) ~ %.4f", dv);
    } else if (op_ == 2) {
      ImGui::Text("curl F (numeric) ~ (%.4f, %.4f, %.4f)", cx, cy, cz);
    } else {
      ImGui::Text("laplacian not selected.");
    }
    if (system_ >= 1) {
      ImGui::TextUnformatted("Cylindrical: transform F and use metric div/curl on your sheet.");
    }
  }
}
