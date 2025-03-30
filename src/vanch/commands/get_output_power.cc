#include <imgui.h>

#include "command.h"

namespace vanch {

std::vector<uint8_t> CmdGetOutputPower::serializeParameters() const { return {antennaNumber}; }

void CmdGetOutputPower::render() {
  int temp = antennaNumber;
  if (ImGui::InputInt("Antenna Number", &temp)) {
    antennaNumber = std::max(1, temp);
  }
}

void RetGetOutputPower::deserializeParameters(const std::span<const uint8_t> data) {
  if (!data.empty()) {
    powerValue = data[0];
  }
}

void RetGetOutputPower::render() { ImGui::Text("Output Power: %u dBm", powerValue); }

}  // namespace vanch