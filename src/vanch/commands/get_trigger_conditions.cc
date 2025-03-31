#include <imgui.h>

#include "command.h"
#include "imgui_memory_editor.h"

namespace vanch {

std::vector<uint8_t> CmdGetTriggerConditions::serializeParameters() const { return {triggerNumber}; }

void CmdGetTriggerConditions::render() {
  int temp = triggerNumber;
  if (ImGui::InputInt("Trigger Number", &temp)) {
    triggerNumber = std::clamp<uint8_t>(temp, 1, 255);
  }
}

void RetGetTriggerConditions::deserializeParameters(const std::span<const uint8_t> data) {
  if (data.size() >= 2) {
    triggerLevel = data[0];
    triggerDuration = data[1];
  }
}

void RetGetTriggerConditions::render() {
  ImGui::Text("Trigger Level: %s", triggerLevel ? "High" : "Low");
  ImGui::Text("Trigger Duration: %u seconds", triggerDuration);
}

}  // namespace vanch
