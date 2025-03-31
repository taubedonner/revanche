#include <imgui.h>

#include "command.h"

namespace vanch {

std::vector<uint8_t> CmdSetFrequencyPoints::serializeParameters() const { return {region, startFreq, endFreq}; }

void CmdSetFrequencyPoints::render() {
  static const char* regions[] = {"FCC (0x01)", "ETSI (0x02)", "CHN (0x03)"};
  int tempRegion = region - 1;
  if (ImGui::Combo("Region", &tempRegion, regions, IM_ARRAYSIZE(regions))) {
    region = tempRegion + 1;
  }

  ImGui::InputScalar("Start Frequency", ImGuiDataType_U8, &startFreq);
  ImGui::InputScalar("End Frequency", ImGuiDataType_U8, &endFreq);
}

}  // namespace vanch
