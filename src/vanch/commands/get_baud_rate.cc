#include "command.h"

#include <imgui.h>

namespace vanch {

[[nodiscard]] static uint32_t getBaudRate(const uint8_t code) {
  static const uint32_t baudRates[] = {9600, 19200, 38400, 57600, 115200};
  return (code < 5) ? baudRates[code] : 0;
}

std::vector<uint8_t> CmdGetBaudRate::serializeParameters() const {
  return {interfaceType};
}

void CmdGetBaudRate::render() {
  static const char* interfaceItems[] = {"RS232 (0x00)", "RS485 (0x01)"};

  int temp = interfaceType;
  if (ImGui::Combo("Interface Type", &temp, interfaceItems, IM_ARRAYSIZE(interfaceItems))) {
    interfaceType = temp;
  }
}

void RetGetBaudRate::deserializeParameters(const std::span<const uint8_t> data) {
  if (!data.empty()) {
    baudRateCode = data[0];
  }
}

void RetGetBaudRate::render() {
  ImGui::Text("Baud Rate Code: %u", baudRateCode);
  ImGui::Text("Baud Rate: %u bps", getBaudRate(baudRateCode));
}

} // namespace vanch
