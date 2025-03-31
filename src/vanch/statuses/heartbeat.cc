#include "status.h"

namespace vanch {

void StatusHeartbeat::render() { ImGui::Text("Heartbeat Data: %s", heartbeatData.c_str()); }

std::vector<uint8_t> StatusHeartbeat::serializeParameters() const {
  const json jsonData = {{"heartbeat", heartbeatData}};
  return serializeJson(jsonData);
}

void StatusHeartbeat::deserializeParameters(const std::span<const uint8_t> data) {
  const json jsonData = deserializeJson(data);
  heartbeatData = jsonData.value("heartbeat", "");
}

}  // namespace vanch
