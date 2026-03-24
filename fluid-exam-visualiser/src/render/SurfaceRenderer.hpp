#pragma once

#include "raylib.h"

// Height field z = h(x,y) on a regular grid (row-major: ix + iy * nx).

void DrawHeightFieldSurface(int nx, int ny, const float* heights, float x0, float x1, float y0,
                            float y1, float zScale, Color tint);
