#include "app/App.hpp"

#include "raylib.h"

int main() {
  App app;
  if (!app.init()) {
    return 1;
  }
  while (!WindowShouldClose()) {
    app.runFrame();
  }
  app.shutdown();
  return 0;
}
