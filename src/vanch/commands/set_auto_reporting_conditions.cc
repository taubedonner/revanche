#include <imgui.h>

#include "command.h"
#include "imgui_memory_editor.h"

namespace vanch {

std::vector<uint8_t> CmdSetAutoReportingConditions::serializeParameters() const {
  return {reportingMode, reportingInterval};
}

void CmdSetAutoReportingConditions::render() {
  static const char* reportingModeItems[] = {
      "Report when inventory is completed (0x00)", "Scheduled reporting (all tags) (0x01)",
      "Scheduled reporting (single tag) (0x02)", "Passive reporting (0x03)", "Real-time reporting (0x04)"};

  int temp = reportingMode;
  if (ImGui::Combo("Reporting Mode", &temp, reportingModeItems, IM_ARRAYSIZE(reportingModeItems))) {
    reportingMode = temp;
  }

  temp = reportingInterval;
  if (ImGui::InputInt("Reporting Interval (seconds)", &temp)) {
    reportingInterval = std::clamp<uint8_t>(temp, 0, 255);
  }
}

}  // namespace vanch
