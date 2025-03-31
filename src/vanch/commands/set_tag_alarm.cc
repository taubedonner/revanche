#include <imgui.h>

#include "command.h"
#include "imgui_memory_editor.h"

namespace vanch {

std::vector<uint8_t> CmdSetTagAlarm::serializeParameters() const {
  std::vector<uint8_t> result;
  result.push_back(isEnabled);
  result.push_back(maskAddress);
  result.push_back(maskLength);
  result.insert(result.end(), maskData.begin(), maskData.end());
  return result;
}

void CmdSetTagAlarm::render() {
  static const char* enableItems[] = {"Disabled (0x00)", "Enabled (0x01)"};
  int tempEnabled = isEnabled;
  if (ImGui::Combo("Enable Alarm", &tempEnabled, enableItems, IM_ARRAYSIZE(enableItems))) {
    isEnabled = tempEnabled;
  }

  int tempAddress = maskAddress;
  if (ImGui::InputInt("Mask Address", &tempAddress)) {
    maskAddress = std::clamp(tempAddress, 0, 63);
  }

  int tempLength = maskLength;
  if (ImGui::InputInt("Mask Length", &tempLength)) {
    maskLength = std::clamp(tempLength, 0, 64);
  }

  if (maskLength > maskData.size()) {
    maskData.resize(maskLength, 0);
  } else if (maskLength < maskData.size()) {
    maskData.resize(maskLength);
  }

  static MemoryEditor mem_edit;
  ImGui::Spacing();
  mem_edit.DrawChildWindow("Mask Data Editor", maskData.data(), maskData.size());
}

}  // namespace vanch
