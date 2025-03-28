#include "command.h"

#include <imgui.h>

namespace vanch {

void RetGetVersionNumber::deserializeParameters(const std::span<const uint8_t> data) {
  if (!data.empty()) {
    versionInfo.assign(data.begin(), data.end());
  }
}

void RetGetVersionNumber::render() {
  ImGui::Text("Version Info: %s", versionInfo.c_str());
}

} // namespace vanch
