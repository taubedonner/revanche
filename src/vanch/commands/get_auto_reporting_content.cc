#include <imgui.h>

#include "command.h"
#include "imgui_memory_editor.h"

namespace vanch {

std::vector<uint8_t> CmdGetAutoReportingContent::serializeParameters() const { return {customFieldNumber}; }

void CmdGetAutoReportingContent::render() {
  int temp = customFieldNumber;
  if (ImGui::InputInt("Custom Field Number", &temp)) {
    customFieldNumber = std::clamp<uint8_t>(temp, 0, 5);
  }
}

void RetGetAutoReportingContent::deserializeParameters(const std::span<const uint8_t> data) {
  if (data.size() >= 24) {
    for (auto i = 0; i < 24; i++) {
      customContent[i] = data[i];
    }
  }
}

void RetGetAutoReportingContent::render() {
  static MemoryEditor mem_edit;
  mem_edit.ReadOnly = true;
  mem_edit.DrawChildWindow("Custom Content Preview", customContent.data(), customContent.size());
}

}  // namespace vanch
