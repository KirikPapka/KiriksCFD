#pragma once

#include "cases/ICase.hpp"

#include <cstdint>

class PipeFlowCase final : public ICase {
public:
  PipeFlowCase();
  ~PipeFlowCase() override;

  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "Pipe Poiseuille"; }
  const char* description() const override {
    return "Steady pressure-driven flow in a straight circular pipe with no slip on the wall. The axial "
           "speed is largest on the axis and falls to zero at the radius in a parabolic profile - this is "
           "the classic Hagen-Poiseuille result. Colour shows how fast the fluid moves; the optional slice "
           "shows the same profile in cross-section. Custom: u_z(r) with r, R, umax.";
  }

  static float evalProfile(float r, float R, void* user);

private:
  void tryCompileCustom();

  float radius_ = 1.0f;
  float length_ = 3.0f;
  float uMax_ = 1.0f;
  bool showSlice_ = false;
  int radialSegs_ = 10;
  int thetaSegs_ = 24;

  int fieldMode_ = 0;
  char exprU_[256] = "umax*(1-(r/R)*(r/R))";
  void* teU_ = nullptr;
  double teR_ = 0;
  double teRpipe_ = 0;
  double teUmaxBind_ = 0;
  uint64_t exprHash_ = 0;
  char exprStatus_[160] = "";
};
