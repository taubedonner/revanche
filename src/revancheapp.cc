#include "revancheapp.h"

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <krog/ui/misc/carbon_icons.h>

#include <nlohmann/json.hpp>

#include "imgui_stdlib.h"
#include "krog/util/persistentconfig.h"

void RevancheApp::OnAttach() {
  auto& config = kr::PersistentConfig::GetRoot();
  m_settings.ip = config["server-ip"].as<std::string>("8.8.8.8");
  m_settings.port = config["server-port"].as<int>(1969);

  m_client.setCommandCallback(KR_BIND_FN(RevancheApp::OnPacketReturn));
  m_client.setStatusCallback(KR_BIND_FN(RevancheApp::OnPacketStatus));
  m_client.setErrorCallback(KR_BIND_FN(RevancheApp::OnPacketError));
  m_client.setBroadcastCallback(KR_BIND_FN(RevancheApp::OnPacketBroadcast));

  m_client.setServerEndpoint(m_settings.ip, m_settings.port);

  m_client.startBroadcastListening();
}

void RevancheApp::OnDetach() {
  auto& config = kr::PersistentConfig::GetRoot();
  config["server-ip"] = m_settings.ip;
  config["server-port"] = m_settings.port;
  kr::PersistentConfig::Save();
}

static void renderCell(const std::string& text) {
  ImGui::TextUnformatted(text.c_str());
  if (ImGui::BeginPopupContextItem(fmt::format("{}.{}", ImGui::GetColumnIndex(), text).c_str())) {
    if (ImGui::MenuItem("Copy")) {
      ImGui::SetClipboardText(text.c_str());
    }
    ImGui::EndPopup();
  }
}

