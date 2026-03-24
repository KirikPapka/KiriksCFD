#include "ui/UiPanel.hpp"
#include "ui/CaseNoteLinks.hpp"

#include "imgui.h"
#include "raylib.h"

#include <algorithm>
#include <cfloat>

void UiPanel::draw(std::vector<std::unique_ptr<ICase>>& cases, int& currentCaseIndex) {
  float w = static_cast<float>(GetScreenWidth());
  float h = static_cast<float>(GetScreenHeight());
  const float defaultW = 380.0f;
  ImGui::SetNextWindowPos(ImVec2(std::max(40.0f, w - defaultW), 0.0f), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(defaultW, h), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSizeConstraints(ImVec2(280.0f, 220.0f), ImVec2(FLT_MAX, FLT_MAX));

  ImGuiWindowFlags flags = ImGuiWindowFlags_None;
  ImGui::Begin("KiriksCFD", nullptr, flags);
  ImGui::TextUnformatted("Exam prep: canonical flows (analytic only).");
  ImGui::Separator();

  if (cases.empty()) {
    ImGui::TextUnformatted("No cases loaded.");
    ImGui::End();
    return;
  }

  currentCaseIndex = std::clamp(currentCaseIndex, 0, static_cast<int>(cases.size()) - 1);

  const char* preview = cases[static_cast<size_t>(currentCaseIndex)]->name();
  if (ImGui::BeginCombo("case", preview)) {
    for (int i = 0; i < static_cast<int>(cases.size()); ++i) {
      bool sel = (i == currentCaseIndex);
      if (ImGui::Selectable(cases[static_cast<size_t>(i)]->name(), sel)) {
        currentCaseIndex = i;
      }
      if (sel) {
        ImGui::SetItemDefaultFocus();
      }
    }
    ImGui::EndCombo();
  }
  ImGui::TextWrapped(
      "Many cases have a \"Custom\" field mode (tinyexpr): check the case panel for eta, psi, u(z), or "
      "u(r). The \"3D vector field\" case is the full u_x, u_y, u_z cube.");

  ImGui::Spacing();
  ImGui::TextUnformatted("About this case");
  ImGui::TextWrapped("%s", cases[static_cast<size_t>(currentCaseIndex)]->description());

  ImGui::Spacing();
  ImGui::TextUnformatted("In your lecture notes");
  ImGui::TextWrapped(
      "MTH3007 = Fluid Dynamics; MTH3001 = Theory of Weather & Climate. Adjust chapters in "
      "src/ui/CaseNoteLinks.cpp if yours differ.");
  const char* refs = noteRefsForCaseName(cases[static_cast<size_t>(currentCaseIndex)]->name());
  if (refs != nullptr) {
    ImGui::TextWrapped("%s", refs);
  }

  ImGui::Separator();
  cases[static_cast<size_t>(currentCaseIndex)]->drawUI();
  ImGui::End();
}
