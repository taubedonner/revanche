#include <imgui.h>

#include "command.h"
#include "imgui_memory_editor.h"

namespace vanch {

std::vector<uint8_t> CmdWrite15693MultipleBlocks::serializeParameters() const {
  std::vector<uint8_t> result = {uidLength};
  result.insert(result.end(), uid.begin(), uid.end());
  result.push_back(startBlock);
  result.push_back(numBlocks);
  result.insert(result.end(), writeData.begin(), writeData.end());
  return result;
}

void CmdWrite15693MultipleBlocks::render() {
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

  if (const auto numBytes = numBlocks * 4; numBytes > writeData.size()) {
    writeData.resize(numBytes, 0);
  } else if (numBytes < writeData.size()) {
    writeData.resize(numBytes);
  }

  static MemoryEditor me1, me2;
  ImGui::Spacing();
  me1.DrawChildWindow("UID Editor", uid.data(), uid.size(), {ImGui::GetContentRegionAvail().x * 0.5f, 0});
  ImGui::SameLine();
  me2.DrawChildWindow("Write Data Editor (n * 4 Bytes)", writeData.data(), writeData.size());

}

}  // namespace vanch
