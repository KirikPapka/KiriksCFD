#pragma once

#include "cases/ICase.hpp"
#include "math/Derivatives.hpp"
#include "math/VecField3D.hpp"

class HelicityCase final : public ICase {
public:
  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "Helicity density"; }
  const char* description() const override {
    return "Helicity measures how much the velocity aligns with the local vorticity: h = u dot omega. "
           "Where it is large, streamline direction and spin axis are related - important in questions about "
           "topology of vortex lines and conservation in ideal flow. Coloured arrows show u; optional "
           "smaller arrows show omega so you can compare directions by eye.";
  }

private:
  void rebuild();

  VecField3D u_;
  VecField3D w_;
  ScalarField3D hel_;
  int gridN_ = 10;
  float box_ = 1.8f;
  int preset_ = 1;
  float paramA_ = 0.5f;
  bool showOmega_ = true;
  bool dirty_ = true;
};
