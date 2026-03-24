#pragma once

#include "cases/ICase.hpp"

#include <cstdint>
#include <vector>

class GeostrophicBalanceCase final : public ICase {
public:
  GeostrophicBalanceCase();
  ~GeostrophicBalanceCase() override;

  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "Geostrophic balance"; }
  const char* description() const override {
    return "Geostrophic balance is the leading-order balance in rotating shallow water (or atmosphere): "
           "the Coriolis force balances the pressure gradient from the free surface (or height field). "
           "Flow runs along contours of surface height, with speed related to the gradient and Coriolis "
           "parameter. The hill-shaped eta and the horizontal velocity arrows illustrate that relation. "
           "Custom: type eta(x,z) on the xz plane.";
  }

private:
  void rebuildEtaBuiltin();
  void rebuildEtaCustom();
  void tryCompileCustom();
  void releaseTe();

  float f_ = 1.0f;
  float g_ = 9.81f;
  float sigma_ = 1.0f;
  float eta0_ = 0.25f;
  int n_ = 36;
  float L_ = 3.0f;

  int fieldMode_ = 0;
  char exprEta_[256] = "eta0*exp(-(x*x+z*z)/(2*sigma*sigma))";
  void* teEta_ = nullptr;
  double teX_ = 0;
  double teZ_ = 0;
  double teEta0_ = 0;
  double teSigma_ = 0;
  double teF_ = 0;
  double teG_ = 0;
  uint64_t exprHash_ = 0;
  char exprStatus_[160] = "";

  std::vector<float> eta_;
};
