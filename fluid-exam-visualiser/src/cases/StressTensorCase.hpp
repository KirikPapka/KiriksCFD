#pragma once

#include "cases/ICase.hpp"

class StressTensorCase final : public ICase {
public:
  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "Stress tensor / traction"; }
  const char* description() const override {
    return "The Cauchy stress tensor maps a unit normal on a surface element to the traction (force per "
           "area) on that face. Toggle pressure vs deviatoric shear contributions on a small cube in a "
           "simple shear flow u = S y to see normal and tangential tractions - sign conventions as on a "
           "typical formula sheet.";
  }

private:
  int mode_ = 0;
  float p_ = 1.0f;
  float shearRate_ = 0.6f;
  float mu_ = 0.4f;
  float cube_ = 0.35f;
};
