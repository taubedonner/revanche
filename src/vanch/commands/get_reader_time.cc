#include <imgui.h>

#include "command.h"

namespace vanch {

void RetGetReaderTime::deserializeParameters(const std::span<const uint8_t> data) {
  if (data.size() >= 6) {
    year = data[0];
    month = data[1];
    day = data[2];
    hour = data[3];
    minute = data[4];
    second = data[5];
  }
}

void RetGetReaderTime::render() {
  ImGui::Text("Date: 20%02d-%02d-%02d", year, month, day);
  ImGui::Text("Time: %02d:%02d:%02d", hour, minute, second);
}

}  // namespace vanch
