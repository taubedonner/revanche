#include <imgui.h>

#include "command.h"

namespace vanch {

std::vector<uint8_t> CmdSetRS485Address::serializeParameters() const { return {address}; }

void CmdSetRS485Address::render() {
  static const char* tooltip =
      "Valid range: 1-254\n"
      "0 and 255 are reserved for broadcast addresses";

  int temp = address;
  if (ImGui::InputInt("RS485 Address", &temp)) {
    address = std::clamp(temp, 1, 254);
  }

  ImGui::SameLine();
  ImGui::Text("(0x%02X)", address);

  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();
    ImGui::Text("%s", tooltip);
    ImGui::EndTooltip();
  }
}

}  // namespace vanch
