#pragma once

#include "cases/ICase.hpp"

#include <vector>

class AtmosphericColumnCase final : public ICase {
public:
  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "Atmospheric column"; }
  const char* description() const override {
    return "A vertical column of air in hydrostatic balance links pressure and density (or temperature) to "
           "height. In an isothermal idealization, pressure decays exponentially with a scale height. With a "
           "dry adiabatic lapse rate, temperature falls linearly with height and pressure follows the "
           "usual power-law form from combining hydrostatics and the adiabat. The plots show p and T vs z "
           "for each model.";
  }

private:
  int mode_ = 0;
  float T0_ = 288.0f;
  float p0_ = 101325.0f;
  float Hscale_ = 8400.0f;
  float Gamma_ = 0.0098f;
  int nZ_ = 128;
  float zTop_ = 15000.0f;
  std::vector<float> zPlot_;
  std::vector<float> pPlot_;
  std::vector<float> tPlot_;
  void recomputeProfiles();
};
