#include <imgui.h>

#include "command.h"

namespace vanch {

std::vector<uint8_t> CmdSetReaderTime::serializeParameters() const { return {year, month, day, hour, minute, second}; }

void CmdSetReaderTime::render() {
  int tempYear = year, tempMonth = month, tempDay = day;
  int tempHour = hour, tempMinute = minute, tempSecond = second;

  ImGui::InputInt("Year (e.g. 23 for 2023)", &tempYear);
  ImGui::InputInt("Month", &tempMonth);
  ImGui::InputInt("Day", &tempDay);
  ImGui::InputInt("Hour", &tempHour);
  ImGui::InputInt("Minute", &tempMinute);
  ImGui::InputInt("Second", &tempSecond);

  year = std::clamp(tempYear, 0, 99);
  month = std::clamp(tempMonth, 1, 12);
  day = std::clamp(tempDay, 1, 31);
  hour = std::clamp(tempHour, 0, 23);
  minute = std::clamp(tempMinute, 0, 59);
  second = std::clamp(tempSecond, 0, 59);
}

}  // namespace vanch
