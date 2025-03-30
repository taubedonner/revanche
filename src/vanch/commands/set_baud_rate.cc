#include <imgui.h>

#include "command.h"

namespace vanch {

std::vector<uint8_t> CmdSetBaudRate::serializeParameters() const { return {interfaceType, baudRateCode}; }

void CmdSetBaudRate::render() {
  static const char* interfaceItems[] = {"RS232 (0x00)", "RS485 (0x01)"};
  static const char* baudRateItems[] = {"9600 (0x00)", "19200 (0x01)", "38400 (0x02)", "57600 (0x03)", "115200 (0x04)"};

  int temp = interfaceType;
  if (ImGui::Combo("Interface Type", &temp, interfaceItems, IM_ARRAYSIZE(interfaceItems))) {
    interfaceType = temp;
  }

  temp = baudRateCode;
  if (ImGui::Combo("Baud Rate", &temp, baudRateItems, IM_ARRAYSIZE(baudRateItems))) {
    baudRateCode = temp;
  }
}

uint32_t CmdSetBaudRate::getBaudRate(const uint8_t code) {
  static const uint32_t baudRates[] = {9600, 19200, 38400, 57600, 115200};
  return (code < 5) ? baudRates[code] : 0;
}

}  // namespace vanch
