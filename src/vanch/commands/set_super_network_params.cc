#include <imgui.h>

#include "command.h"

namespace vanch {

std::vector<uint8_t> CmdSetSuperNetworkParams::serializeParameters() const {
  std::vector<uint8_t> result;
  result.push_back(mode);
  result.insert(result.end(), ipAddress.begin(), ipAddress.end());
  result.insert(result.end(), subnetMask.begin(), subnetMask.end());
  result.insert(result.end(), gateway.begin(), gateway.end());
  result.push_back((port >> 8) & 0xFF);
  result.push_back(port & 0xFF);
  result.insert(result.end(), remoteIP.begin(), remoteIP.end());
  return result;
}

void CmdSetSuperNetworkParams::render() {
  static const char* modeItems[] = {"UDP Server (0x00)", "TCP Server (0x01)", "UDP Client (0x02)", "TCP Client (0x03)"};

  int tempMode = mode;
  if (ImGui::Combo("Network Mode", &tempMode, modeItems, IM_ARRAYSIZE(modeItems))) {
    mode = tempMode;
  }

  CmdSetRJ45LocalParams::renderIPAddress("IP Address", ipAddress);
  CmdSetRJ45LocalParams::renderIPAddress("Subnet Mask", subnetMask);
  CmdSetRJ45LocalParams::renderIPAddress("Gateway", gateway);

  int tempPort = port;
  if (ImGui::InputInt("Port", &tempPort)) {
    port = std::clamp(tempPort, 1, 65535);
  }

  CmdSetRJ45LocalParams::renderIPAddress("Remote IP", remoteIP);
}

}  // namespace vanch
