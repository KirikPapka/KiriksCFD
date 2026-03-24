#pragma once

#include "cases/ICase.hpp"

class SphericalFlowCase final : public ICase {
public:
  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "Spherical-coordinate flow"; }
  const char* description() const override {
    return "This flow is purely in the theta direction on a sphere, with amplitude sin(theta)/r - exactly "
           "the kind of field used to test spherical coordinates on exams. It lets you think about unit "
           "vectors, whether the field is incompressible, what its curl looks like, and how advection "
           "(u dot nabla)u picks up components in other directions even when u has only one component.";
  }

private:
  float r0_ = 1.2f;
  int nTheta_ = 10;
  int nPhi_ = 16;
};
