#pragma once

#include "cases/ICase.hpp"
#include "math/VecField3D.hpp"
#include "raylib.h"

#include <vector>

class StreamlineGeometry3DCase final : public ICase {
public:
  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "3D streamline geometry (helical preset)"; }
  const char* description() const override {
    return "A fixed three-dimensional linear field u = (z, a, -x) appears in many vector-calculus exams. "
           "Changing the constant a deforms the flow: streamlines can look more helical or more flattened. "
           "The scene draws both arrows and integrated streamlines so you connect the formula to geometry "
           "and to ideas like divergence and curl of a simple polynomial field.";
  }

private:
  void rebuild();

  VecField3D field_;
  float paramA_ = 0.5f;
  int gridN_ = 10;
  float box_ = 2.0f;
  float slStep_ = 0.06f;
  int slMax_ = 220;
  int seeds_ = 5;
  std::vector<std::vector<Vector3>> lines_;
  bool dirty_ = true;
};
