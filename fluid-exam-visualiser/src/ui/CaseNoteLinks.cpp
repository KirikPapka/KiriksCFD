#include "ui/CaseNoteLinks.hpp"

#include <cstring>

namespace {

struct Row {
  const char* caseName;
  const char* text;
};

Row kTable[] = {
    {"3D vector field",
     "MTH3007: Ch. 1 (fundamentals / incompressible Navier-Stokes / visualising fluid flows): "
     "velocity fields, streamlines, divergence, curl, kinematics.\n"
     "MTH3001: optional background only."},

    {"Pipe Poiseuille",
     "MTH3007: Ch. 2.4 (Hagen-Poiseuille flow in a cylindrical pipe).\n"
     "MTH3001: (n/a)"},

    {"Shallow water surface",
     "MTH3007: optional wave intuition only.\n"
     "MTH3001: Ch. 5.2 (Shallow water theory) + Ch. 5.3 (Plane waves)."},

    {"Ekman spiral",
     "MTH3007: optional rotating-flow intuition only.\n"
     "MTH3001: Ch. 5.1.2 (Ekman spirals)."},

    {"Couette flow",
     "MTH3007: Ch. 2.2 (Plane Couette flow).\n"
     "MTH3001: (n/a)"},

    {"Streamfunction / vorticity",
     "MTH3007: Ch. 1 (2-D incompressible flow / streamfunction basics) + exam problems on "
     "streamfunction-vorticity formulation.\n"
     "MTH3001: optional geostrophic streamfunction link in Ch. 5.1.1."},

    {"Inclined film flow",
     "MTH3007: closest match is Ch. 2.3 (plane Poiseuille-type viscous exact solutions) and "
     "related exam exact-flow questions.\n"
     "MTH3001: (n/a)"},

    {"Rossby wave (beta-plane)",
     "MTH3007: optional rotating-wave intuition only.\n"
     "MTH3001: Ch. 5.3.2 (Rossby waves on a beta-plane)."},

    {"Inertia-gravity waves (f-plane)",
     "MTH3007: optional wave intuition only.\n"
     "MTH3001: Ch. 5.3.1 (Inertial waves on an f-plane); also linked to Ch. 5.2 shallow water."},

    {"Thermal wind",
     "MTH3007: optional rotating-flow preview only.\n"
     "MTH3001: Ch. 5.1.1 (Geostrophic flow / Thermal Wind shear)."},

    {"Helicity density",
     "MTH3007: Ch. 1 (vorticity / kinematics) + exam vorticity questions.\n"
     "MTH3001: (n/a)"},

    {"3D streamline geometry (helical preset)",
     "MTH3007: Ch. 1 (visualising fluid flows, streamlines, trajectories).\n"
     "MTH3001: (n/a)"},

    {"Diffusion similarity",
     "MTH3007: Ch. 2 (unidirectional viscous flow / exact reductions) + related exam similarity "
     "or diffusion-style questions.\n"
     "MTH3001: (n/a)"},

    {"Hyperbolic streamfunction",
     "MTH3007: Ch. 1 (streamlines / streamfunction geometry) + related exam streamline topology "
     "questions.\n"
     "MTH3001: (n/a)"},

    {"Vorticity equation sandbox",
     "MTH3007: Ch. 1 (vorticity / incompressible flow theory) + exam vorticity-equation problems.\n"
     "MTH3001: Ch. 5.4 (Quasi-geostrophic potential vorticity) for large-scale vorticity ideas."},

    {"Coordinate operators lab",
     "MTH3007: Ch. 1 (coordinate systems / vector calculus foundations) + formula sheet.\n"
     "MTH3001: optional coordinate / rotating-frame background only."},

    {"Spherical-coordinate flow",
     "MTH3007: Ch. 1 (coordinate systems / vector calculus in curvilinear coordinates) + "
     "formula sheet.\n"
     "MTH3001: optional atmospheric-shell geometry background."},

    {"Geostrophic balance",
     "MTH3007: optional rotating-flow intuition only.\n"
     "MTH3001: Ch. 5.1.1 (Geostrophic flow / Thermal Wind shear)."},

    {"Rossby deformation radius",
     "MTH3007: optional rotating-wave scale intuition only.\n"
     "MTH3001: Ch. 5.2 (Shallow water theory) + Ch. 5.3.2 (Rossby waves)."},

    {"Rotating-frame forces",
     "MTH3007: optional only.\n"
     "MTH3001: Ch. 5.1 (Rotating Navier-Stokes)."},

    {"Brunt-Vaisala oscillation",
     "MTH3007: optional buoyancy-wave intuition only.\n"
     "MTH3001: Ch. 3.5 (Vertical oscillations in stratified fluids)."},

    {"Blasius boundary layer",
     "MTH3007: Ch.5.4-5.5 (boundary-layer equations, Blasius / similarity).\n"
     "MTH3001: (n/a)"},

    {"Stokes flow (creeping past sphere)",
     "MTH3007: Ch.6 (Stokes flow, low-Re drag).\n"
     "MTH3001: (n/a)"},

    {"Reynolds number scaling",
     "MTH3007: Ch.5.4 and Ch.6.1 (Re ordering; inertia vs viscosity).\n"
     "MTH3001: indirect scaling in rotating / SW notes only."},

    {"Kelvin waves",
     "MTH3007: (n/a)\n"
     "MTH3001: Sec.5.3.3-5.3.4 (coastal and equatorial Kelvin waves)."},

    {"QG potential vorticity (toy)",
     "MTH3007: Ch.1 vorticity / optional rotating intuition only.\n"
     "MTH3001: Ch.5.4 (quasi-geostrophic PV)."},

    {"Barotropic shear instability",
     "MTH3007: (n/a)\n"
     "MTH3001: Ch.5.5 (barotropic instability / jet meanders)."},

    {"Baroclinic / Eady wave",
     "MTH3007: (n/a)\n"
     "MTH3001: Ch.5.6 (baroclinic instability, Eady model)."},

    {"Ocean gyre (Sverdrup sketch)",
     "MTH3007: (n/a)\n"
     "MTH3001: Ch.6.1-6.3 (wind-driven gyres, western intensification)."},

    {"Moist thermodynamics (CAPE sketch)",
     "MTH3007: (n/a)\n"
     "MTH3001: Ch.4.1-4.2.1 (moist thermodynamics, CAPE/CIN ideas)."},

    {"Stress tensor / traction",
     "MTH3007: Cauchy stress / traction with NS (Ch.1 foundations + BL applications).\n"
     "MTH3001: (n/a)"},

    {"Atmospheric column",
     "MTH3007: (n/a)\n"
     "MTH3001: Ch.3.1-3.6 and Ch.4 (vertical structure, thermodynamics, potential temperature)."},
};

}  // namespace

const char* noteRefsForCaseName(const char* caseNameUtf8) {
  if (caseNameUtf8 == nullptr) {
    return nullptr;
  }
  for (const Row& r : kTable) {
    if (std::strcmp(caseNameUtf8, r.caseName) == 0) {
      return r.text;
    }
  }
  return "No crosswalk entry yet - add this case title to kTable in src/ui/CaseNoteLinks.cpp.";
}