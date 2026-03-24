#include "math/Streamline.hpp"

#include <cmath>

static bool insideDomain(const VecField3D& f, Vector3 p) {
  return p.x >= f.xmin && p.x <= f.xmax && p.y >= f.ymin && p.y <= f.ymax && p.z >= f.zmin &&
         p.z <= f.zmax;
}

static Vector3 rk4Step(const VecField3D& field, Vector3 p, float h) {
  Vector3 k1 = field.sample(p);
  Vector3 k2 = field.sample({p.x + 0.5f * h * k1.x, p.y + 0.5f * h * k1.y, p.z + 0.5f * h * k1.z});
  Vector3 k3 = field.sample({p.x + 0.5f * h * k2.x, p.y + 0.5f * h * k2.y, p.z + 0.5f * h * k2.z});
  Vector3 k4 = field.sample({p.x + h * k3.x, p.y + h * k3.y, p.z + h * k3.z});
  float inv6 = 1.0f / 6.0f;
  return {inv6 * (k1.x + 2.0f * k2.x + 2.0f * k3.x + k4.x),
          inv6 * (k1.y + 2.0f * k2.y + 2.0f * k3.y + k4.y),
          inv6 * (k1.z + 2.0f * k2.z + 2.0f * k3.z + k4.z)};
}

std::vector<Vector3> traceStreamline(const VecField3D& field, Vector3 seed,
                                     const StreamlineParams& p) {
  std::vector<Vector3> pts;
  if (!insideDomain(field, seed)) {
    return pts;
  }
  pts.push_back(seed);
  float h = p.stepSize * (p.forward ? 1.0f : -1.0f);
  Vector3 x = seed;
  for (int s = 0; s < p.maxSteps; ++s) {
    Vector3 v = rk4Step(field, x, h);
    float mag = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (mag < 1e-6f) {
      break;
    }
    Vector3 step = {h * v.x, h * v.y, h * v.z};
    x = {x.x + step.x, x.y + step.y, x.z + step.z};
    if (!insideDomain(field, x)) {
      break;
    }
    pts.push_back(x);
  }
  return pts;
}
