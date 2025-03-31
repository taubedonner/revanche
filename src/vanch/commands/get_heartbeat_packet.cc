#include <imgui.h>

#include "command.h"

namespace vanch {

void RetGetHeartbeatPacket::deserializeParameters(const std::span<const uint8_t> data) {
  if (data.size() >= 2) {
    isEnabled = data[0];
    interval = data[1];
    heartbeatData.assign(data.begin() + 2, data.end());
  }
}

void RetGetHeartbeatPacket::render() {
  ImGui::Text("Heartbeat Enabled: %s", isEnabled ? "Yes" : "No");
  ImGui::Text("Interval: %u seconds", interval);
  ImGui::Text("Heartbeat Data: %s", heartbeatData.c_str());
}

}  // namespace vanch
