#include <imgui.h>

#include "command.h"

namespace vanch {

std::vector<uint8_t> CmdSetRJ45LocalParams::serializeParameters() const {
  std::vector<uint8_t> result;
  result.insert(result.end(), ip.begin(), ip.end());
  result.insert(result.end(), mask.begin(), mask.end());
  result.insert(result.end(), gateway.begin(), gateway.end());
  result.push_back((port >> 8) & 0xFF);
  result.push_back(port & 0xFF);
  return result;
}

void CmdSetRJ45LocalParams::render() {
  renderIPAddress("IP Address", ip);
  renderIPAddress("Subnet Mask", mask);
  renderIPAddress("Gateway", gateway);

  int tempPort = port;
  if (ImGui::InputInt("Port", &tempPort)) {
    port = std::clamp(tempPort, 1, 65535);
  }
}

void CmdSetRJ45LocalParams::renderIPAddress(const char* label, std::array<uint8_t, 4>& ip) {
  static char buf[16];
  sprintf_s(buf, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  if (ImGui::InputText(label, buf, sizeof(buf))) {
    sscanf_s(buf, "%hhu.%hhu.%hhu.%hhu", &ip[0], &ip[1], &ip[2], &ip[3]);
  }
}

}  // namespace vanch
