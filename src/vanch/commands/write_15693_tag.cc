#include <imgui.h>

#include "command.h"
#include "imgui_memory_editor.h"

namespace vanch {

std::vector<uint8_t> CmdWrite15693Tag::serializeParameters() const {
  std::vector<uint8_t> result = {uidLength};
  result.insert(result.end(), uid.begin(), uid.end());
  result.push_back(writeBlock);
  result.insert(result.end(), writeData.begin(), writeData.end());
  return result;
}

void CmdWrite15693Tag::render() {
  int temp = uidLength;
  if (ImGui::InputInt("UID Length", &temp)) {
    uidLength = std::clamp(temp, 0, 255);
  }

  if (uidLength > uid.size()) {
    uid.resize(uidLength, 0);
  } else if (uidLength < uid.size()) {
    uid.resize(uidLength);
  }

  temp = writeBlock;
  if (ImGui::InputInt("Write Block Address", &temp)) {
    writeBlock = std::clamp(temp, 0, 255);
  }

  static MemoryEditor me1, me2;
  ImGui::Spacing();
  me1.DrawChildWindow("UID Editor", uid.data(), uid.size(), {ImGui::GetContentRegionAvail().x * 0.5f, 0});
  ImGui::SameLine();
  me2.DrawChildWindow("Write Data Editor (4 Bytes)", writeData.data(), writeData.size());
}

}  // namespace vanch
