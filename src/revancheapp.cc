#include "revancheapp.h"

#include <krog/ui/misc/carbon_icons.h>

#include <nlohmann/json.hpp>

#include <fmt/format.h>
#include <fmt/chrono.h>

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
}

void RevancheApp::OnDetach() {
  auto& config = kr::PersistentConfig::GetRoot();
  config["server-ip"] = m_settings.ip;
  config["server-port"] = m_settings.port;
  kr::PersistentConfig::Save();
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
      ImGui::SameLine(ImGui::GetContentRegionMax().x - 265.0f - ImGui::GetStyle().FramePadding.x, 0.0f);
      static std::string show_status_btn = fmt::format("{}  Show Status", CarbonIcons::Iot::Platform);
      if (ImGui::Button(show_status_btn.c_str(), {120, 0})) {
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
          const auto val = std::chrono::floor<std::chrono::duration<int64_t, std::milli>>(m_command->messageTimestamp.value());
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
          const auto val = std::chrono::floor<std::chrono::duration<int64_t, std::milli>>(m_return->messageTimestamp.value());
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

  if (!m_showStatus) return;

  ImGui::SetNextWindowSize({840, 480}, ImGuiCond_Once);
  if (ImGui::Begin("Status Viewer", &m_showStatus)) {
    if (m_statusAutoRead) {
      m_statusAutoRead->render();

      if (m_statusAutoRead->messageTimestamp.has_value()) {
        ImGui::Spacing();
        ImGui::Separator();
        const auto ts = std::chrono::floor<std::chrono::minutes>(m_statusAutoRead->messageTimestamp.value());
        const auto msd = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - ts);
        ImGui::TextUnformatted(fmt::format("{:%H:%M:%S} ({:%S}s)", std::chrono::current_zone()->to_local(ts),
                                           std::chrono::floor<std::chrono::duration<int64_t, std::deci> >(msd)) .c_str());
      }
    }
  }
  ImGui::End();
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
    case 0x01: m_statusAutoRead = std::dynamic_pointer_cast<vanch::StatusAutoCardReading>(msg); break;
    case 0x03: m_statusHeartbeat = std::dynamic_pointer_cast<vanch::StatusHeartbeat>(msg); break;
    default: logger->warn("Unknown status message with code 0x{:02x}", msg->getCmdCode());
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
