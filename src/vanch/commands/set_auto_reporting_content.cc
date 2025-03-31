#include <imgui.h>

#include "command.h"
#include "imgui_memory_editor.h"

namespace vanch {

std::vector<uint8_t> CmdSetAutoReportingContent::serializeParameters() const {
  std::vector<uint8_t> result = {customFieldNumber};
  result.insert(result.end(), customContent.begin(), customContent.end());
  return result;
}

void CmdSetAutoReportingContent::render() {
  int temp = customFieldNumber;
  ImGui::InputInt("Custom Field Number", &temp);
  customFieldNumber = std::clamp<uint8_t>(temp, 0, 5);

  static MemoryEditor mem_edit;
  mem_edit.ReadOnly = false;
  ImGui::Spacing();
  mem_edit.DrawChildWindow("Custom Content Editor", customContent.data(), customContent.size());
}

}  // namespace vanch
