#include <fmt/format.h>
#include <imgui.h>

#include "command.h"
#include "imgui_memory_editor.h"

namespace vanch {

std::vector<uint8_t> CmdSetAutoPollingAntenna::serializeParameters() const { return antennas; }

void CmdSetAutoPollingAntenna::render() {
  static bool selectedAntennas[4] = {false};  // Assuming max 4 antennas
  ImGui::Text("Select Antennas for Polling:");
  for (int i = 0; i < 4; ++i) {
    ImGui::Checkbox(fmt::format("Antenna {}", i + 1).c_str(), &selectedAntennas[i]);
  }

  // Update antennas vector based on selection
  antennas.clear();
  for (int i = 0; i < 4; ++i) {
    if (selectedAntennas[i]) {
      antennas.push_back(i + 1);
    }
  }
}

}  // namespace vanch
