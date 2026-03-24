#pragma once

#include "cases/ICase.hpp"

class VorticityEquationCase final : public ICase {
public:
  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "Vorticity equation sandbox"; }
  const char* description() const override {
    return "The vorticity equation says how local rotation of the fluid changes: advection, tilting and "
           "stretching of vortex lines in 3D, and viscous diffusion. This panel is a conceptual sandbox - "
           "toggles and text remind you which terms exist and how they are named, not a numerical "
           "solution of the full equation.";
  }

private:
  bool showStretch_ = true;
  bool showDiff_ = true;
  bool showAdv_ = true;
  float nu_ = 0.1f;
  float shear_ = 1.0f;
};
