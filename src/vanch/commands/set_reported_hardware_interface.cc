#include <imgui.h>

#include "command.h"
#include "imgui_memory_editor.h"

namespace vanch {

std::vector<uint8_t> CmdSetReportedHardwareInterface::serializeParameters() const {
  return {interfaceNumber, enableReporting};
}

void CmdSetReportedHardwareInterface::render() {
  static const char* interfaceItems[] = {"RS232 (0x00)",      "RS485 (0x01)", "RJ45 (0x02)", "Wiegand 26 (0x03)",
                                         "Wiegand 34 (0x04)", "WIFI (0x05)",  "4G (0x06)"};

  int temp = interfaceNumber;
  if (ImGui::Combo("Interface Number", &temp, interfaceItems, IM_ARRAYSIZE(interfaceItems))) {
    interfaceNumber = temp;
  }

  temp = enableReporting;
  if (ImGui::Combo("Enable Reporting", &temp, "Off\0On\0")) {
    enableReporting = temp;
  }
}

}  // namespace vanch
