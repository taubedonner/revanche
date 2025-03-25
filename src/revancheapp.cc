#include "revancheapp.h"

void RevancheApp::OnAttach() {
  Layer::OnAttach();
}

void RevancheApp::OnDetach() {
  Layer::OnDetach();
}

void RevancheApp::OnUiUpdate() {
  constexpr auto flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus;
  const auto viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->Pos + ImVec2(0, ImGui::GetFrameHeight()));
  ImGui::SetNextWindowSize(viewport->Size - ImVec2(0, ImGui::GetFrameHeight()));
  ImGui::SetNextWindowViewport(viewport->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  if (ImGui::Begin("##FullscreenWindow", nullptr, flags)) {
    ImGui::PopStyleVar(2);
    ImGui::TextUnformatted("ddfsdf");
  }
  ImGui::End();
}
