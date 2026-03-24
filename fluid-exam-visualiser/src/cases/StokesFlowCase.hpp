#pragma once

#include "cases/ICase.hpp"

class StokesFlowCase final : public ICase {
public:
  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "Stokes flow (creeping past sphere)"; }
  const char* description() const override {
    return "At very low Reynolds number, inertia is negligible and viscous stresses balance pressure "
           "gradients (Stokes equations). Flow past a sphere becomes fore-aft symmetric in the Stokes "
           "approximation; drag scales as 6 pi mu a U. Arrows use the classical Stokes solution outside "
           "the sphere; toggle compares to a crude inertial (potential-flow-style) reminder at higher Re.";
  }

private:
  float U_ = 1.0f;
  float a_ = 0.35f;
  float nu_ = 0.5f;
  bool showCompare_ = true;
};
