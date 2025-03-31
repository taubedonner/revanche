#include <imgui.h>

#include "command.h"
#include "imgui_memory_editor.h"

namespace vanch {

std::vector<uint8_t> CmdSetAutoReportingFields::serializeParameters() const {
  return {static_cast<uint8_t>((fieldBitmap >> 24) & 0xFF),
          static_cast<uint8_t>((fieldBitmap >> 16) & 0xFF),
          static_cast<uint8_t>((fieldBitmap >> 8) & 0xFF),
          static_cast<uint8_t>(fieldBitmap & 0xFF),
          tidStartAddress,
          tidLength,
          userStartAddress,
          userLength};
}

void CmdSetAutoReportingFields::render() {
  ImGui::Text("Select Reporting Fields:");

  ImGui::CheckboxFlags("EPC (UID)", &fieldBitmap, 1 << 0);   // BIT0
  ImGui::CheckboxFlags("TID", &fieldBitmap, 1 << 1);         // BIT1
  ImGui::CheckboxFlags("USER", &fieldBitmap, 1 << 2);        // BIT2
  ImGui::CheckboxFlags("RSSI", &fieldBitmap, 1 << 3);        // BIT3
  ImGui::CheckboxFlags("Timestamp", &fieldBitmap, 1 << 4);   // BIT4
  ImGui::CheckboxFlags("Tag Type", &fieldBitmap, 1 << 5);    // BIT5
  ImGui::CheckboxFlags("IP Address", &fieldBitmap, 1 << 6);  // BIT6

  int temp = tidStartAddress;
  if (ImGui::InputInt("TID Start Address", &temp)) {
    tidStartAddress = std::clamp<uint8_t>(temp, 0, 255);
  }

  temp = tidLength;
  if (ImGui::InputInt("TID Length", &temp)) {
    tidLength = std::clamp<uint8_t>(temp, 0, 255);
  }

  temp = userStartAddress;
  if (ImGui::InputInt("USER Start Address", &temp)) {
    userStartAddress = std::clamp<uint8_t>(temp, 0, 255);
  }

  temp = userLength;
  if (ImGui::InputInt("USER Length", &temp)) {
    userLength = std::clamp<uint8_t>(temp, 0, 255);
  }
}

}  // namespace vanch
