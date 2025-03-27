#include "revancheapp.h"

#include "imgui_stdlib.h"

void RevancheApp::OnAttach() {
  Layer::OnAttach();

  m_client.setCommandCallback(KR_BIND_FN(RevancheApp::OnPacketReturn));
  m_client.setStatusCallback(KR_BIND_FN(RevancheApp::OnPacketStatus));
  m_client.setErrorCallback(KR_BIND_FN(RevancheApp::OnPacketError));
}

void RevancheApp::OnDetach() {
  Layer::OnDetach();
}

void RevancheApp::OnUiUpdate() {
  const bool is_connected = m_client.isRunning();

  constexpr auto flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus;
  const auto viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->Pos);
  ImGui::SetNextWindowSize(viewport->Size);
  ImGui::SetNextWindowViewport(viewport->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  if (ImGui::Begin("##FullscreenWindow", nullptr, flags)) {
    ImGui::PopStyleVar(2);
    {
      static std::string ip = m_client.getServerIp();
      static uint16_t port = m_client.getServerPort();

      ImGui::SetNextItemWidth(120.f);
      ImGui::InputText("IP Address", &ip);
      ImGui::SameLine();
      ImGui::SetNextItemWidth(80.f);
      ImGui::InputScalar("Port", ImGuiDataType_U16, &port);
      ImGui::SameLine(0.0f, 16.0f);
      if (is_connected) {
        if (ImGui::Button("Disconnect", {90, 0})) {
          m_client.stop();
        }
      } else {
        if (ImGui::Button("Connect", {90, 0})) {
          m_client.start();
        }
      }
      ImGui::SameLine(ImGui::GetContentRegionMax().x - 220.0f - ImGui::GetStyle().FramePadding.x, 0.0f);
      if (ImGui::DisablingButton("Send Command", !is_connected || !m_command, {120, 0})) {
        m_client.sendCommand(m_command->serialize());
      }
      ImGui::SameLine();
      if (ImGui::Button("Open Status", {100, 0})) {
        m_show_status = true;
      }
    }

    ImGui::Spacing();

    {
      ImGui::BeginChild("left pane", ImVec2(200, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
      for (const auto& [k, v] : m_registry.getCreators()) {
        if (k.first == vanch::MessageType_Command) {
          static std::vector<uint8_t> dummy = {vanch::MessageType_Command, 0x00, 0x00, 0x00, k.second, 0x00};
          auto label = fmt::format("{:02X}H {}", k.second, v.name);
          if (ImGui::Selectable(label.c_str(), (m_command && m_command->m_cmdCode == k.second))) {
            dummy[4] = k.second;
            m_command = v.creator(dummy);
          }
        }
      }
      ImGui::EndChild();
    }
    ImGui::SameLine();
    {
      ImGui::BeginGroup();
      ImGui::BeginChild("input message", ImVec2(0, ImGui::GetContentRegionAvail().y / 2.0f), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeY);
      if (m_command) {
        auto title = fmt::format("Command {:02X}H: {}", m_command->m_cmdCode, m_command->getMessageName());
        ImGui::TextUnformatted(title.c_str());
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        m_command->render();
      }
      ImGui::EndChild();
      ImGui::BeginChild("output message", ImVec2(0, 0), ImGuiChildFlags_Borders);
      if (m_received) {
        m_received->render();
      }
      ImGui::EndChild();
      ImGui::EndGroup();
    }
  }
  ImGui::End();

  if (!m_show_status) return;

  ImGui::SetNextWindowSize({840, 480}, ImGuiCond_Once);
  if (ImGui::Begin("Status Checker", &m_show_status)) {
    if (m_status) {
      m_status->render();
    }
  }
  ImGui::End();
}

void RevancheApp::OnPacketReturn(const std::vector<uint8_t>& data) {
  const auto msg = m_registry.createMessage(data);

  if (msg->m_header == vanch::MessageType_Error) {
    if (const auto res = std::dynamic_pointer_cast<vanch::ErrorResponse>(msg); res) {
      logger->error("Failed to parse packet: ", res->description);
      return;
    }
  }

  m_received = std::move(msg);
}

void RevancheApp::OnPacketStatus(const std::vector<uint8_t>& data) {
  const auto msg = m_registry.createMessage(data);

  if (msg->m_header == vanch::MessageType_Error) {
    if (const auto res = std::dynamic_pointer_cast<vanch::ErrorResponse>(msg); res) {
      logger->error("Failed to parse status packet: ", res->description);
      return;
    }
  }

  m_status = std::move(msg);
}

void RevancheApp::OnPacketError(const std::string& message, const uint8_t code) {
  if (code == 0xFF) {
    logger->error("Received app error: {}", message);
  } else if (const auto it = m_registry.getCreators().find(std::make_pair(vanch::MessageType_Error, code)); it != m_registry.getCreators().end()) {
    logger->error("Received device error {}: {}", code, it->second.name);
  }
}
