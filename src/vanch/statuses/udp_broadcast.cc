#include "status.h"

namespace vanch {

void StatusUdpBroadcast::render() {
  ImGui::Text("IP Address: %s", ipAddress.c_str());
  ImGui::Text("Port: %u", port);
  ImGui::Text("Device Type: %s", deviceType.c_str());
  ImGui::Text("Device ID: %s", deviceId.c_str());
  ImGui::Text("RS485 Address: %u", rs485Address);
  ImGui::Text("RS232 Baud Rate: %u", rs232BaudRate);
  ImGui::Text("RS485 Baud Rate: %u", rs485BaudRate);
  ImGui::Text("Internal Model: %u", internalModel);
}

std::vector<uint8_t> StatusUdpBroadcast::serializeParameters() const {
  const json jsonData = {{"IP", ipAddress},
                         {"Port", port},
                         {"DeviceType", deviceType},
                         {"ID", deviceId},
                         {"RS485", rs485Address},
                         {"RS232Baud", rs232BaudRate},
                         {"RS485Baud", rs485BaudRate},
                         {"ti", internalModel}};
  return serializeJson(jsonData);
}

void StatusUdpBroadcast::deserializeParameters(const std::span<const uint8_t> data) {
  const json jsonData = deserializeJson(data);
  ipAddress = jsonData.value("IP", "");
  port = jsonData.value("Port", 0);
  deviceType = jsonData.value("DeviceType", "");
  deviceId = jsonData.value("ID", "");
  rs485Address = jsonData.value("RS485", 0);
  rs232BaudRate = jsonData.value("RS232Baud", 0);
  rs485BaudRate = jsonData.value("RS485Baud", 0);
  internalModel = jsonData.value("ti", 0);
}

}  // namespace vanch
