#pragma once

#include "raylib.h"

// Cylinder along +Z from z0 to z1, centred on z axis; colour by analytic u_z(r) = Umax(1 - r²/R²).

void DrawPoiseuilleCylinder(float radius, float z0, float z1, int radialSegs, int thetaSegs, float uMax,
                            Color cold, Color hot);

// Same geometry; u_z(r) from callback u = fn(r, R, userData). uNorm scales colour (e.g. max |u|).

using PoiseuilleProfileFn = float (*)(float r, float R, void* userData);

void DrawPoiseuilleCylinderFn(float radius, float z0, float z1, int radialSegs, int thetaSegs, float uNorm,
                              Color cold, Color hot, PoiseuilleProfileFn fn, void* userData);

// Filled disk in XY at z = zPlane, coloured by parabolic profile (exam cross-section).

void DrawPoiseuilleSlice(float radius, float zPlane, int seg, float uMax, Color cold, Color hot);

void DrawPoiseuilleSliceFn(float radius, float zPlane, int seg, float uNorm, Color cold, Color hot,
                           PoiseuilleProfileFn fn, void* userData);
