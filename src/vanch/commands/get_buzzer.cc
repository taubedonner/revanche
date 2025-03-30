#include <imgui.h>

#include "command.h"

namespace vanch {

void RetGetBuzzer::deserializeParameters(const std::span<const uint8_t> data) {
  if (!data.empty()) {
    state = data[0];
  }
}

void RetGetBuzzer::render() { ImGui::Text("Buzzer State: %s (%u)", state ? "On" : "Off", state); }

}  // namespace vanch