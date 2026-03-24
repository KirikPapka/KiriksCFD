#pragma once

#include "cases/ICase.hpp"

#include <cstdint>

class DiffusionSimilarityCase final : public ICase {
public:
  DiffusionSimilarityCase();
  ~DiffusionSimilarityCase() override;

  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "Diffusion similarity"; }
  const char* description() const override {
    return "u(z,t) = U erf(eta), eta = z / sqrt(4 nu t); satisfies u_t = nu u_zz; constant flux in z for "
           "this profile class. Custom: u(z,t) with variables z, t, U, nu.";
  }

private:
  void tryCompileCustom();

  float nu_ = 0.1f;
  float t_ = 0.5f;
  float U_ = 1.0f;
  float L_ = 2.0f;

  int fieldMode_ = 0;
  char exprU_[256] = "U*z/sqrt(z*z+4*nu*t)";
  void* teU_ = nullptr;
  double teZ_ = 0;
  double teT_ = 0;
  double teUcap_ = 0;
  double teNu_ = 0;
  uint64_t exprHash_ = 0;
  char exprStatus_[160] = "";
};
