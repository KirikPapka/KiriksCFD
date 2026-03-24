#pragma once

#include "cases/ICase.hpp"

class CoordinateOperatorsCase final : public ICase {
public:
  CoordinateOperatorsCase();
  ~CoordinateOperatorsCase() override;

  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "Coordinate operators lab"; }
  const char* description() const override {
    return "Gradient, divergence, curl, and Laplacian look different in Cartesian, cylindrical, and "
           "spherical coordinates. Here you pick simple test fields and a point (x,y,z); the app prints "
           "the analytic result you would get from your formula sheet. Custom modes let you type "
           "phi(x,y,z) or F components with tinyexpr; grad / lap / div / curl are shown numerically in "
           "Cartesian coordinates. Use it to sanity-check signs and components when revising vector "
           "identities in curvilinear systems.";
  }

private:
  int system_ = 0;
  int field_ = 0;
  int op_ = 0;
  float xr_ = 1.0f;
  float yr_ = 0.5f;
  float zr_ = 0.8f;
  float quadA_ = 1.0f;
  float quadB_ = 1.0f;
  float quadC_ = 1.0f;
  float vecFx_ = 1.0f;
  float vecFy_ = 1.0f;

  char exprPhi_[384]{};
  char exprFx_[256]{};
  char exprFy_[256]{};
  char exprFz_[256]{};
  char phiStatus_[112]{};
  char vecStatus_[160]{};
  double teX_ = 0.0;
  double teY_ = 0.0;
  double teZ_ = 0.0;
  void* tePhi_ = nullptr;
  void* teFx_ = nullptr;
  void* teFy_ = nullptr;
  void* teFz_ = nullptr;

  void tryCompileCustomScalar();
  void tryCompileCustomVector();
  void numericGradPhi(float x, float y, float z, float* gx, float* gy, float* gz);
  float numericLapPhi(float x, float y, float z);
  bool evalCustomF(double x, double y, double z, double* ox, double* oy, double* oz);
  void numericDivF(float x, float y, float z, float* div);
  void numericCurlF(float x, float y, float z, float* cx, float* cy, float* cz);
};
