#include "cases/MoistThermodynamicsCase.hpp"

#include "imgui.h"
#include "raylib.h"

#include <algorithm>
#include <cmath>
#include <vector>

void MoistThermodynamicsCase::update(float /*dt*/) {}

void MoistThermodynamicsCase::draw3D() {
  DrawCylinder({0.0f, 1.2f, 0.0f}, 0.25f, 0.25f, 2.4f, 10, {140, 180, 220, 120});
  float buoy = std::max(0.0f, (GammaEnv_ - GammaParcel_) * 2000.0f);
  DrawCube({0.45f, 1.4f + buoy * 0.0003f, 0.0f}, 0.5f, 0.35f + buoy * 0.0002f, 0.4f,
           {255, 200, 160, static_cast<unsigned char>(std::min(200, 80 + static_cast<int>(buoy)))});
}

void MoistThermodynamicsCase::drawUI() {
  ImGui::TextUnformatted("Toy dry-adiabat-style slopes: compare parcel Gamma_parcel to env Gamma_env.");
  ImGui::SliderFloat("T0 (K) at surface", &T0_, 280.0f, 310.0f);
  ImGui::SliderFloat("env lapse |dT/dz| (K/m)", &GammaEnv_, 0.004f, 0.01f);
  ImGui::SliderFloat("parcel lapse |dT/dz| (K/m)", &GammaParcel_, 0.004f, 0.01f);
  ImGui::SliderFloat("z_top (m)", &zTop_, 5000.0f, 16000.0f);
  ImGui::SliderFloat("lift level (km)", &liftKm_, 0.0f, 3.0f);
  const int n = 64;
  std::vector<float> zv(n), te(n), tp(n);
  float g = 9.81f;
  float cp = 1005.0f;
  for (int i = 0; i < n; ++i) {
    float z = static_cast<float>(i) / static_cast<float>(n - 1) * zTop_;
    zv[static_cast<size_t>(i)] = z / 1000.0f;
    float Tenv = T0_ - GammaEnv_ * z;
    float zl = liftKm_ * 1000.0f;
    float Tplift = T0_ - GammaParcel_ * zl;
    float Tparcel = (z >= zl) ? (Tplift - GammaParcel_ * (z - zl)) : (T0_ - GammaParcel_ * z);
    te[static_cast<size_t>(i)] = Tenv;
    tp[static_cast<size_t>(i)] = Tparcel;
  }
  ImGui::PlotLines("T_env (K)", te.data(), n, 0, nullptr, 200.0f, 320.0f, ImVec2(0, 90));
  ImGui::PlotLines("T_parcel (K)", tp.data(), n, 0, nullptr, 200.0f, 320.0f, ImVec2(0, 90));
  float capeSketch = std::max(0.0f, (GammaEnv_ - GammaParcel_)) * g / std::max(1e-4f, T0_) * zTop_;
  ImGui::Text("Buoyancy proxy ~(Gamma_env - Gamma_parcel) * g/T0 * H  ~  %.4f (arbitrary units)", capeSketch);
  ImGui::TextUnformatted("MTH3001: Ch.4 moist / CAPE-CIN ideas. MTH3007: (n/a).");
}
