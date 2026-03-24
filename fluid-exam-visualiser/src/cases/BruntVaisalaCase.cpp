#include "cases/BruntVaisalaCase.hpp"

#include <algorithm>

#include "imgui.h"
#include "raylib.h"
#include "raymath.h"

#include <cmath>

void BruntVaisalaCase::update(float dt) {
  if (std::abs(z0_ - trailZ0_) > 1e-4f || std::abs(A_ - trailA_) > 1e-4f) {
    trail_.clear();
    trailZ0_ = z0_;
    trailA_ = A_;
  }
  time_ += dt * timeScale_;
  float y = z0_ + A_ * std::sin(N_ * time_);
  Vector3 p{0.0f, y, 0.0f};
  if (trail_.empty() || Vector3Distance(p, trail_.back()) > 0.025f) {
    trail_.push_back(p);
  }
  while (trail_.size() > kMaxTrail) {
    trail_.erase(trail_.begin());
  }
}

void BruntVaisalaCase::draw3D() {
  float y = z0_ + A_ * std::sin(N_ * time_);
  for (size_t i = 1; i < trail_.size(); ++i) {
    DrawLine3D(trail_[i - 1], trail_[i], {255, 180, 90, 200});
  }
  DrawSphere({0, y, 0}, 0.15f, {255, 200, 100, 255});
  DrawLine3D({0, z0_ - A_, 0}, {0, z0_ + A_, 0}, {100, 100, 140, 180});
  DrawGrid(16, 0.5f);
}

void BruntVaisalaCase::drawUI() {
  ImGui::TextUnformatted("Parcel height y(t) = z0 + A sin(N t); N = sqrt(-(g/rho) d rho/dz) (stable stratification).");
  ImGui::Text("Period T = 2 pi / N = %.3f s (if N in rad/s)", 2.0f * PI / std::max(1e-4f, N_));
  ImGui::SliderFloat("N (buoyancy frequency)", &N_, 0.2f, 4.0f);
  ImGui::SliderFloat("amplitude A", &A_, 0.1f, 1.5f);
  ImGui::SliderFloat("equilibrium z0", &z0_, 0.5f, 4.0f);
  ImGui::SliderFloat("time scale", &timeScale_, 0.0f, 2.0f);
}
