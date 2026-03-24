#pragma once

// Minimal interface for each exam visualisation scenario.

class ICase {
public:
  virtual ~ICase() = default;

  virtual void update(float dt) = 0;
  virtual void draw3D() = 0;
  virtual void drawUI() = 0;
  virtual const char* name() const = 0;
  // Short conceptual overview: what the physics is and what the view is showing.
  virtual const char* description() const = 0;
};
