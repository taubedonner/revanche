#include <imgui.h>

#include "command.h"
#include "imgui_memory_editor.h"

namespace vanch {

std::vector<uint8_t> CmdSetAutoReadTagType::serializeParameters() const { return {tagTypeBitmap}; }

void CmdSetAutoReadTagType::render() {
  int temp = tagTypeBitmap;

  ImGui::Text("Bitmap for Tag Types:");
  auto changed = ImGui::CheckboxFlags("ISO18000-6B", &temp, 0x01);  // Bit0
  changed |= ImGui::CheckboxFlags("ISO18000-6C", &temp, 0x02);      // Bit1
  changed |= ImGui::CheckboxFlags("Temperature Tag", &temp, 0x04);  // Bit2

  if (changed) {
    tagTypeBitmap = temp;
  }
}

}  // namespace vanch
