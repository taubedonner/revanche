#include <imgui.h>

#include "command.h"

namespace vanch {

void RetGetReaderName::deserializeParameters(const std::span<const uint8_t> data) {
  readerName.assign(data.begin(), data.end());
}

void RetGetReaderName::render() { ImGui::Text("Reader Name: %s", readerName.c_str()); }

}  // namespace vanch
