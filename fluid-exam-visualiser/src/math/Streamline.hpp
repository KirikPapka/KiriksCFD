#pragma once

#include "math/VecField3D.hpp"

#include "raylib.h"

#include <vector>

// Integrate dx/dt = u(x) with classical RK4; stops at domain bounds or max steps.

struct StreamlineParams {
  float stepSize = 0.05f;
  int maxSteps = 200;
  bool forward = true;
};

std::vector<Vector3> traceStreamline(const VecField3D& field, Vector3 seed,
                                     const StreamlineParams& p);
