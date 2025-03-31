#include <imgui.h>

#include "command.h"
#include "imgui_memory_editor.h"

namespace vanch {

void RetGetAutoReportingFields::deserializeParameters(const std::span<const uint8_t> data) {
  if (data.size() >= 8) {
    fieldBitmap = (static_cast<uint32_t>(data[0]) << 24) | (static_cast<uint32_t>(data[1]) << 16) |
                  (static_cast<uint32_t>(data[2]) << 8) | static_cast<uint32_t>(data[3]);
    tidStartAddress = data[4];
    tidLength = data[5];
    userStartAddress = data[6];
    userLength = data[7];
  }
}

void RetGetAutoReportingFields::render() {
  ImGui::Text("Selected Reporting Fields:");
  ImGui::Text("EPC (UID): %s", (fieldBitmap & (1 << 0)) ? "Enabled" : "Disabled");
  ImGui::Text("TID: %s", (fieldBitmap & (1 << 1)) ? "Enabled" : "Disabled");
  ImGui::Text("USER: %s", (fieldBitmap & (1 << 2)) ? "Enabled" : "Disabled");
  ImGui::Text("RSSI: %s", (fieldBitmap & (1 << 3)) ? "Enabled" : "Disabled");
  ImGui::Text("Timestamp: %s", (fieldBitmap & (1 << 4)) ? "Enabled" : "Disabled");
  ImGui::Text("Tag Type: %s", (fieldBitmap & (1 << 5)) ? "Enabled" : "Disabled");
  ImGui::Text("IP Address: %s", (fieldBitmap & (1 << 6)) ? "Enabled" : "Disabled");

  ImGui::Text("TID Start Address: %u", tidStartAddress);
  ImGui::Text("TID Length: %u", tidLength);
  ImGui::Text("USER Start Address: %u", userStartAddress);
  ImGui::Text("USER Length: %u", userLength);
}

}  // namespace vanch
