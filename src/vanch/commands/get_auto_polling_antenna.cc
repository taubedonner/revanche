#include <imgui.h>

#include "command.h"
#include "imgui_memory_editor.h"

namespace vanch {

void RetGetAutoPollingAntenna::deserializeParameters(const std::span<const uint8_t> data) {
  antennas.assign(data.begin(), data.end());
}

void RetGetAutoPollingAntenna::render() {
  ImGui::Text("Automatic Polling Antennas:");
  for (auto antenna : antennas) {
    ImGui::Text("Antenna %u", antenna);
  }
}

}  // namespace vanch
