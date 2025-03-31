#include <imgui.h>

#include "command.h"

namespace vanch {

std::vector<uint8_t> CmdSetHeartbeatPacket::serializeParameters() const {
  std::vector<uint8_t> result;
  result.push_back(isEnabled);
  result.push_back(interval);
  result.insert(result.end(), heartbeatData.begin(), heartbeatData.end());
  return result;
}

void CmdSetHeartbeatPacket::render() {
  static const char* enableItems[] = {"Disabled (0x00)", "Enabled (0x01)"};
  int tempEnabled = isEnabled;
  if (ImGui::Combo("Enable Heartbeat", &tempEnabled, enableItems, IM_ARRAYSIZE(enableItems))) {
    isEnabled = tempEnabled;
  }

  int tempInterval = interval;
  if (ImGui::InputInt("Interval (seconds)", &tempInterval)) {
    interval = std::clamp(tempInterval, 0, 255);
  }

  char dataBuf[25] = {};
  std::copy(heartbeatData.begin(), heartbeatData.end(), dataBuf);
  if (ImGui::InputText("Heartbeat Data", dataBuf, sizeof(dataBuf))) {
    heartbeatData = dataBuf;
  }
}

}  // namespace vanch
