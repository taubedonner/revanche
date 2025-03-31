#include <imgui.h>

#include "command.h"
#include "imgui_memory_editor.h"

namespace vanch {

std::vector<uint8_t> CmdGetReportedHardwareInterface::serializeParameters() const { return {interfaceNumber}; }

void CmdGetReportedHardwareInterface::render() {
  static const char* interfaceItems[] = {"RS232 (0x00)",      "RS485 (0x01)", "RJ45 (0x02)", "Wiegand 26 (0x03)",
                                         "Wiegand 34 (0x04)", "WIFI (0x05)",  "4G (0x06)"};

  int temp = interfaceNumber;
  if (ImGui::Combo("Interface Number", &temp, interfaceItems, IM_ARRAYSIZE(interfaceItems))) {
    interfaceNumber = temp;
  }
}

void RetGetReportedHardwareInterface::deserializeParameters(const std::span<const uint8_t> data) {
  if (!data.empty()) {
    enableReporting = data[0];
  }
}

void RetGetReportedHardwareInterface::render() { ImGui::Text("Reporting Enabled: %s", enableReporting ? "Yes" : "No"); }

}  // namespace vanch
