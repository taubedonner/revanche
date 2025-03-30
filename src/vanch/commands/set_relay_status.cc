#include <imgui.h>

#include "command.h"

namespace vanch {

std::vector<uint8_t> CmdSetRelayStatus::serializeParameters() const { return {relayNumber, relayState}; }

void CmdSetRelayStatus::render() {
  static const char* stateItems[] = {"Off (0x00)", "On (0x01)"};

  int tempNumber = relayNumber;
  if (ImGui::InputInt("Relay Number", &tempNumber)) {
    relayNumber = std::max(1, tempNumber);
  }

  int tempState = relayState;
  if (ImGui::Combo("Relay State", &tempState, stateItems, IM_ARRAYSIZE(stateItems))) {
    relayState = tempState;
  }
}

}  // namespace vanch