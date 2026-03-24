#include "render/ArrowRenderer.hpp"

#include <algorithm>
#include <cmath>

static Vector3 NormalizeOrZero(Vector3 v) {
  float m = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
  if (m < 1e-8f) {
    return {0, 0, 0};
  }
  return {v.x / m, v.y / m, v.z / m};
}

void DrawArrow3D(Vector3 from, Vector3 dir, float shaftLen, float headLen, float shaftThick,
                 Color col) {
  Vector3 d = NormalizeOrZero(dir);
  if (d.x == 0 && d.y == 0 && d.z == 0) {
    return;
  }
  Vector3 tip = {from.x + d.x * shaftLen, from.y + d.y * shaftLen, from.z + d.z * shaftLen};
  Vector3 headBase = {tip.x - d.x * headLen, tip.y - d.y * headLen, tip.z - d.z * headLen};
  float headR = std::max(shaftThick * 1.8f, 0.02f);
  DrawCylinderEx(from, headBase, shaftThick, shaftThick, 8, col);
  DrawCylinderEx(headBase, tip, headR, 0.001f, 8, col);
}
