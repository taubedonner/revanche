#include <imgui.h>

#include "command.h"

namespace vanch {

void RetGetSuperNetworkParams::deserializeParameters(const std::span<const uint8_t> data) {
  if (data.size() >= 19) {
    mode = data[0];
    std::copy_n(data.begin() + 1, 4, ipAddress.begin());
    std::copy_n(data.begin() + 5, 4, subnetMask.begin());
    std::copy_n(data.begin() + 9, 4, gateway.begin());
    port = (data[13] << 8) | data[14];
    std::copy_n(data.begin() + 15, 4, remoteIP.begin());
  }
}

void RetGetSuperNetworkParams::render() {
  static const char* modeText[] = {"UDP Server", "TCP Server", "UDP Client", "TCP Client"};
  const char* text = (mode < 4) ? modeText[mode] : "Unknown";

  ImGui::Text("Network Mode: %s (%u)", text, mode);
  ImGui::Text("IP Address: %d.%d.%d.%d", ipAddress[0], ipAddress[1], ipAddress[2], ipAddress[3]);
  ImGui::Text("Subnet Mask: %d.%d.%d.%d", subnetMask[0], subnetMask[1], subnetMask[2], subnetMask[3]);
  ImGui::Text("Gateway: %d.%d.%d.%d", gateway[0], gateway[1], gateway[2], gateway[3]);
  ImGui::Text("Port: %u", port);
  ImGui::Text("Remote IP: %d.%d.%d.%d", remoteIP[0], remoteIP[1], remoteIP[2], remoteIP[3]);
}

}  // namespace vanch
