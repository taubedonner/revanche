#include <imgui.h>

#include "command.h"
#include "imgui_memory_editor.h"

namespace vanch {

std::vector<uint8_t> CmdGetRelayAutoControlParams::serializeParameters() const { return {relayNumber}; }

void CmdGetRelayAutoControlParams::render() {
  int temp = relayNumber;
  if (ImGui::InputInt("Relay Number", &temp)) {
    relayNumber = temp;
  }
}

void RetGetRelayAutoControlParams::deserializeParameters(const std::span<const uint8_t> data) {
  if (data.size() >= 2) {
    relayPurpose = data[0];
    pickupTime = data[1];
  }
}

void RetGetRelayAutoControlParams::render() {
  static const char* purposeItems[] = {"No auto control", "Activate on tag read", "Activate on alarm condition"};
  ImGui::Text("Relay Purpose: %s", purposeItems[relayPurpose]);
  ImGui::Text("Pickup Time: %u seconds", pickupTime);
}

}  // namespace vanch
