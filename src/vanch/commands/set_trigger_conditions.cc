#include <imgui.h>

#include "command.h"
#include "imgui_memory_editor.h"

namespace vanch {

std::vector<uint8_t> CmdSetTriggerConditions::serializeParameters() const {
  return {triggerNumber, triggerLevel, triggerDuration};
}

void CmdSetTriggerConditions::render() {
  int temp = triggerNumber;
  if (ImGui::InputInt("Trigger Number", &temp)) {
    triggerNumber = std::clamp<uint8_t>(temp, 1, 255);
  }

  temp = triggerLevel;
  if (ImGui::Combo("Trigger Level", &temp, "Low\0High\0")) {
    triggerLevel = temp;
  }

  temp = triggerDuration;
  if (ImGui::InputInt("Trigger Duration (seconds)", &temp)) {
    triggerDuration = std::clamp<uint8_t>(temp, 0, 255);
  }
}

}  // namespace vanch
