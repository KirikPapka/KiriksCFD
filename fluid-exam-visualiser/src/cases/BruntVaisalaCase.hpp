#pragma once

#include "cases/ICase.hpp"

#include <vector>

#include "raylib.h"

class BruntVaisalaCase final : public ICase {
public:
  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "Brunt-Vaisala oscillation"; }
  const char* description() const override {
    return "In a stably stratified fluid, a displaced parcel feels a restoring buoyancy force and oscillates "
           "with the Brunt-Vaisala frequency N. Larger N means stiffer stratification and faster vertical "
           "oscillations. The bouncing sphere is a literal visualization of that harmonic motion; the "
           "period is 2*pi/N when N is in radians per second.";
  }

private:
  float N_ = 1.2f;
  float A_ = 0.6f;
  float z0_ = 2.0f;
  float time_ = 0.0f;
  float timeScale_ = 1.0f;
  std::vector<Vector3> trail_;
  float trailZ0_ = 2.0f;
  float trailA_ = 0.6f;
  static constexpr size_t kMaxTrail = 220;
};
