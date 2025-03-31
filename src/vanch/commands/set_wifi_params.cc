#include <imgui.h>

#include "command.h"

namespace vanch {

std::vector<uint8_t> CmdSetWIFIParams::serializeParameters() const {
  std::vector<uint8_t> result;
  result.push_back(wifiMode);
  result.insert(result.end(), localIP.begin(), localIP.end());
  result.insert(result.end(), subnetMask.begin(), subnetMask.end());
  result.insert(result.end(), gateway.begin(), gateway.end());
  result.push_back((port >> 8) & 0xFF);
  result.push_back(port & 0xFF);
  result.insert(result.end(), remoteIP.begin(), remoteIP.end());
  result.push_back(ssidLength);
  result.insert(result.end(), ssid.begin(), ssid.end());
  result.push_back(passwordLength);
  result.insert(result.end(), password.begin(), password.end());
  return result;
}

void CmdSetWIFIParams::render() {
  static const char* modeItems[] = {"UDP Server (0x00)", "TCP Server (0x01)", "UDP Client (0x02)", "TCP Client (0x03)"};

  int tempMode = wifiMode;
  if (ImGui::Combo("WIFI Mode", &tempMode, modeItems, IM_ARRAYSIZE(modeItems))) {
    wifiMode = tempMode;
  }

  CmdSetRJ45LocalParams::renderIPAddress("Local IP", localIP);
  CmdSetRJ45LocalParams::renderIPAddress("Subnet Mask", subnetMask);
  CmdSetRJ45LocalParams::renderIPAddress("Gateway", gateway);

  int tempPort = port;
  if (ImGui::InputInt("Port", &tempPort)) {
    port = std::clamp(tempPort, 1, 65535);
  }

  CmdSetRJ45LocalParams::renderIPAddress("Remote IP", remoteIP);

  char ssidBuf[33] = {};
  std::copy(ssid.begin(), ssid.end(), ssidBuf);
  if (ImGui::InputText("SSID", ssidBuf, sizeof(ssidBuf))) {
    ssid = ssidBuf;
    ssidLength = std::min(static_cast<uint8_t>(ssid.size()), uint8_t(32));
  }

  char passwordBuf[65] = {};
  std::copy(password.begin(), password.end(), passwordBuf);
  if (ImGui::InputText("Password", passwordBuf, sizeof(passwordBuf), ImGuiInputTextFlags_Password)) {
    password = passwordBuf;
    passwordLength = std::min(static_cast<uint8_t>(password.size()), uint8_t(64));
  }
}

}  // namespace vanch
