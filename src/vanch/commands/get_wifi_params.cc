#include <imgui.h>

#include "command.h"

namespace vanch {

void RetGetWIFIParams::deserializeParameters(const std::span<const uint8_t> data) {
  size_t offset = 0;
  if (data.size() >= 21) {
    wifiMode = data[offset++];
    std::copy_n(data.begin() + offset, 4, localIP.begin());
    offset += 4;
    std::copy_n(data.begin() + offset, 4, subnetMask.begin());
    offset += 4;
    std::copy_n(data.begin() + offset, 4, gateway.begin());
    offset += 4;
    port = (data[offset] << 8) | data[offset + 1];
    offset += 2;
    std::copy_n(data.begin() + offset, 4, remoteIP.begin());
    offset += 4;
    ssidLength = data[offset++];
    ssid.assign(data.begin() + offset, data.begin() + offset + ssidLength);
    offset += ssidLength;
    passwordLength = data[offset++];
    password.assign(data.begin() + offset, data.begin() + offset + passwordLength);
  }
}

void RetGetWIFIParams::render() {
  static const char* modeText[] = {"UDP Server", "TCP Server", "UDP Client", "TCP Client"};
  const char* text = (wifiMode < 4) ? modeText[wifiMode] : "Unknown";

  ImGui::Text("WIFI Mode: %s (%u)", text, wifiMode);
  ImGui::Text("Local IP: %d.%d.%d.%d", localIP[0], localIP[1], localIP[2], localIP[3]);
  ImGui::Text("Subnet Mask: %d.%d.%d.%d", subnetMask[0], subnetMask[1], subnetMask[2], subnetMask[3]);
  ImGui::Text("Gateway: %d.%d.%d.%d", gateway[0], gateway[1], gateway[2], gateway[3]);
  ImGui::Text("Port: %u", port);
  ImGui::Text("Remote IP: %d.%d.%d.%d", remoteIP[0], remoteIP[1], remoteIP[2], remoteIP[3]);
  ImGui::Text("SSID: %s", ssid.c_str());
  ImGui::Text("Password: %s", std::string(passwordLength, '*').c_str());
}

}  // namespace vanch
