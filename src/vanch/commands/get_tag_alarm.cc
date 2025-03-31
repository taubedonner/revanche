#include <imgui.h>

#include "command.h"
#include "imgui_memory_editor.h"

namespace vanch {

void RetGetTagAlarm::deserializeParameters(const std::span<const uint8_t> data) {
  if (data.size() >= 3) {
    isEnabled = data[0];
    maskAddress = data[1];
    maskLength = data[2];
    maskData.assign(data.begin() + 3, data.end());
  }
}

void RetGetTagAlarm::render() {
  ImGui::Text("Alarm Enabled: %s", isEnabled ? "Yes" : "No");
  ImGui::Text("Mask Address: %u", maskAddress);
  ImGui::Text("Mask Length: %u", maskLength);

  static MemoryEditor mem_edit;
  mem_edit.ReadOnly = true;
  ImGui::Spacing();
  mem_edit.DrawChildWindow("Mask Data Preview", maskData.data(), maskData.size());
}

}  // namespace vanch
