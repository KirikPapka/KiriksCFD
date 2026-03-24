#pragma once

#include "cases/ICase.hpp"

class BarotropicInstabilityCase final : public ICase {
public:
  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "Barotropic shear instability"; }
  const char* description() const override {
    return "A zonal jet with lateral shear can be unstable to meandering (barotropic instability). "
           "Rayleigh-type criteria motivate why certain profiles break up. Here a sech^2 jet is drawn with "
           "a growing sinusoidal displacement of streamlines in the lateral direction - a qualitative "
           "cartoon, not a normal-mode eigenvalue solve.";
  }

private:
  float U0_ = 1.2f;
  float jetW_ = 0.45f;
  float k_ = 2.2f;
  float growth_ = 0.35f;
  float time_ = 0.0f;
  float timeScale_ = 1.0f;
};
