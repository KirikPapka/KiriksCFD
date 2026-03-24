#pragma once

#include "raylib.h"

// Map a scalar in [tMin, tMax] to an RGB colour (viridis-ish: dark blue -> green -> yellow).

Color ColorFromScalar(float value, float vmin, float vmax);
