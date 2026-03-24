#pragma once

#include "cases/ICase.hpp"

#include <cstdint>
#include <vector>

class InertiaGravityWaveCase final : public ICase {
public:
  InertiaGravityWaveCase();
  ~InertiaGravityWaveCase() override;

  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "Inertia-gravity waves (f-plane)"; }
  const char* description() const override {
    return "Small disturbances on a rotating shallow layer behave as inertia-gravity waves: rotation adds "
           "the Coriolis frequency f, gravity and depth give restoring forces through gH. The dispersion "
           "relation links frequency to total wavenumber; phase speed and group speed need not match. Ld = "
           "sqrt(gH)/|f| is the natural horizontal scale (deformation radius) that appears in rotating "
           "shallow-water theory. Custom: eta(x,z,t) with omega = sqrt(f0^2 + g H (kx^2+ky^2)) from sliders.";
  }

private:
  void tryCompileCustom();

  float f0_ = 1.0f;
  float g_ = 9.81f;
  float H_ = 1.0f;
  float kx_ = 1.0f;
  float ky_ = 0.5f;
  float time_ = 0.0f;
  float timeScale_ = 1.0f;

  int fieldMode_ = 0;
  char exprEta_[256] = "0.15*cos(kx*x+ky*z-omega*t)";
  void* teEta_ = nullptr;
  double teX_ = 0;
  double teZ_ = 0;
  double teT_ = 0;
  double teKx_ = 0;
  double teKy_ = 0;
  double teF0_ = 0;
  double teG_ = 0;
  double teH_ = 0;
  double teOmega_ = 0;
  uint64_t exprHash_ = 0;
  char exprStatus_[160] = "";

  int gridN_ = 14;
  float halfL_ = 2.5f;
  std::vector<float> eta_;
};
