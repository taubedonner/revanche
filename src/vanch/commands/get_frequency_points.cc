#include <imgui.h>

#include "command.h"

namespace vanch {

void RetGetFrequencyPoints::deserializeParameters(const std::span<const uint8_t> data) {
  if (data.size() >= 3) {
    region = data[0];
    startFreq = data[1];
    endFreq = data[2];
  }
}

void RetGetFrequencyPoints::render() {
  static const char* regionText[] = {"FCC", "ETSI", "CHN"};
  const char* text = (region < 3) ? regionText[region - 1] : "Unknown";
  ImGui::Text("Region: %s (%u)", text, region);
  ImGui::Text("Start Frequency: %u", startFreq);
  ImGui::Text("End Frequency: %u", endFreq);
}

}  // namespace vanch
