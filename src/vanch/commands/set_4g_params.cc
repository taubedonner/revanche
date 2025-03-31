#include <imgui.h>

#include "command.h"

namespace vanch {

std::vector<uint8_t> CmdSet4GParams::serializeParameters() const {
  std::vector<uint8_t> result;
  result.push_back(transmissionMode);
  result.push_back((port >> 8) & 0xFF);
  result.push_back(port & 0xFF);
  result.push_back(addressLength);
  result.insert(result.end(), address.begin(), address.end());
  return result;
}

void CmdSet4GParams::render() {
  static const char* modeItems[] = {"UDP Client (0x00)", "TCP Client (0x01)"};

  int tempMode = transmissionMode;
  if (ImGui::Combo("Transmission Mode", &tempMode, modeItems, IM_ARRAYSIZE(modeItems))) {
    transmissionMode = tempMode;
  }

  int tempPort = port;
  if (ImGui::InputInt("Port", &tempPort)) {
    port = std::clamp(tempPort, 1, 65535);
  }

  char addrBuf[65] = {};
  std::copy(address.begin(), address.end(), addrBuf);
  if (ImGui::InputText("Address/IP", addrBuf, sizeof(addrBuf))) {
    address = addrBuf;
    addressLength = std::min(static_cast<uint8_t>(address.size()), uint8_t(64));
  }
}

}  // namespace vanch
