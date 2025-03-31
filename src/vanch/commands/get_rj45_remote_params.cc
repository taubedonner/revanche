#include <imgui.h>

#include "command.h"

namespace vanch {

void RetGetRJ45RemoteParams::deserializeParameters(const std::span<const uint8_t> data) {
  if (data.size() >= 15) {
    std::copy_n(data.begin(), 4, udpServerIP.begin());
    udpServerPort = (data[4] << 8) | data[5];
    udpEnabled = data[6];
    std::copy_n(data.begin() + 7, 4, tcpServerIP.begin());
    tcpServerPort = (data[11] << 8) | data[12];
    tcpEnabled = data[13];
  }
}

void RetGetRJ45RemoteParams::render() {
  ImGui::Text("UDP Server IP: %d.%d.%d.%d", udpServerIP[0], udpServerIP[1], udpServerIP[2], udpServerIP[3]);
  ImGui::Text("UDP Server Port: %u", udpServerPort);
  ImGui::Text("UDP Reporting: %s", udpEnabled ? "Enabled" : "Disabled");

  ImGui::Text("TCP Server IP: %d.%d.%d.%d", tcpServerIP[0], tcpServerIP[1], tcpServerIP[2], tcpServerIP[3]);
  ImGui::Text("TCP Server Port: %u", tcpServerPort);
  ImGui::Text("TCP Mode: %s", tcpEnabled ? "Client" : "Server");
}

}  // namespace vanch
