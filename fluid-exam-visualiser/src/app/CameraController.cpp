#include "app/CameraController.hpp"

#include <algorithm>
#include <cmath>

void CameraController::reset() {
  cam_.fovy = 55.0f;
  cam_.projection = CAMERA_PERSPECTIVE;
  yaw_ = 0.9f;
  // Elevation above the horizontal (xz) plane through the target: positive => camera above y=target.y.
  pitch_ = 0.45f;
  distance_ = 9.0f;
}

void CameraController::update(bool imguiCapturesMouse) {
  if (!imguiCapturesMouse) {
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
      Vector2 d = GetMouseDelta();
      yaw_ -= d.x * sens_;
      // Mouse down => look down => decrease elevation.
      pitch_ -= d.y * sens_;
    }
    float wheel = GetMouseWheelMove();
    distance_ -= wheel * zoomSens_;
  }
  const float pitchLimit = static_cast<float>(PI) * 0.5f - 0.02f;
  pitch_ = std::clamp(pitch_, -pitchLimit, pitchLimit);
  distance_ = std::clamp(distance_, 2.0f, 120.0f);

  cam_.target = {0.0f, 0.0f, 0.0f};
  float cosP = std::cos(pitch_);
  float sinP = std::sin(pitch_);
  float sinY = std::sin(yaw_);
  float cosY = std::cos(yaw_);
  // Y-up orbit: offset from target (can sit above or below the xz plane through the origin).
  float x = distance_ * cosP * sinY;
  float y = distance_ * sinP;
  float z = distance_ * cosP * cosY;
  cam_.position = {cam_.target.x + x, cam_.target.y + y, cam_.target.z + z};
  cam_.up = {0.0f, 1.0f, 0.0f};
}
