#pragma once

#include "cases/ICase.hpp"

#include <cstdint>

class CouetteFlowCase final : public ICase {
public:
  CouetteFlowCase();
  ~CouetteFlowCase() override;

  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "Couette flow"; }
  const char* description() const override {
    return "Plane Couette: bottom plate fixed, top plate speed U along +x, u_x(z) = U z/h, Q = U h / 2 per "
           "unit width. Custom: type u(z) for the horizontal arrow length (linear profile default).";
  }

private:
  void tryCompileCustom();

  float h_ = 1.0f;
  float U_ = 1.0f;
  float plateW_ = 3.0f;
  float plateD_ = 2.0f;

  int fieldMode_ = 0;
  char exprU_[256] = "U*z/h";
  void* teU_ = nullptr;
  double teZ_ = 0;
  double teH_ = 0;
  double teUcap_ = 0;
  uint64_t exprHash_ = 0;
  char exprStatus_[160] = "";
};
