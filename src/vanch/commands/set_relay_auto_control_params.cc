#include <imgui.h>

#include "command.h"
#include "imgui_memory_editor.h"

namespace vanch {

std::vector<uint8_t> CmdSetRelayAutoControlParams::serializeParameters() const {
  return {relayNumber, relayPurpose, pickupTime};
}

void CmdSetRelayAutoControlParams::render() {
  static const char* purposeItems[] = {"No auto control (0x00)", "Activate on tag read (0x01)",
                                       "Activate on alarm condition (0x02)"};

  int temp = relayNumber;
  if (ImGui::InputInt("Relay Number", &temp)) {
    relayNumber = temp;
  }

  temp = relayPurpose;
  if (ImGui::Combo("Relay Purpose", &temp, purposeItems, IM_ARRAYSIZE(purposeItems))) {
    relayPurpose = temp;
  }

  temp = pickupTime;
  if (ImGui::InputInt("Pickup Time (seconds)", &temp)) {
    pickupTime = temp;
  }
}

}  // namespace vanch
