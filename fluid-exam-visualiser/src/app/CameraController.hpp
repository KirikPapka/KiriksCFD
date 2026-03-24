#pragma once

#include "raylib.h"

class CameraController {
public:
  void reset();
  void update(bool imguiCapturesMouse);
  Camera3D& camera() { return cam_; }
  const Camera3D& camera() const { return cam_; }

private:
  Camera3D cam_{};
  float yaw_ = 0.9f;
  float pitch_ = 0.45f;
  float distance_ = 9.0f;
  float sens_ = 0.005f;
  float zoomSens_ = 0.35f;
};
