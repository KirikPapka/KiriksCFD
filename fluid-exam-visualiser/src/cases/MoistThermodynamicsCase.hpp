#pragma once

#include "cases/ICase.hpp"

class MoistThermodynamicsCase final : public ICase {
public:
  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "Moist thermodynamics (CAPE sketch)"; }
  const char* description() const override {
    return "Moist convection exams use CAPE (positive buoyancy area between a lifted parcel and the "
           "environment) and CIN (negative area below the level of free convection). This panel gives "
           "sliders for a toy sounding and plots a cartoon T(z) vs environment; 3D shows a column and a "
           "shaded buoyancy slab - not a full tephigram digitisation.";
  }

private:
  float T0_ = 300.0f;
  float GammaEnv_ = 0.007f;
  float GammaParcel_ = 0.006f;
  float zTop_ = 12000.0f;
  float liftKm_ = 1.0f;
};
