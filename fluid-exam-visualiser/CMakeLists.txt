cmake_minimum_required(VERSION 3.16)
project(KiriksCFD LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum macOS version")
endif()

include(FetchContent)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG 5.5
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(raylib)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG v1.92.6-docking
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(imgui)

FetchContent_Declare(
  rlimgui
  GIT_REPOSITORY https://github.com/raylib-extras/rlImGui.git
  GIT_TAG main
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(rlimgui)

# rlImGui only calls GetCharPressed() when io.WantCaptureKeyboard is already true; that flag is updated
# after the previous frame's ImGui::Render(), so ProcessEvents() at frame start often skips Unicode input
# and InputText fields never receive characters. Always draining the queue is safe: ImGui drops unused text.
set(KIRIKS_RLIMGUI_CPP "${CMAKE_CURRENT_BINARY_DIR}/generated/kriks_rlImGui.cpp")
file(READ "${rlimgui_SOURCE_DIR}/rlImGui.cpp" KIRIKS_RLIMGUI_SRC)
string(
  REPLACE "    if (io.WantCaptureKeyboard)"
          "    if (true) // KiriksCFD: was io.WantCaptureKeyboard (stale at frame start; broke InputText)"
          KIRIKS_RLIMGUI_SRC "${KIRIKS_RLIMGUI_SRC}")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(WRITE "${KIRIKS_RLIMGUI_CPP}" "${KIRIKS_RLIMGUI_SRC}")

FetchContent_Declare(
  tinyexpr
  GIT_REPOSITORY https://github.com/codeplea/tinyexpr.git
  GIT_TAG master
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(tinyexpr)

add_library(
  imgui_core STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
)
target_include_directories(imgui_core PUBLIC ${imgui_SOURCE_DIR})

add_library(rlimgui_bridge STATIC "${KIRIKS_RLIMGUI_CPP}")
target_include_directories(rlimgui_bridge PUBLIC ${rlimgui_SOURCE_DIR} ${imgui_SOURCE_DIR})
target_link_libraries(rlimgui_bridge PUBLIC raylib imgui_core)
target_compile_definitions(rlimgui_bridge PRIVATE NO_FONT_AWESOME=1)

set(KIRIKS_SRC
    src/main.cpp
    src/app/App.cpp
    src/app/CameraController.cpp
    src/math/VecField3D.cpp
    src/math/Streamline.cpp
    src/math/Derivatives.cpp
    src/cases/VectorFieldCase.cpp
    src/cases/PipeFlowCase.cpp
    src/cases/ShallowWaterCase.cpp
    src/cases/EkmanCase.cpp
    src/cases/CouetteFlowCase.cpp
    src/cases/StreamfunctionVorticityCase.cpp
    src/cases/InclinedFilmFlowCase.cpp
    src/cases/RossbyWaveCase.cpp
    src/cases/InertiaGravityWaveCase.cpp
    src/cases/ThermalWindCase.cpp
    src/cases/HelicityCase.cpp
    src/cases/StreamlineGeometry3DCase.cpp
    src/cases/DiffusionSimilarityCase.cpp
    src/cases/SaddleStreamfunctionCase.cpp
    src/cases/VorticityEquationCase.cpp
    src/cases/CoordinateOperatorsCase.cpp
    src/cases/SphericalFlowCase.cpp
    src/cases/GeostrophicBalanceCase.cpp
    src/cases/RossbyRadiusCase.cpp
    src/cases/RotatingForceBalanceCase.cpp
    src/cases/BruntVaisalaCase.cpp
    src/cases/BoundaryLayerCase.cpp
    src/cases/StokesFlowCase.cpp
    src/cases/ReynoldsScalingCase.cpp
    src/cases/KelvinWaveCase.cpp
    src/cases/QGPVCase.cpp
    src/cases/BarotropicInstabilityCase.cpp
    src/cases/BaroclinicInstabilityCase.cpp
    src/cases/OceanCirculationCase.cpp
    src/cases/MoistThermodynamicsCase.cpp
    src/cases/StressTensorCase.cpp
    src/cases/AtmosphericColumnCase.cpp
    src/render/ArrowRenderer.cpp
    src/render/SurfaceRenderer.cpp
    src/render/TubeRenderer.cpp
    src/render/ColorMap.cpp
    src/ui/UiPanel.cpp
    src/ui/CaseNoteLinks.cpp
)

add_executable(KiriksCFD ${KIRIKS_SRC} ${tinyexpr_SOURCE_DIR}/tinyexpr.c)
target_include_directories(
  KiriksCFD PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${tinyexpr_SOURCE_DIR}
)
target_link_libraries(KiriksCFD PRIVATE rlimgui_bridge)
set_source_files_properties(${tinyexpr_SOURCE_DIR}/tinyexpr.c PROPERTIES LANGUAGE C)
