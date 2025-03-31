#include <imgui.h>

#include "command.h"
#include "imgui_memory_editor.h"

namespace vanch {

std::vector<uint8_t> CmdReadISO15693Tag::serializeParameters() const {
  std::vector<uint8_t> result = {uidLength};
  result.insert(result.end(), uid.begin(), uid.end());
  result.push_back(startBlock);
  result.push_back(numBlocks);
  result.push_back(blockSize);
  return result;
}

void CmdReadISO15693Tag::render() {
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

  temp = blockSize;
  if (ImGui::InputInt("Block Size", &temp)) {
    blockSize = std::clamp(temp, 0, 255);
  }

  static MemoryEditor mem_edit;
  mem_edit.ReadOnly = false;
  ImGui::Spacing();
  mem_edit.DrawChildWindow("UID Editor", uid.data(), uid.size());
}

void RetReadISO15693Tag::deserializeParameters(const std::span<const uint8_t> data) {
  tagData.assign(data.begin(), data.end());
}

void RetReadISO15693Tag::render() {
  static MemoryEditor mem_edit;
  mem_edit.ReadOnly = true;
  mem_edit.DrawChildWindow("Read Data Preview", tagData.data(), tagData.size());
}

}  // namespace vanch
