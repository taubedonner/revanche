#include <imgui.h>

#include "command.h"

namespace vanch {

std::vector<uint8_t> CmdGetRelayStatus::serializeParameters() const { return {relayNumber}; }

void CmdGetRelayStatus::render() {
  int temp = relayNumber;
  if (ImGui::InputInt("Relay Number", &temp)) {
    relayNumber = std::max(1, temp);
  }
}

void RetGetRelayStatus::deserializeParameters(const std::span<const uint8_t> data) {
  if (!data.empty()) {
    relayState = data[0];
  }
}

void RetGetRelayStatus::render() {
  static const char* stateText[] = {"Off", "On"};
  const char* text = (relayState < 2) ? stateText[relayState] : "Unknown";
  ImGui::Text("Relay State: %s (%u)", text, relayState);
}

}  // namespace vanch