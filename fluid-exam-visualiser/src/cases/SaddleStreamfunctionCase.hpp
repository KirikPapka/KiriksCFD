#pragma once

#include "cases/ICase.hpp"

#include <cstdint>
#include <vector>

class SaddleStreamfunctionCase final : public ICase {
public:
  SaddleStreamfunctionCase();
  ~SaddleStreamfunctionCase() override;

  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "Hyperbolic streamfunction"; }
  const char* description() const override {
    return "The streamfunction psi = x^2/a^2 - z^2/b^2 describes a saddle point in the flow: inflow along "
           "some axes and outflow along others, with hyperbola-shaped streamlines. Because psi is a "
           "quadratic polynomial, its Laplacian is constant, so vorticity is uniform in the plane - good for "
           "checking your derivations on paper against what you see. Custom psi(x,z) uses finite "
           "differences for vorticity on the grid.";
  }

private:
  void rebuild();
  float psiBuiltin(float x, float z) const;
  void tryCompileCustom();

  int n_ = 40;
  float L_ = 2.0f;
  float a_ = 1.0f;
  float b_ = 1.0f;
  std::vector<float> psi_;
  std::vector<float> omega_;
  bool dirty_ = true;

  int fieldMode_ = 0;
  char exprPsi_[256] = "x*x/(a*a)-z*z/(b*b)";
  void* tePsi_ = nullptr;
  double teX_ = 0;
  double teZ_ = 0;
  double teA_ = 0;
  double teB_ = 0;
  uint64_t exprHash_ = 0;
  char exprStatus_[160] = "";
};
