#include "status.h"

namespace vanch {

void StatusAutoCardReading::render() {
  ImGui::Text("Antenna Number: %u", antennaNumber);
  ImGui::Text("Triggered Channels: ");
  for (const auto channel : triggeredChannels) {
    ImGui::Text("%u ", channel);
  }
  ImGui::Text("Direction: %s", direction.c_str());
  ImGui::Text("IP Address: %s", ipAddress.c_str());
  ImGui::Text("EPC: %s", epc.c_str());
  ImGui::Text("TID: %s", tid.c_str());
  ImGui::Text("User Area: %s", userArea.c_str());
  ImGui::Text("Device ID: %s", deviceId.c_str());
  ImGui::Text("RSSI: %d", rssi);
  ImGui::Text("Timestamp: %u", timestamp);
  ImGui::Text("Tag Type: 0x%02X", tagType);
  ImGui::Text("Custom Fields: ");
  for (const auto& field : customFields) {
    ImGui::Text("%s", field.c_str());
  }
  ImGui::Text("Temperature: %.2f", temperature);
}

std::vector<uint8_t> StatusAutoCardReading::serializeParameters() const {
  const json jsonData = {{"Ant", antennaNumber},
                         {"FIN", triggeredChannels},
                         {"Door", direction},
                         {"IP", ipAddress},
                         {"EPC", epc},
                         {"TID", tid},
                         {"USER", userArea},
                         {"ID", deviceId},
                         {"RSSI", rssi},
                         {"TS", timestamp},
                         {"TagType", tagType},
                         {"Custom1", customFields.size() > 0 ? customFields[0] : ""},
                         {"Custom2", customFields.size() > 1 ? customFields[1] : ""},
                         {"Custom3", customFields.size() > 2 ? customFields[2] : ""},
                         {"Custom4", customFields.size() > 3 ? customFields[3] : ""},
                         {"Custom5", customFields.size() > 4 ? customFields[4] : ""},
                         {"Temp", temperature}};
  return serializeJson(jsonData);
}

void StatusAutoCardReading::deserializeParameters(const std::span<const uint8_t> data) {
  const json jsonData = deserializeJson(data);
  antennaNumber = jsonData.value("Ant", 0);
  triggeredChannels = jsonData.value("FIN", std::vector<uint8_t>());
  direction = jsonData.value("Door", "");
  ipAddress = jsonData.value("IP", "");
  epc = jsonData.value("EPC", "");
  tid = jsonData.value("TID", "");
  userArea = jsonData.value("USER", "");
  deviceId = jsonData.value("ID", "");
  rssi = jsonData.value("RSSI", 0);
  timestamp = jsonData.value("TS", 0);
  tagType = jsonData.value("TagType", 0);
  customFields = {jsonData.value("Custom1", ""), jsonData.value("Custom2", ""), jsonData.value("Custom3", ""),
                  jsonData.value("Custom4", ""), jsonData.value("Custom5", "")};
  temperature = jsonData.value("Temp", 0.0f);
}

}  // namespace vanch