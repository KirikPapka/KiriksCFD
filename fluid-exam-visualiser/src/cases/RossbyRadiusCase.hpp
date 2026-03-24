#pragma once

#include "cases/ICase.hpp"

class RossbyRadiusCase final : public ICase {
public:
  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "Rossby deformation radius"; }
  const char* description() const override {
    return "The Rossby deformation radius Ld = sqrt(gH)/f compares the distance gravity can 'feel' the "
           "surface in one inertial period to the scale of the motion. When wavelengths are long compared "
           "to Ld, wave dynamics differ from when they are short - the dispersion relation changes character. "
           "The numbers in the panel contrast a short and a long zonal wavenumber side by side.";
  }

private:
  float g_ = 9.81f;
  float H_ = 1.0f;
  float f0_ = 1.0f;
  float kShort_ = 3.0f;
  float kLong_ = 0.3f;
};
