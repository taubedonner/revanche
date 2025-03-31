#include <imgui.h>

#include "command.h"
#include "imgui_memory_editor.h"

namespace vanch {

std::vector<uint8_t> CmdSetWiegandParams::serializeParameters() const { return {pulseWidth, pulseInterval}; }

void CmdSetWiegandParams::render() {
  int temp = pulseWidth;
  if (ImGui::InputInt("Pulse Width (10us units)", &temp)) {
    pulseWidth = std::clamp<uint8_t>(temp, 0, 255);
  }

  temp = pulseInterval;
  if (ImGui::InputInt("Pulse Interval (10us units)", &temp)) {
    pulseInterval = std::clamp<uint8_t>(temp, 0, 255);
  }
}

}  // namespace vanch
