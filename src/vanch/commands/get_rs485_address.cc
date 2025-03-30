#include <imgui.h>

#include "command.h"

namespace vanch {

void RetGetRS485Address::deserializeParameters(const std::span<const uint8_t> data) {
  if (!data.empty()) {
    address = data[0];
  }
}

void RetGetRS485Address::render() { ImGui::Text("RS485 Address: %u (0x%02X)", address, address); }

}  // namespace vanch