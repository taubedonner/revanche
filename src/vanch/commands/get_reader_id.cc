#include <imgui.h>

#include "command.h"

namespace vanch {

void RetGetReaderID::deserializeParameters(const std::span<const uint8_t> data) {
  readerID.assign(data.begin(), data.end());
}

void RetGetReaderID::render() { ImGui::Text("Reader ID: %s", readerID.c_str()); }

}  // namespace vanch
