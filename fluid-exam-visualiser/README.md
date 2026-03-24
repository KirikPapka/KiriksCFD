# KiriksCFD — visualiser for Fluid Dynamics and Theory of Weather and Climate problems

Small **C++17** desktop app for revising canonical **fluid dynamics** and **geophysical fluid dynamics** problems. It is **not** a general CFD or Navier–Stokes solver: everything here is **analytic or grid post-processing** for pictures and intuition.

## Stack

- **CMake** + **raylib** (window, 3D drawing, camera)
- **Dear ImGui** via **[rlImGui](https://github.com/raylib-extras/rlImGui)** (UI)
- No Eigen in this MVP (plain `Vector3` / small loops)
- **[tinyexpr](https://github.com/codeplea/tinyexpr)** (optional use): custom `u_x, u_y, u_z` formulas in **3D vector field** mode

Dependencies are pulled automatically with **CMake FetchContent** (first configure needs network).

## Build

```bash
cd fluid-exam-visualiser
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/KiriksCFD
```

**macOS:** Xcode Command Line Tools are enough. You may see a one-time OpenGL deprecation notice from the SDK; raylib still uses OpenGL 3.3 there.

**Linux:** Install usual OpenGL/GLFW build deps for raylib (e.g. `libgl1-mesa-dev`, `libx11-dev`, …) if the linker complains.

**Windows:** Visual Studio 2019+ generator or Ninja; CMake will build raylib for you.

### From GitHub

The repository root is the parent folder that contains `fluid-exam-visualiser/` (see `.gitignore` there). After cloning:

```bash
cd fluid-exam-visualiser
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/KiriksCFD
```

On **Windows**, use `build\Release\KiriksCFD.exe` or `build\KiriksCFD.exe` depending on the generator. GitHub Actions (`.github/workflows/ci.yml`) builds on Ubuntu, macOS, and Windows on each push to `main`/`master`.

## Controls

- **Right-drag:** orbit camera
- **Mouse wheel:** zoom
- **Side panel:** pick a **case**, read **About this case** (short conceptual explanation), then tune parameters

## Cases (dropdown order)

Each item is a separate `ICase` under `src/cases/`. All are **analytic / pedagogical**, not CFD solvers.

**Original MVP**

1. **3D vector field** — **u** on a cube, RK4 streamlines, ∇·**u**, ∇×**u**, colour modes.
2. **Pipe Poiseuille** — **u_z(r) = U_max (1 − r²/R²)**; optional cross-section.
3. **Shallow water** — travelling **η** surface + schematic arrows.
4. **Ekman spiral** — decay and turning with height.

**Tier 1 (exam-heavy)**

5. **Couette flow** — **u = U z/h**, flux per unit width **Q = U h/2**.
6. **Streamfunction / vorticity** — **u** = curl(ψ **j**), **ω** = −∇²ψ, coloured ψ and arrows.
7. **Inclined film** — Nusselt-type parabola, **Q = ρg sin α h³/(3μ)**.
8. **Rossby wave (beta-plane)** — **ω = −β k_x/(K² + L_d⁻²)** on a moving **η** surface.
9. **Inertia–gravity waves** — **ω² = f² + gH K²**, **L_d**, phase vs group speed sketch.
10. **Thermal wind / jet** — shear **du/dz ∝ (g/fT₀) ∂T/∂y** (schematic).

**Tier 2**

11. **Helicity** — **u**, **ω**, and **u·ω** colouring.
12. **3D streamline geometry** — preset **u = (z, a, −x)** + RK4 lines, slider **a**.
13. **Diffusing shear layer** — **u = U erf(z/√(4νt))**, similarity width.
14. **Hyperbolic streamfunction** — **ψ = x²/a² − z²/b²**, uniform **ω_z**.
15. **Vorticity equation sandbox** — toggles / text for stretching, diffusion, advection.
16. **Coordinate operators lab** — grad / div / curl / Laplacian on test fields + cyl/sph notes.

**Tier 3**

17. **Spherical flow** — **u = (sin θ/r) θ̂** on a sphere (div / curl exam context).
18. **Geostrophic balance** — Gaussian **η**, **u_g** from **f** and ∇**η**.
19. **Rossby deformation radius** — short vs long **k** limits side by side.
20. **Rotating-frame forces** — schematic pressure, Coriolis, inertia, viscosity vs **Ro**, **Re**.
21. **Brunt–Väisälä** — parcel **sin(Nt)**.
22. **Boundary layer (Blasius sketch)** — plate, **δ(x)**, Pohlhausen **u(y)**, wall shear tint.
23. **Stokes flow (sphere)** — low-**Re** Stokes field + optional inertial hint.
24. **Reynolds scaling** — **Re = UL/ν** sliders, three regime panels + crude force bar.
25. **Kelvin waves** — trapped **η** + offshore decay, coastal / wide-decay toggle.
26. **QG PV (toy)** — two stacked **ψ** surfaces and coupling sticks.
27. **Barotropic instability** — **sech²** jet + growing meander cartoon.
28. **Baroclinic / Eady** — shear **Λz**, tilted isentrope sketch, growth envelope.
29. **Ocean gyre** — Sverdrup-style **ψ** + western exponential intensification.
30. **Moist thermodynamics** — toy sounding, **T** plots, CAPE proxy + 3D column hint.
31. **Stress tensor** — cube faces, **t = σ·n** for pressure / shear / full Cauchy.
32. **Atmospheric column** — isothermal **p(z)** or dry adiabatic **T(z)**, plots + vertical **T** column in 3D.

## Layout

```
fluid-exam-visualiser/
  CMakeLists.txt
  README.md
  src/
    main.cpp
    app/          # window loop, orbit camera
    cases/        # ICase + one file per scenario
    math/         # grid field, RK4 streamlines, finite differences
    render/       # arrows, surface strips, Poiseuille geometry, colour map
    ui/           # ImGui side panel + case dropdown
```

## TODO / extensions (ideas)

- Pin **rlImGui** to a **git tag or commit** alongside the ImGui version in `CMakeLists.txt` for fully reproducible builds.
- Add **export screenshot** or **GIF** for lecture notes.
- **Vector field:** load CSV / NumPy-exported grids; more analytic fields (Hill vortex, stagnation point).
- **Shallow water:** true linear wave relations **ω(k)** and **geostrophic** balance toggle.
- **Ekman:** match textbook **u + iv = u_g + (u_0 − u_g) e^{−(1+i)z/D}** with separate **u_g** slider.
- Optional **Eigen** for small least-squares or 2×2 linear algebra if you add data fitting.
- Replace triangle soup **pipe** with a single **mesh + vertex colours** for speed at high segment counts.

## Licence

Look at licence section

##Any mistakes/suggestions can be emailed to kp604@exeter.ac.uk
