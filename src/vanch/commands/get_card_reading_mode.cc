#include <imgui.h>

#include "command.h"

namespace vanch {

void RetGetCardReadingMode::deserializeParameters(const std::span<const uint8_t> data) {
  if (!data.empty()) {
    mode = data[0];
  }
}

void RetGetCardReadingMode::render() {
  static const char* modeText[] = {"Command Mode", "Automatic Mode (Continuous)", "Automatic Mode (Switch Trigger)",
                                   "Automatic Mode (Two-way Trigger)"};
  const char* text = (mode < 4) ? modeText[mode] : "Unknown";
  ImGui::Text("Card Reading Mode: %s (%u)", text, mode);
}

}  // namespace vanch