#include <imgui.h>

#include "command.h"

namespace vanch {

std::vector<uint8_t> CmdSetReaderName::serializeParameters() const {
  std::vector<uint8_t> result;
  result.insert(result.end(), readerName.begin(), readerName.end());
  return result;
}

void CmdSetReaderName::render() {
  char nameBuf[25] = {};
  std::copy(readerName.begin(), readerName.end(), nameBuf);
  if (ImGui::InputText("Reader Name", nameBuf, sizeof(nameBuf))) {
    readerName = nameBuf;
  }
}

}  // namespace vanch
