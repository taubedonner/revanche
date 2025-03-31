#include <imgui.h>

#include "command.h"

namespace vanch {

std::vector<uint8_t> CmdSetReaderID::serializeParameters() const {
  std::vector<uint8_t> result;
  result.insert(result.end(), readerID.begin(), readerID.end());
  return result;
}

void CmdSetReaderID::render() {
  char idBuf[25] = {};
  std::copy(readerID.begin(), readerID.end(), idBuf);
  if (ImGui::InputText("Reader ID", idBuf, sizeof(idBuf))) {
    readerID = idBuf;
  }
}

}  // namespace vanch
