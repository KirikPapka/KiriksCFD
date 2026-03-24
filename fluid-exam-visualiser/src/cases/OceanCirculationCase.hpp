#pragma once

#include "cases/ICase.hpp"

class OceanCirculationCase final : public ICase {
public:
  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "Ocean gyre (Sverdrup sketch)"; }
  const char* description() const override {
    return "Wind-driven ocean circulation in a basin is often explained with Sverdrup interior flow plus a "
           "narrow fast western boundary current (Stommel/Munk style physics on exams). This sketch shows "
           "a sinusoidal gyre streamfunction with an exponential western intensification and horizontal "
           "velocity arrows derived from its derivatives - illustrative, not a full Munk solution.";
  }

private:
  float Lx_ = 3.0f;
  float Lz_ = 2.0f;
  float wbW_ = 0.22f;
  float wbcAmp_ = 0.45f;
  float time_ = 0.0f;
};