void RevancheApp::OnUiUpdate() {
  const bool is_connected = m_client.isRunning();

  constexpr auto flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking |
                         ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoBringToFrontOnFocus;
  const auto viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->Pos);
  ImGui::SetNextWindowSize(viewport->Size);
  ImGui::SetNextWindowViewport(viewport->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  if (ImGui::Begin("##FullscreenWindow", nullptr, flags)) {
    namespace Col = ImGui::Spectrum::Colors;
    const auto& sp = ImGui::Spectrum::GetProps();
    ImGui::PopStyleVar(2);
    {
      if (is_connected) {
        static std::string btn_run = fmt::format("{}  Stop Client", CarbonIcons::Stop::Filled::Alt);
        if (ImGui::ColoredButton(btn_run.c_str(), sp.Color(Col::RED1000, 0.15), sp.Color(Col::RED900), {110, 0})) {
          m_client.stop();
        }
      } else {
        static std::string btn_run = fmt::format("{}  Run Client", CarbonIcons::Play::Filled::Alt);
        if (ImGui::ColoredButton(btn_run.c_str(), sp.Color(Col::BLUE1000, 0.15), sp.Color(Col::BLUE900), {110, 0})) {
          m_client.start();
        }
      }
      ImGui::SameLine(0, 8);
      ImGui::BeginGroup();
      ImGui::SetNextItemWidth(120.f);
      if (ImGui::InputText("IP Address", &m_settings.ip, ImGuiInputTextFlags_EnterReturnsTrue)) {
        m_client.setServerEndpoint(m_settings.ip, m_settings.port);
      }
      ImGui::SameLine(0, 8);
      ImGui::SetNextItemWidth(80.f);
      if (ImGui::InputScalar("Port", ImGuiDataType_U16, &m_settings.port, nullptr, nullptr, nullptr,
                             ImGuiInputTextFlags_EnterReturnsTrue)) {
        m_client.setServerEndpoint(m_settings.ip, m_settings.port);
      }
      ImGui::EndGroup();
      ImGui::SetItemTooltip("Press <Enter> to apply");
      ImGui::SameLine(ImGui::GetContentRegionMax().x - 401.0f - ImGui::GetStyle().FramePadding.x, 0.0f);
      std::string showDevicesBtn = fmt::format("{} {:>3}", CarbonIcons::Query, m_statusDevices.size());
      if (ImGui::ColoredButton(showDevicesBtn.c_str(), sp.Color(Col::GREEN1000, 0.15), sp.Color(Col::GREEN900),
                               {60, 0})) {
        m_showDevList = true;
      }
      ImGui::SetItemTooltip("Found VUP-compatible devices");
      ImGui::SameLine();
      std::string hbTime = "\xE2\x88\x9E";
      if (m_statusHeartbeat && m_statusHeartbeat->messageTimestamp.has_value()) {
        auto val = m_statusHeartbeat->messageTimestamp.value();
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::seconds>(now - val).count();
        if (time < 100) hbTime = fmt::format("{}s", time);
      }
      std::string heartbeatBtn = fmt::format("{} {:>3}", CarbonIcons::Activity, hbTime);
      if (ImGui::ColoredButton(heartbeatBtn.c_str(), sp.Color(Col::YELLOW1000, 0.15), sp.Color(Col::YELLOW900),
                               {60, 0})) {
        m_showStatus = true;
      }
      ImGui::SetItemTooltip("Last received heartbeat packet");
      ImGui::SameLine();
      static std::string showStatusBtn = fmt::format("{}  Auto Read", CarbonIcons::Iot::Platform);
      if (ImGui::Button(showStatusBtn.c_str(), {120, 0})) {
        m_showStatus = true;
      }
      ImGui::SameLine();
      static std::string send_btn = fmt::format("{}  Send Command", CarbonIcons::Mail::All);
      if (ImGui::DisablingButton(send_btn.c_str(), !is_connected || !m_command, {145, 0})) {
        const auto now = std::chrono::system_clock::now();
        m_command->messageTimestamp = {now};
        m_client.sendCommand(m_command);
      }
    }

    ImGui::Spacing();

    {
      auto cmds = vanch::MessageRegistry::getCommandMetadata();
      ImGui::BeginChild("left pane", ImVec2(200, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX,
                        ImGuiWindowFlags_MenuBar);
      if (ImGui::BeginMenuBar()) {
        ImGui::Text("Command List (%llu)", cmds.size());
        ImGui::EndMenuBar();
      }
      for (const auto& [k, v] : cmds) {
        if (auto label = fmt::format("{:02X}H {}", k, v);
            ImGui::Selectable(label.c_str(), (m_command && m_command->getCmdCode() == k))) {
          m_command = vanch::MessageRegistry::create(k, vanch::MessageType_Command);
        }
      }
      ImGui::EndChild();
    }
    ImGui::SameLine();
    {
      ImGui::BeginGroup();
      ImGui::BeginChild("input message", ImVec2(0, ImGui::GetContentRegionAvail().y / 2.0f),
                        ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeY, ImGuiWindowFlags_MenuBar);
      if (ImGui::BeginMenuBar()) {
        ImGui::TextUnformatted("Command Editor");
        if (m_command && m_command->messageTimestamp.has_value()) {
          const auto val =
              std::chrono::floor<std::chrono::duration<int64_t, std::milli>>(m_command->messageTimestamp.value());
          auto ts = fmt::format("Sent at: {:%H:%M:%S}", std::chrono::current_zone()->to_local(val));
          ImGui::SameLine(ImGui::GetContentRegionMax().x - ImGui::CalcTextSize(ts.c_str()).x - 6.0f);
          ImGui::TextDisabled("%s", ts.c_str());
        }
        ImGui::EndMenuBar();
      }
      if (m_command) {
        auto title = fmt::format("{:02X}H: {}", m_command->getCmdCode(), m_command->getMessageName());
        ImGui::TextUnformatted(title.c_str());
        ImGui::Separator();
        ImGui::Spacing();
        m_command->render();
      } else {
        const auto lab = "Command is not selected";
        const auto txtSize = ImGui::CalcTextSize(lab);
        const auto content = ImGui::GetContentRegionMax();
        ImGui::SetCursorPos({(content.x - txtSize.x) / 2.0f, (content.y + txtSize.y) / 2.0f});
        ImGui::TextDisabled(lab);
      }
      ImGui::EndChild();
      ImGui::BeginChild("output message", ImVec2(0, 0), ImGuiChildFlags_Borders, ImGuiWindowFlags_MenuBar);
      if (ImGui::BeginMenuBar()) {
        ImGui::TextUnformatted("Response View");
        if (m_return && m_return->messageTimestamp.has_value()) {
          const auto val =
              std::chrono::floor<std::chrono::duration<int64_t, std::milli>>(m_return->messageTimestamp.value());
          auto ts = fmt::format("Received at: {:%H:%M:%S}", std::chrono::current_zone()->to_local(val));
          ImGui::SameLine(ImGui::GetContentRegionMax().x - ImGui::CalcTextSize(ts.c_str()).x - 6.0f);
          ImGui::TextDisabled("%s", ts.c_str());
        }
        ImGui::EndMenuBar();
      }
      if (m_return) {
        m_return->render();
      } else {
        const auto lab = "Nothing to show";
        const auto txtSize = ImGui::CalcTextSize(lab);
        const auto content = ImGui::GetContentRegionMax();
        ImGui::SetCursorPos({(content.x - txtSize.x) / 2.0f, (content.y + txtSize.y) / 2.0f});
        ImGui::TextDisabled(lab);
      }
      ImGui::EndChild();
      ImGui::EndGroup();
    }
  }
  ImGui::End();

  if (m_showStatus) {
    ImGui::SetNextWindowSize({840, 480}, ImGuiCond_Once);
    if (ImGui::Begin("Status Viewer", &m_showStatus)) {
      if (m_statusAutoRead) {
        m_statusAutoRead->render();
      }
    }
    ImGui::End();
  }

  if (m_showDevList) {
    ImGui::SetNextWindowSize({840, 360}, ImGuiCond_Once);
    if (ImGui::Begin("Device Finder", &m_showDevList)) {
      if (m_statusDevices.empty()) {
        const auto lab = "No reported devices";
        const auto txtSize = ImGui::CalcTextSize(lab);
        const auto content = ImGui::GetContentRegionMax();
        ImGui::SetCursorPos({(content.x - txtSize.x) / 2.0f, (content.y + txtSize.y) / 2.0f});
        ImGui::TextDisabled(lab);
      } else if (ImGui::BeginTable("DeviceTable", 9, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable, {0, -1})) {
        ImGui::TableSetupColumn("IP");
        ImGui::TableSetupColumn("Port");
        ImGui::TableSetupColumn("Dev Type");
        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("RS485");
        ImGui::TableSetupColumn("RS232 Baud");
        ImGui::TableSetupColumn("RS485 Baud");
        ImGui::TableSetupColumn("Reported time");
        ImGui::TableSetupColumn("Int Model");
        ImGui::TableHeadersRow();

        for (const auto& packet : m_statusDevices | std::views::values) {
          ImGui::TableNextRow();

          std::string elapsed = "N/A";
          if (packet->messageTimestamp.has_value()) {
            const auto time = std::chrono::floor<std::chrono::duration<int64_t>>(packet->messageTimestamp.value());
            elapsed = fmt::format("{:%H:%M:%S}", std::chrono::current_zone()->to_local(time));
          }

          ImGui::TableSetColumnIndex(0);
          renderCell(packet->ipAddress);
          ImGui::TableSetColumnIndex(1);
          renderCell(std::to_string(packet->port));
          ImGui::TableSetColumnIndex(2);
          renderCell(packet->deviceType);
          ImGui::TableSetColumnIndex(3);
          renderCell(packet->deviceId);
          ImGui::TableSetColumnIndex(4);
          renderCell(std::to_string(packet->rs485Address));
          ImGui::TableSetColumnIndex(5);
          renderCell(std::to_string(packet->rs232BaudRate));
          ImGui::TableSetColumnIndex(6);
          renderCell(std::to_string(packet->rs485BaudRate));
          ImGui::TableSetColumnIndex(7);
          renderCell(elapsed);
          ImGui::TableSetColumnIndex(8);
          renderCell(std::to_string(packet->internalModel));
        }
        ImGui::EndTable();
      }
      ImGui::End();
    }
  }
}

void RevancheApp::OnPacketReturn(const std::shared_ptr<vanch::IMessage>& msg) {
  if (!msg) return;

  msg->messageTimestamp = {std::chrono::system_clock::now()};

  m_return = msg;
}

void RevancheApp::OnPacketStatus(const std::shared_ptr<vanch::IMessage>& msg) {
  if (!msg) return;

  msg->messageTimestamp = {std::chrono::system_clock::now()};

  switch (msg->getCmdCode()) {
    case 0x01:
      m_statusAutoRead = std::dynamic_pointer_cast<vanch::StatusAutoCardReading>(msg);
      break;
    case 0x03:
      m_statusHeartbeat = std::dynamic_pointer_cast<vanch::StatusHeartbeat>(msg);
      break;
    default:
      logger->warn("Unknown status message with code 0x{:02x}", msg->getCmdCode());
  }
}

void RevancheApp::OnPacketError(std::string_view message, const uint8_t code) {
  if (code == 0xFF) {
    logger->error("Received network error: {}", message);
  } else {
    logger->error("Received device error {}: {}", code, message);
  }

  m_return = nullptr;
}

void RevancheApp::OnPacketBroadcast(const std::shared_ptr<vanch::IMessage>& msg) {
  if (!msg || msg->getCmdCode() != 0x02) return;

  msg->messageTimestamp = {std::chrono::system_clock::now()};

  const auto sd = std::dynamic_pointer_cast<vanch::StatusUdpBroadcast>(msg);

  m_statusDevices[sd->deviceId] = sd;
}
