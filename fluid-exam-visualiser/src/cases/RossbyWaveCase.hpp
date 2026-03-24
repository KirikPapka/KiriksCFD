#pragma once

#include "cases/ICase.hpp"

#include <cstdint>
#include <vector>

class RossbyWaveCase final : public ICase {
public:
  RossbyWaveCase();
  ~RossbyWaveCase() override;

  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "Rossby wave (beta-plane)"; }
  const char* description() const override {
    return "On a beta plane the Coriolis effect varies slightly with latitude, which allows large-scale "
           "rotating waves on the free surface (or streamfunction). Their dispersion ties zonal wavenumber "
           "to frequency; phase often propagates westward relative to the mean flow. The deformation "
           "radius Ld = sqrt(gH)/f sets how strongly the layer's gravity and rotation compete in the "
           "dispersion relation shown in the panel. Use Custom to type eta(x,y,t) directly.";
  }

private:
  void ensureGrid();
  void fillEtaBuiltin();
  void fillEtaCustom();
  void tryCompileCustom();
  void releaseTe();

  float time_ = 0.0f;
  float beta_ = 1.0f;
  float g_ = 9.81f;
  float H_ = 1.0f;
  float f0_ = 1.0f;
  float kx_ = 1.0f;
  float ky_ = 0.3f;
  float eta0_ = 0.2f;
  float L_ = 3.0f;
  int n_ = 36;
  float timeScale_ = 1.0f;

  int fieldMode_ = 0;
  char exprEta_[256] = "eta0*sin(kx*x+ky*y-t)";
  void* teEta_ = nullptr;
  double teX_ = 0;
  double teY_ = 0;
  double teT_ = 0;
  double teKx_ = 0;
  double teKy_ = 0;
  double teBeta_ = 0;
  double teG_ = 0;
  double teH_ = 0;
  double teF0_ = 0;
  double teEta0_ = 0;
  uint64_t exprHash_ = 0;
  char exprStatus_[160] = "";

  std::vector<float> eta_;
};
