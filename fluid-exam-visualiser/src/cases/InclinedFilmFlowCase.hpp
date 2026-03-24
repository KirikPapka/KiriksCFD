#pragma once

#include "cases/ICase.hpp"

class InclinedFilmFlowCase final : public ICase {
public:
  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "Inclined film flow"; }
  const char* description() const override {
    return "A thin layer of viscous fluid runs down an inclined plane. At the wall the fluid sticks "
           "(no slip); at the free surface there is no shear stress. The result is a parabolic profile "
           "across the film thickness, driven by gravity along the slope. The visualization emphasizes how "
           "speed grows from the wall toward the interior and the formula for total flow rate per unit "
           "width.";
  }

private:
  float h_ = 1.0f;
  float rho_ = 1.0f;
  float mu_ = 0.1f;
  float sinAlpha_ = 0.5f;
  float g_ = 9.81f;
  float wallW_ = 3.0f;
};
