#include <imgui.h>

#include "command.h"

namespace vanch {

void RetGetRJ45LocalParams::deserializeParameters(const std::span<const uint8_t> data) {
  if (data.size() >= 14) {
    std::copy_n(data.begin(), 4, ip.begin());
    std::copy_n(data.begin() + 4, 4, mask.begin());
    std::copy_n(data.begin() + 8, 4, gateway.begin());
    port = (data[12] << 8) | data[13];
  }
}

void RetGetRJ45LocalParams::render() {
  ImGui::Text("IP Address: %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  ImGui::Text("Subnet Mask: %d.%d.%d.%d", mask[0], mask[1], mask[2], mask[3]);
  ImGui::Text("Gateway: %d.%d.%d.%d", gateway[0], gateway[1], gateway[2], gateway[3]);
  ImGui::Text("Port: %u", port);
}

}  // namespace vanch
