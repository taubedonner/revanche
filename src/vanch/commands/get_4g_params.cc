#include <imgui.h>

#include "command.h"

namespace vanch {

void RetGet4GParams::deserializeParameters(const std::span<const uint8_t> data) {
  if (data.size() >= 5) {
    transmissionMode = data[0];
    port = (data[1] << 8) | data[2];
    addressLength = data[3];
    address.assign(data.begin() + 4, data.begin() + 4 + addressLength);
  }
}

void RetGet4GParams::render() {
  static const char* modeText[] = {"UDP Client", "TCP Client"};
  const char* text = (transmissionMode < 2) ? modeText[transmissionMode] : "Unknown";

  ImGui::Text("Transmission Mode: %s (%u)", text, transmissionMode);
  ImGui::Text("Port: %u", port);
  ImGui::Text("Address/IP: %s", address.c_str());
}

}  // namespace vanch
