#include <imgui.h>

#include "command.h"
#include "imgui_memory_editor.h"

namespace vanch {

std::vector<uint8_t> CmdSelect14443ASector::serializeParameters() const {
  std::vector<uint8_t> result = {uidLength};
  result.insert(result.end(), uid.begin(), uid.end());
  result.push_back(sectorNumber);
  return result;
}

void CmdSelect14443ASector::render() {
  int temp = uidLength;
  if (ImGui::InputInt("UID Length", &temp)) {
    uidLength = std::clamp(temp, 0, 255);
  }

  if (uidLength > uid.size()) {
    uid.resize(uidLength, 0);
  } else if (uidLength < uid.size()) {
    uid.resize(uidLength);
  }

  temp = sectorNumber;
  if (ImGui::InputInt("Sector Number", &temp)) {
    sectorNumber = std::clamp(temp, 0, 255);
  }

  static MemoryEditor mem_edit;
  mem_edit.ReadOnly = false;
  ImGui::Spacing();
  mem_edit.DrawChildWindow("UID Editor", uid.data(), uid.size());
}

}  // namespace vanch
