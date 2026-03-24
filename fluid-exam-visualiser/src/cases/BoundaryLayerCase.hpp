#pragma once

#include "cases/ICase.hpp"

class BoundaryLayerCase final : public ICase {
public:
  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "Blasius boundary layer"; }
  const char* description() const override {
    return "A thin viscous layer grows along a flat plate: outer uniform flow U meets the no-slip wall. "
           "Pohlhausen-type profiles illustrate u(y) at several x with thickness scaling like x/sqrt(Re_x). "
           "Wall colour indicates relative wall shear (qualitative). This is a schematic, not a PDE solve.";
  }

private:
  float U_ = 1.0f;
  float nu_ = 0.02f;
  float xPlate_ = 4.0f;
  float zPlate_ = 1.2f;
  int profileCount_ = 5;
};
