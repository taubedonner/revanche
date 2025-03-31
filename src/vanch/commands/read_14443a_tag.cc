#include <imgui.h>

#include "command.h"
#include "imgui_memory_editor.h"

namespace vanch {

std::vector<uint8_t> CmdRead14443ATag::serializeParameters() const {
  std::vector<uint8_t> result = {uidLength};
  result.insert(result.end(), uid.begin(), uid.end());
  result.push_back(startBlock);
  result.push_back(numBlocks);
  return result;
}

void CmdRead14443ATag::render() {
  int temp = uidLength;
  if (ImGui::InputInt("UID Length", &temp)) {
    uidLength = std::clamp(temp, 0, 255);
  }

  if (uidLength > uid.size()) {
    uid.resize(uidLength, 0);
  } else if (uidLength < uid.size()) {
    uid.resize(uidLength);
  }

  temp = startBlock;
  if (ImGui::InputInt("Start Block Address", &temp)) {
    startBlock = std::clamp(temp, 0, 255);
  }

  temp = numBlocks;
  if (ImGui::InputInt("Number of Blocks", &temp)) {
    numBlocks = std::clamp(temp, 0, 255);
  }

  static MemoryEditor mem_edit;
  mem_edit.ReadOnly = false;
  ImGui::Spacing();
  mem_edit.DrawChildWindow("UID Editor", uid.data(), uid.size());
}

void RetRead14443ATag::deserializeParameters(const std::span<const uint8_t> data) {
  tagData.assign(data.begin(), data.end());
}

void RetRead14443ATag::render() {
  static MemoryEditor mem_edit;
  mem_edit.ReadOnly = true;
  mem_edit.DrawChildWindow("Read Data Preview", tagData.data(), tagData.size());
}

}  // namespace vanch
