#include <imgui.h>

#include "command.h"

namespace vanch {

void RetGetMACAddress::deserializeParameters(const std::span<const uint8_t> data) {
  if (data.size() >= 6) {
    std::copy_n(data.begin(), 6, mac.begin());
  }
}

void RetGetMACAddress::render() {
  ImGui::Text("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

}  // namespace vanch
