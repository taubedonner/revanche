#include "command.h"

#include <imgui.h>

namespace vanch {

std::vector<uint8_t> CmdSetBaudRate::serializeParameters() const {
  return {interfaceType, baudRateCode};
}

void CmdSetBaudRate::render() {
  ImGui::InputScalar("Interface Type (0=RS232, 1=RS485)", ImGuiDataType_U8, &interfaceType);
  ImGui::InputScalar("Baud Rate Code", ImGuiDataType_U8, &baudRateCode);
  ImGui::Text("Baud Rate Codes: 0=9600, 1=19200, 2=38400, 3=57600, 4=115200");
}

} // namespace vanch
