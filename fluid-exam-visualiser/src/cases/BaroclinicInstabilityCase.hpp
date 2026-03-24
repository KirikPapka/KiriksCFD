#pragma once

#include "cases/ICase.hpp"

class BaroclinicInstabilityCase final : public ICase {
public:
  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "Baroclinic / Eady wave"; }
  const char* description() const override {
    return "Baroclinic instability draws energy from horizontal temperature gradients via vertical shear "
           "(Eady model on an f-plane with rigid lids is the classic exam setup). Sloping isentropes and "
           "a vertically tilting wave illustrate the westward tilt with height; growth is a schematic "
           "exponential envelope, not a solved Eady eigenfunction.";
  }

private:
  float Lambda_ = 0.55f;
  float k_ = 1.8f;
  float tilt_ = 0.35f;
  float growth_ = 0.25f;
  float time_ = 0.0f;
  float timeScale_ = 1.0f;
};
