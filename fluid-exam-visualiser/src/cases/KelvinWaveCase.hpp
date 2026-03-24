#pragma once

#include "cases/ICase.hpp"

#include <vector>

class KelvinWaveCase final : public ICase {
public:
  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "Kelvin waves"; }
  const char* description() const override {
    return "Kelvin waves are trapped to a vertical boundary (coastal) or to the equator. In shallow water "
           "on an f-plane, free-surface height decays exponentially away from the wall while propagating "
           "along it. Toggle switches decay scale between mid-latitude (R_d = sqrt(gH)/|f|) and a wider "
           "equatorial-style scale for illustration.";
  }

private:
  int mode_ = 0;
  float H_ = 1.0f;
  float g_ = 9.81f;
  float f_ = 1.0e-4f;
  float k_ = 0.8f;
  float A_ = 0.12f;
  float timeScale_ = 1.0f;
  float time_ = 0.0f;
  int gridN_ = 48;
  float yMax_ = 3.0f;
  float xHalf_ = 3.0f;
  std::vector<float> heights_;
};
