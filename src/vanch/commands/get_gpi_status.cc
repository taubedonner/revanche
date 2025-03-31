#include <imgui.h>

#include "command.h"

namespace vanch {

std::vector<uint8_t> CmdGetGPIStatus::serializeParameters() const { return {gpiNumber}; }

void CmdGetGPIStatus::render() {
  int tempGPI = gpiNumber;
  if (ImGui::InputInt("GPI Number", &tempGPI)) {
    gpiNumber = std::max(1, tempGPI);  // Номер GPI начинается с 1
  }
}

void RetGetGPIStatus::deserializeParameters(const std::span<const uint8_t> data) {
  if (!data.empty()) {
    gpiLevel = data[0];
  }
}

void RetGetGPIStatus::render() {
  ImGui::Text("GPI Level: %s", (gpiLevel == 0) ? "Low" : "High");
}

}  // namespace vanch
