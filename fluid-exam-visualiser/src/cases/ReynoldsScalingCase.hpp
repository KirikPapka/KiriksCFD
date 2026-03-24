#pragma once

#include "cases/ICase.hpp"

class ReynoldsScalingCase final : public ICase {
public:
  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "Reynolds number scaling"; }
  const char* description() const override {
    return "Re = U L / nu orders which terms matter in the Navier-Stokes equations. At low Re viscous "
           "smoothing dominates; at high Re thin boundary layers and inertial cores appear. Sliders update "
           "Re and the 3D sketch cross-section morphs between diffuse, transitional, and thin-layer shapes.";
  }

private:
  float U_ = 1.0f;
  float L_ = 1.0f;
  float nu_ = 0.08f;
};
