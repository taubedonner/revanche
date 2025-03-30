#include <imgui.h>

#include "command.h"

namespace vanch {

std::vector<uint8_t> CmdSetOutputPower::serializeParameters() const { return {antennaNumber, powerValue}; }

void CmdSetOutputPower::render() {
  int tempAntenna = antennaNumber;
  if (ImGui::InputInt("Antenna Number", &tempAntenna)) {
    antennaNumber = std::max(1, tempAntenna);
  }

  int tempPower = powerValue;
  if (ImGui::SliderInt("Power Value (dBm)", &tempPower, 0, 33)) {
    powerValue = std::clamp(tempPower, 0, 33);
  }
}

}  // namespace vanch