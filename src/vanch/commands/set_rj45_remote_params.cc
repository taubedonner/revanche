#include <imgui.h>

#include "command.h"

namespace vanch {

std::vector<uint8_t> CmdSetRJ45RemoteParams::serializeParameters() const {
  std::vector<uint8_t> result;
  result.insert(result.end(), udpServerIP.begin(), udpServerIP.end());
  result.push_back((udpServerPort >> 8) & 0xFF);
  result.push_back(udpServerPort & 0xFF);
  result.push_back(udpEnabled);
  result.insert(result.end(), tcpServerIP.begin(), tcpServerIP.end());
  result.push_back((tcpServerPort >> 8) & 0xFF);
  result.push_back(tcpServerPort & 0xFF);
  result.push_back(tcpEnabled);
  return result;
}

void CmdSetRJ45RemoteParams::render() {
  CmdSetRJ45LocalParams::renderIPAddress("UDP Server IP", udpServerIP);

  int tempUDPPort = udpServerPort;
  if (ImGui::InputInt("UDP Server Port", &tempUDPPort)) {
    udpServerPort = std::clamp(tempUDPPort, 1, 65535);
  }

  static const char* udpEnableItems[] = {"Disabled (0x00)", "Enabled (0x01)"};
  int tempUdpEnabled = udpEnabled;
  if (ImGui::Combo("UDP Reporting", &tempUdpEnabled, udpEnableItems, IM_ARRAYSIZE(udpEnableItems))) {
    udpEnabled = tempUdpEnabled;
  }

  CmdSetRJ45LocalParams::renderIPAddress("TCP Server IP", tcpServerIP);

  int tempTCPPort = tcpServerPort;
  if (ImGui::InputInt("TCP Server Port", &tempTCPPort)) {
    tcpServerPort = std::clamp(tempTCPPort, 1, 65535);
  }

  static const char* tcpEnableItems[] = {"TCP Server Mode (0x00)", "TCP Client Mode (0x01)"};
  int tempTcpEnabled = tcpEnabled;
  if (ImGui::Combo("TCP Reporting", &tempTcpEnabled, tcpEnableItems, IM_ARRAYSIZE(tcpEnableItems))) {
    tcpEnabled = tempTcpEnabled;
  }
}

}  // namespace vanch
