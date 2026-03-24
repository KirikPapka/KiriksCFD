#include "render/TubeRenderer.hpp"

#include <algorithm>
#include <cmath>

static Color LerpColor(Color a, Color b, float t) {
  t = std::clamp(t, 0.0f, 1.0f);
  return {static_cast<unsigned char>(a.r + (b.r - a.r) * t),
          static_cast<unsigned char>(a.g + (b.g - a.g) * t),
          static_cast<unsigned char>(a.b + (b.b - a.b) * t), 255};
}

static float parabolicUz(float r, float R, float uMax) {
  if (R < 1e-6f) {
    return 0.0f;
  }
  float rr = r / R;
  return uMax * (1.0f - rr * rr);
}

static float sampleUz(float r, float R, float uNorm, PoiseuilleProfileFn fn, void* userData) {
  float u0 = (fn != nullptr) ? fn(r, R, userData) : parabolicUz(r, R, uNorm);
  return u0;
}

static Vector3 cylPt(float r, float theta, float z) {
  return {std::cos(theta) * r, std::sin(theta) * r, z};
}

void DrawPoiseuilleCylinderFn(float radius, float z0, float z1, int radialSegs, int thetaSegs, float uNorm,
                              Color cold, Color hot, PoiseuilleProfileFn fn, void* userData) {
  radialSegs = std::max(4, radialSegs);
  thetaSegs = std::max(12, thetaSegs);
  for (int ir = 0; ir < radialSegs; ++ir) {
    float rIn = static_cast<float>(ir) / static_cast<float>(radialSegs) * radius;
    float rOut = static_cast<float>(ir + 1) / static_cast<float>(radialSegs) * radius;
    float rMid = 0.5f * (rIn + rOut);
    float u0 = sampleUz(rMid, radius, uNorm, fn, userData);
    float ut = (uNorm > 1e-6f) ? std::clamp(u0 / uNorm, 0.0f, 1.0f) : 0.0f;
    Color c = LerpColor(cold, hot, ut);
    for (int it = 0; it < thetaSegs; ++it) {
      float t0 = static_cast<float>(it) / static_cast<float>(thetaSegs) * 2.0f * PI;
      float t1 = static_cast<float>(it + 1) / static_cast<float>(thetaSegs) * 2.0f * PI;
      Vector3 b00 = cylPt(rIn, t0, z0);
      Vector3 b01 = cylPt(rIn, t1, z0);
      Vector3 b10 = cylPt(rOut, t0, z0);
      Vector3 b11 = cylPt(rOut, t1, z0);
      Vector3 u00 = cylPt(rIn, t0, z1);
      Vector3 u01 = cylPt(rIn, t1, z1);
      Vector3 u10 = cylPt(rOut, t0, z1);
      Vector3 u11 = cylPt(rOut, t1, z1);
      DrawTriangle3D(b00, b10, b11, c);
      DrawTriangle3D(b00, b11, b01, c);
      DrawTriangle3D(u00, u01, u11, c);
      DrawTriangle3D(u00, u11, u10, c);
      DrawTriangle3D(b10, u10, u11, c);
      DrawTriangle3D(b10, u11, b11, c);
      DrawTriangle3D(b00, u01, u00, c);
      DrawTriangle3D(b00, b01, u01, c);
    }
  }
}

void DrawPoiseuilleCylinder(float radius, float z0, float z1, int radialSegs, int thetaSegs, float uMax,
                            Color cold, Color hot) {
  DrawPoiseuilleCylinderFn(radius, z0, z1, radialSegs, thetaSegs, uMax, cold, hot, nullptr, nullptr);
}

void DrawPoiseuilleSliceFn(float radius, float zPlane, int seg, float uNorm, Color cold, Color hot,
                           PoiseuilleProfileFn fn, void* userData) {
  seg = std::max(12, seg);
  Vector3 centre{0, 0, zPlane};
  int rings = 6;
  for (int j = 0; j < seg; ++j) {
    float t0 = static_cast<float>(j) / static_cast<float>(seg) * 2.0f * PI;
    float t1 = static_cast<float>(j + 1) / static_cast<float>(seg) * 2.0f * PI;
    for (int r = 0; r < rings; ++r) {
      float rr0 = static_cast<float>(r) / static_cast<float>(rings) * radius;
      float rr1 = static_cast<float>(r + 1) / static_cast<float>(rings) * radius;
      Vector3 p00{std::cos(t0) * rr0, std::sin(t0) * rr0, zPlane};
      Vector3 p01{std::cos(t1) * rr0, std::sin(t1) * rr0, zPlane};
      Vector3 p10{std::cos(t0) * rr1, std::sin(t0) * rr1, zPlane};
      Vector3 p11{std::cos(t1) * rr1, std::sin(t1) * rr1, zPlane};
      float u = sampleUz(0.5f * (rr0 + rr1), radius, uNorm, fn, userData);
      float ut = (uNorm > 1e-6f) ? std::clamp(u / uNorm, 0.0f, 1.0f) : 0.0f;
      Color col = LerpColor(cold, hot, ut);
      if (rr0 < 1e-5f) {
        DrawTriangle3D(centre, p10, p11, col);
      } else {
        DrawTriangle3D(p00, p10, p11, col);
        DrawTriangle3D(p00, p11, p01, col);
      }
    }
  }
}

void DrawPoiseuilleSlice(float radius, float zPlane, int seg, float uMax, Color cold, Color hot) {
  DrawPoiseuilleSliceFn(radius, zPlane, seg, uMax, cold, hot, nullptr, nullptr);
}
