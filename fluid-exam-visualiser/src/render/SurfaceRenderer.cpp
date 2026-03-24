#include "render/SurfaceRenderer.hpp"

#include <algorithm>
#include <cmath>

void DrawHeightFieldSurface(int nx, int ny, const float* heights, float x0, float x1, float y0,
                            float y1, float zScale, Color tint) {
  if (nx < 2 || ny < 2 || heights == nullptr) {
    return;
  }
  float dx = (x1 - x0) / static_cast<float>(nx - 1);
  float dy = (y1 - y0) / static_cast<float>(ny - 1);
  float hMin = heights[0], hMax = heights[0];
  for (int i = 1; i < nx * ny; ++i) {
    hMin = std::min(hMin, heights[i]);
    hMax = std::max(hMax, heights[i]);
  }
  float hr = (hMax > hMin + 1e-8f) ? (1.0f / (hMax - hMin)) : 1.0f;

  auto pos = [&](int ix, int iy) {
    float x = x0 + static_cast<float>(ix) * dx;
    float y = y0 + static_cast<float>(iy) * dy;
    float z = heights[iy * nx + ix] * zScale;
    return Vector3{x, z, y};
  };

  auto cellColor = [&](int ix, int iy) {
    float h = heights[iy * nx + ix];
    float t = (h - hMin) * hr;
    t = std::clamp(t, 0.0f, 1.0f);
    unsigned char r = static_cast<unsigned char>(tint.r * (0.4f + 0.6f * t));
    unsigned char g = static_cast<unsigned char>(tint.g * (0.5f + 0.5f * (1.0f - std::abs(t - 0.5f))));
    unsigned char b = static_cast<unsigned char>(tint.b * (1.0f - 0.5f * t));
    return Color{r, g, b, 255};
  };

  for (int j = 0; j < ny - 1; ++j) {
    for (int i = 0; i < nx - 1; ++i) {
      Vector3 p00 = pos(i, j);
      Vector3 p10 = pos(i + 1, j);
      Vector3 p01 = pos(i, j + 1);
      Vector3 p11 = pos(i + 1, j + 1);
      // Winding chosen so outward normal has +Y; default order was back-face culled from above.
      DrawTriangle3D(p00, p11, p10, cellColor(i, j));
      DrawTriangle3D(p00, p01, p11, cellColor(i, j));
    }
  }
}
