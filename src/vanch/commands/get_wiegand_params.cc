#include <imgui.h>

#include "command.h"
#include "imgui_memory_editor.h"

namespace vanch {

void RetGetWiegandParams::deserializeParameters(const std::span<const uint8_t> data) {
  if (data.size() >= 2) {
    pulseWidth = data[0];
    pulseInterval = data[1];
  }
}

void RetGetWiegandParams::render() {
  ImGui::Text("Pulse Width: %u (10us units)", pulseWidth);
  ImGui::Text("Pulse Interval: %u (10us units)", pulseInterval);
}

}  // namespace vanch
