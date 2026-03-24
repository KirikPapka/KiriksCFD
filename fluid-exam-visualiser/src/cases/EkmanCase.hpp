#pragma once

#include "cases/ICase.hpp"

#include <cstdint>

class EkmanCase final : public ICase {
public:
  EkmanCase();
  ~EkmanCase() override;

  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "Ekman spiral"; }
  const char* description() const override {
    return "Near a boundary, friction and rotation combine so that horizontal velocity turns and weakens "
           "with height or depth - this is the idea of an Ekman layer. Here horizontal vectors are stacked "
           "vertically: you see a spiral-like change of direction and an exponential-style decay. The "
           "sliders change how fast the flow dies off and how tightly it turns - schematic, not a full "
           "boundary-layer solve. Custom: u(z), v(z) for the horizontal arrow components.";
  }

private:
  void tryCompileCustom();

  float decayScale_ = 1.0f;
  float rotationFactor_ = 1.0f;
  float u0_ = 1.0f;
  int nLevels_ = 24;
  float zMax_ = 4.0f;
  int nColumns_ = 3;
  float columnSpacing_ = 1.2f;

  int fieldMode_ = 0;
  char exprU_[256] = "u0*exp(-z/d)*cos(f*z/d)";
  char exprV_[256] = "u0*exp(-z/d)*sin(f*z/d)";
  void* teU_ = nullptr;
  void* teV_ = nullptr;
  double teZ_ = 0;
  double teD_ = 0;
  double teF_ = 0;
  double teU0_ = 0;
  uint64_t exprHash_ = 0;
  char exprStatus_[160] = "";
};
