#pragma once

#include "cases/ICase.hpp"

#include <cstdint>
#include <vector>

class StreamfunctionVorticityCase final : public ICase {
public:
  StreamfunctionVorticityCase();
  ~StreamfunctionVorticityCase() override;

  void update(float dt) override;
  void draw3D() override;
  void drawUI() override;
  const char* name() const override { return "Streamfunction / vorticity"; }
  const char* description() const override {
    return "In two-dimensional incompressible flow you can pack both components of velocity into a single "
           "scalar streamfunction psi: flow follows contours of psi, and the scalar vorticity is minus the "
           "Laplacian of psi. The coloured sheet is psi; arrows are the velocity field. Switching presets "
           "changes the flow pattern (waves, a vortex-like core, etc.) while keeping the same conceptual "
           "link between psi, u, and vorticity. Custom psi(x,z) uses grid finite differences for omega.";
  }

private:
  void rebuild();
  float psiAt(float x, float z) const;
  void tryCompileCustom();

  int n_ = 32;
  float L_ = 2.0f;
  int preset_ = 0;
  float amp_ = 1.0f;
  float k_ = 1.2f;

  std::vector<float> psi_;
  std::vector<float> omega_;
  bool dirty_ = true;

  int fieldMode_ = 0;
  char exprPsi_[256] = "amp*sin(k*x)*sin(k*z)";
  void* tePsi_ = nullptr;
  double teX_ = 0;
  double teZ_ = 0;
  double teAmp_ = 0;
  double teK_ = 0;
  uint64_t exprHash_ = 0;
  char exprStatus_[160] = "";
};
