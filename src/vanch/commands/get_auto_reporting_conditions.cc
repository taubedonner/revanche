#include <imgui.h>

#include "command.h"
#include "imgui_memory_editor.h"

namespace vanch {

void RetGetAutoReportingConditions::deserializeParameters(const std::span<const uint8_t> data) {
  if (data.size() >= 2) {
    reportingMode = data[0];
    reportingInterval = data[1];
  }
}

void RetGetAutoReportingConditions::render() {
  ImGui::Text("Reporting Mode: %u", reportingMode);
  ImGui::Text("Reporting Interval: %u seconds", reportingInterval);
}

}  // namespace vanch
