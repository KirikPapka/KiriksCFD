#pragma once

#include "cases/ICase.hpp"

#include <vector>

class QGPVCase final : public ICase {
public:
  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "QG potential vorticity (toy)"; }
  const char* description() const override {
    return "Quasi-geostrophic dynamics conserves potential vorticity built from vorticity, stretching, and "
           "planetary beta. This toy shows two stacked horizontal streamfunction patterns and a crude PV "
           "anomaly colour (Laplacian of psi plus a beta*y term). It is schematic, not a full multi-layer QG "
           "solver.";
  }

private:
  float k_ = 1.2f;
  float coupling_ = 0.35f;
  int layers_ = 2;
  float time_ = 0.0f;
  int nx_ = 32;
  float halfL_ = 2.0f;
  std::vector<float> psi0_;
  std::vector<float> psi1_;
};
