#pragma once

#include "app/CameraController.hpp"
#include "cases/ICase.hpp"

#include <memory>
#include <vector>

class App {
public:
  bool init();
  void shutdown();
  void runFrame();

private:
  void drawWorld();

  CameraController camera_;
  std::vector<std::unique_ptr<ICase>> cases_;
  int currentCase_ = 0;
  bool inited_ = false;
  bool imguiWantsMouse_ = false;
};
