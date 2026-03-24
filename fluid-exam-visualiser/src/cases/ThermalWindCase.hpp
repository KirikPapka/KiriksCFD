#pragma once

#include "cases/ICase.hpp"

#include <cstdint>

class ThermalWindCase final : public ICase {
public:
  ThermalWindCase();
  ~ThermalWindCase() override;

  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "Thermal wind"; }
  const char* description() const override {
    return "Schematic thermal wind shear: du/dz = - (g / (f T0)) * dT/dy (northern-hemisphere style "
           "scaling). Custom: u(z) with variables z, K (thermal-wind coefficient from sliders).";
  }

private:
  void tryCompileCustom();

  float f_ = 1e-4f;
  float dTdy_ = 2.0f;
  float T0_ = 280.0f;
  float scale_ = 8.0f;
  float zMax_ = 4.0f;
  int nZ_ = 16;

  int fieldMode_ = 0;
  char exprU_[256] = "K*z";
  void* teU_ = nullptr;
  double teZ_ = 0;
  double teK_ = 0;
  uint64_t exprHash_ = 0;
  char exprStatus_[160] = "";
};
