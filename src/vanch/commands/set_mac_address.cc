#include <imgui.h>

#include "command.h"

namespace vanch {

std::vector<uint8_t> CmdSetMACAddress::serializeParameters() const { return {mac.begin(), mac.end()}; }

void CmdSetMACAddress::render() {
  static char macStr[18];
  sprintf_s(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  if (ImGui::InputText("MAC Address", macStr, sizeof(macStr))) {
    sscanf_s(macStr, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);
  }
}

}  // namespace vanch
