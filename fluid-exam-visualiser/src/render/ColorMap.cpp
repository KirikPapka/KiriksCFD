#include "render/ColorMap.hpp"

#include <algorithm>
#include <cmath>

static float Smoothstep(float t) {
  t = std::clamp(t, 0.0f, 1.0f);
  return t * t * (3.0f - 2.0f * t);
}

Color ColorFromScalar(float value, float vmin, float vmax) {
  if (vmax <= vmin + 1e-8f) {
    return {120, 200, 255, 255};
  }
  float t = (value - vmin) / (vmax - vmin);
  t = std::clamp(t, 0.0f, 1.0f);
  // Piecewise: blue -> cyan -> green -> yellow
  float r = 0.0f, g = 0.0f, b = 0.0f;
  if (t < 0.25f) {
    float u = Smoothstep(t / 0.25f);
    r = 15.0f + 40.0f * u;
    g = 30.0f + 120.0f * u;
    b = 120.0f + 135.0f * u;
  } else if (t < 0.5f) {
    float u = Smoothstep((t - 0.25f) / 0.25f);
    r = 55.0f + 100.0f * u;
    g = 150.0f + 80.0f * u;
    b = 255.0f - 100.0f * u;
  } else if (t < 0.75f) {
    float u = Smoothstep((t - 0.5f) / 0.25f);
    r = 155.0f + 80.0f * u;
    g = 230.0f + 25.0f * u;
    b = 155.0f - 100.0f * u;
  } else {
    float u = Smoothstep((t - 0.75f) / 0.25f);
    r = 235.0f + 20.0f * u;
    g = 255.0f;
    b = 55.0f + 50.0f * u;
  }
  return {static_cast<unsigned char>(r), static_cast<unsigned char>(g),
          static_cast<unsigned char>(b), 255};
}
