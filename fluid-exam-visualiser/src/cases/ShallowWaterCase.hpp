#pragma once

#include "cases/ICase.hpp"

#include <cstdint>
#include <vector>

class ShallowWaterCase final : public ICase {
public:
  ShallowWaterCase();
  ~ShallowWaterCase() override;

  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "Shallow water surface"; }
  const char* description() const override {
    return "The free surface of a thin layer of fluid is drawn as a height field eta above a flat plane. "
           "A simple travelling wave pattern moves in time so you can see phase propagation. The arrows "
           "sketch horizontal motion tied to the wave - useful for connecting surface shape to velocity in "
           "shallow-water thinking. Custom mode: type eta(x,y,t); arrows follow the horizontal gradient.";
  }

private:
  void rebuildHeightsBuiltin();
  void rebuildHeightsCustom();
  void tryCompileCustom();
  void releaseTe();

  float time_ = 0.0f;
  float amplitude_ = 0.35f;
  float wavelength_ = 2.0f;
  float phaseSpeed_ = 1.0f;
  float timeScale_ = 1.0f;
  float domainHalf_ = 2.5f;
  int gridN_ = 32;
  float velScale_ = 0.4f;
  int arrowStride_ = 3;

  int fieldMode_ = 0;
  char exprEta_[256] = "A*sin(k*(x-c*t))*cos(0.35*k*y)";
  void* teEta_ = nullptr;
  double teX_ = 0;
  double teY_ = 0;
  double teT_ = 0;
  double teA_ = 0;
  double teK_ = 0;
  double teC_ = 0;
  uint64_t exprHash_ = 0;
  char exprStatus_[160] = "";

  std::vector<float> heights_;
};
