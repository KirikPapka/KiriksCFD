#pragma once

#include "cases/ICase.hpp"
#include "math/Derivatives.hpp"
#include "math/VecField3D.hpp"

#include <cstdint>
#include <vector>

enum class VectorColorMode { Speed, Divergence, Vorticity };

class VectorFieldCase final : public ICase {
public:
  VectorFieldCase();
  ~VectorFieldCase() override;

  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "3D vector field"; }
  const char* description() const override {
    return "This module shows a three-dimensional velocity field (no CFD solver). Choose built-in exam "
           "examples or type your own u_x, u_y, u_z as formulas of x, y, z (see Custom mode). Arrows show "
           "the field; you can colour by speed, divergence, or vorticity magnitude. Streamlines integrate "
           "the field with RK4.";
  }

private:
  void releaseCompiledExprs();
  bool tryCompileCustomExprs(char* errBuf, int errBufLen);
  void fillGridFromCompiledExprs();
  void rebuildField();
  void rebuildStreamlines();
  float scalarAtNode(int i, int j, int k) const;

  VecField3D field_;
  VecField3D curl_;
  ScalarField3D speed_;
  ScalarField3D div_;
  ScalarField3D vortMag_;

  int gridN_ = 12;
  float box_ = 2.0f;
  int fieldSource_ = 1;
  int fieldPreset_ = 0;
  float paramA_ = 0.5f;

  char exprUx_[256] = "y";
  char exprUy_[256] = "x";
  char exprUz_[256] = "0";
  double teVarX_ = 0;
  double teVarY_ = 0;
  double teVarZ_ = 0;
  void* teExpr_[3]{nullptr, nullptr, nullptr};
  int exprCompileError_ = 0;
  char exprStatus_[192] = "";
  uint64_t customExprHash_ = 0;

  VectorColorMode colorMode_ = VectorColorMode::Speed;
  float arrowScale_ = 0.35f;

  bool showStreamlines_ = true;
  int seedsPerAxis_ = 5;
  float slStep_ = 0.06f;
  int slMaxSteps_ = 180;

  std::vector<std::vector<Vector3>> streamlines_;

  bool dirty_ = true;
};
