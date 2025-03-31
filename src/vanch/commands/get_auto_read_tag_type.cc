#include <imgui.h>

#include "command.h"
#include "imgui_memory_editor.h"

namespace vanch {

void RetGetAutoReadTagType::deserializeParameters(const std::span<const uint8_t> data) {
  if (!data.empty()) {
    tagTypeBitmap = data[0];
  }
}

void RetGetAutoReadTagType::render() {
  ImGui::Text("Automatically Read Tag Types:");
  ImGui::Text("ISO18000-6B: %s", (tagTypeBitmap & 0x01) ? "Enabled" : "Disabled");
  ImGui::Text("ISO18000-6C: %s", (tagTypeBitmap & 0x02) ? "Enabled" : "Disabled");
  ImGui::Text("Temperature Tag: %s", (tagTypeBitmap & 0x04) ? "Enabled" : "Disabled");
}

}  // namespace vanch
