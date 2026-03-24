#pragma once

#include "cases/ICase.hpp"

class RotatingForceBalanceCase final : public ICase {
public:
  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "Rotating-frame forces"; }
  const char* description() const override {
    return "In a rotating frame, momentum balance involves pressure gradient, Coriolis, material "
           "acceleration, and often friction. Dimensionless numbers like Rossby and Reynolds tell you "
           "which terms dominate in a given flow. The arrows are not a force polygon for a real point - "
           "they are scaled cartoons to remind you how Ro and Re change the relative size of effects.";
  }

private:
  float Ro_ = 0.3f;
  float Re_ = 100.0f;
};
