#include <imgui.h>

#include "command.h"

namespace vanch {

std::vector<uint8_t> CmdSetCardReadingMode::serializeParameters() const { return {mode}; }

void CmdSetCardReadingMode::render() {
  static const char* modes[] = {"Command Mode (0x00)", "Automatic Mode (Continuous) (0x01)",
                                "Automatic Mode (Switch Trigger) (0x02)", "Automatic Mode (Two-way Trigger) (0x03)"};

  int temp = mode;
  if (ImGui::Combo("Card Reading Mode", &temp, modes, IM_ARRAYSIZE(modes))) {
    mode = temp;
  }
}

}  // namespace vanch