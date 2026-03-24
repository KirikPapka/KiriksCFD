#include "app/App.hpp"

#include "cases/AtmosphericColumnCase.hpp"
#include "cases/BaroclinicInstabilityCase.hpp"
#include "cases/BarotropicInstabilityCase.hpp"
#include "cases/BoundaryLayerCase.hpp"
#include "cases/BruntVaisalaCase.hpp"
#include "cases/CoordinateOperatorsCase.hpp"
#include "cases/CouetteFlowCase.hpp"
#include "cases/DiffusionSimilarityCase.hpp"
#include "cases/EkmanCase.hpp"
#include "cases/GeostrophicBalanceCase.hpp"
#include "cases/MoistThermodynamicsCase.hpp"
#include "cases/HelicityCase.hpp"
#include "cases/InclinedFilmFlowCase.hpp"
#include "cases/InertiaGravityWaveCase.hpp"
#include "cases/KelvinWaveCase.hpp"
#include "cases/OceanCirculationCase.hpp"
#include "cases/PipeFlowCase.hpp"
#include "cases/QGPVCase.hpp"
#include "cases/ReynoldsScalingCase.hpp"
#include "cases/RotatingForceBalanceCase.hpp"
#include "cases/RossbyRadiusCase.hpp"
#include "cases/RossbyWaveCase.hpp"
#include "cases/SaddleStreamfunctionCase.hpp"
#include "cases/ShallowWaterCase.hpp"
#include "cases/SphericalFlowCase.hpp"
#include "cases/StokesFlowCase.hpp"
#include "cases/StressTensorCase.hpp"
#include "cases/StreamfunctionVorticityCase.hpp"
#include "cases/StreamlineGeometry3DCase.hpp"
#include "cases/ThermalWindCase.hpp"
#include "cases/VectorFieldCase.hpp"
#include "cases/VorticityEquationCase.hpp"
#include "ui/UiPanel.hpp"

#include "imgui.h"
#include "raylib.h"
#include "rlImGui.h"

bool App::init() {
  if (inited_) {
    return true;
  }
  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
  InitWindow(1280, 720, "KiriksCFD - fluid exam visualiser");
  SetTargetFPS(60);
  rlImGuiSetup(true);
  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  camera_.reset();

  cases_.push_back(std::make_unique<VectorFieldCase>());
  cases_.push_back(std::make_unique<PipeFlowCase>());
  cases_.push_back(std::make_unique<ShallowWaterCase>());
  cases_.push_back(std::make_unique<EkmanCase>());
  cases_.push_back(std::make_unique<CouetteFlowCase>());
  cases_.push_back(std::make_unique<StreamfunctionVorticityCase>());
  cases_.push_back(std::make_unique<InclinedFilmFlowCase>());
  cases_.push_back(std::make_unique<RossbyWaveCase>());
  cases_.push_back(std::make_unique<InertiaGravityWaveCase>());
  cases_.push_back(std::make_unique<ThermalWindCase>());
  cases_.push_back(std::make_unique<HelicityCase>());
  cases_.push_back(std::make_unique<StreamlineGeometry3DCase>());
  cases_.push_back(std::make_unique<DiffusionSimilarityCase>());
  cases_.push_back(std::make_unique<SaddleStreamfunctionCase>());
  cases_.push_back(std::make_unique<VorticityEquationCase>());
  cases_.push_back(std::make_unique<CoordinateOperatorsCase>());
  cases_.push_back(std::make_unique<SphericalFlowCase>());
  cases_.push_back(std::make_unique<GeostrophicBalanceCase>());
  cases_.push_back(std::make_unique<RossbyRadiusCase>());
  cases_.push_back(std::make_unique<RotatingForceBalanceCase>());
  cases_.push_back(std::make_unique<BruntVaisalaCase>());
  cases_.push_back(std::make_unique<BoundaryLayerCase>());
  cases_.push_back(std::make_unique<StokesFlowCase>());
  cases_.push_back(std::make_unique<ReynoldsScalingCase>());
  cases_.push_back(std::make_unique<KelvinWaveCase>());
  cases_.push_back(std::make_unique<QGPVCase>());
  cases_.push_back(std::make_unique<BarotropicInstabilityCase>());
  cases_.push_back(std::make_unique<BaroclinicInstabilityCase>());
  cases_.push_back(std::make_unique<OceanCirculationCase>());
  cases_.push_back(std::make_unique<MoistThermodynamicsCase>());
  cases_.push_back(std::make_unique<StressTensorCase>());
  cases_.push_back(std::make_unique<AtmosphericColumnCase>());

  inited_ = true;
  return true;
}

void App::shutdown() {
  if (!inited_) {
    return;
  }
  rlImGuiShutdown();
  CloseWindow();
  cases_.clear();
  inited_ = false;
}

void App::drawWorld() {
  Camera3D cam = camera_.camera();
  BeginMode3D(cam);
  DrawGrid(48, 0.5f);
  const float L = 4.0f;
  DrawLine3D({0, 0, 0}, {L, 0, 0}, RED);
  DrawLine3D({0, 0, 0}, {0, L, 0}, GREEN);
  DrawLine3D({0, 0, 0}, {0, 0, L}, BLUE);
  if (currentCase_ >= 0 && currentCase_ < static_cast<int>(cases_.size())) {
    cases_[static_cast<size_t>(currentCase_)]->draw3D();
  }
  EndMode3D();

  auto axisLabel = [&](Vector3 world, const char* text, Color color) {
    Vector2 s = GetWorldToScreen(world, cam);
    if (s.x >= 0.0f && s.y >= 0.0f && s.x < static_cast<float>(GetScreenWidth()) &&
        s.y < static_cast<float>(GetScreenHeight())) {
      DrawText(text, static_cast<int>(s.x) - 6, static_cast<int>(s.y) - 10, 22, color);
    }
  };
  axisLabel({L + 0.22f, 0.0f, 0.0f}, "x", RED);
  axisLabel({0.0f, L + 0.22f, 0.0f}, "y", GREEN);
  axisLabel({0.0f, 0.0f, L + 0.22f}, "z", BLUE);
}

void App::runFrame() {
  float dt = GetFrameTime();
  BeginDrawing();
  ClearBackground({18, 22, 32, 255});

  camera_.update(!imguiWantsMouse_);

  if (currentCase_ >= 0 && currentCase_ < static_cast<int>(cases_.size())) {
    cases_[static_cast<size_t>(currentCase_)]->update(dt);
  }

  drawWorld();

  rlImGuiBegin();
  UiPanel::draw(cases_, currentCase_);
  rlImGuiEnd();
  imguiWantsMouse_ = ImGui::GetIO().WantCaptureMouse;

  EndDrawing();
}
