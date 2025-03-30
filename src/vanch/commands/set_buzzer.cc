#include <imgui.h>

#include "command.h"

namespace vanch {

std::vector<uint8_t> CmdSetBuzzer::serializeParameters() const { return {state}; }

void CmdSetBuzzer::render() {
  static const char* items[] = {"Off (0x00)", "On (0x01)"};

  int temp = state;
  if (ImGui::Combo("Buzzer State", &temp, items, IM_ARRAYSIZE(items))) {
    state = temp;
  }
}

}  // namespace vanch