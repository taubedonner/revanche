#pragma once

#include <krog/entry.h>

#include "backgroundiocontext.h"
#include "vanch/messageregistry.h"
#include "vanch/udpclient.h"

struct AppSettings {
  std::string ip;
  uint16_t port;
};

class RevancheApp final : public kr::Layer, protected kr::Loggable {
public:
  RevancheApp() : Layer("RevancheApp"), Loggable("App"), m_io(), m_client(m_io.getIoContext(), "192.168.1.100", 1969) {
    vanch::MessageRegistry::init();
  }

private:
  void OnAttach() override;

  void OnDetach() override;

  void OnUiUpdate() override;

  void OnPacketReturn(const std::shared_ptr<vanch::IMessage>& msg);

  void OnPacketStatus(const std::shared_ptr<vanch::IMessage>& msg);

  void OnPacketError(std::string_view message, uint8_t code);

  void OnPacketBroadcast(const std::shared_ptr<vanch::IMessage>& msg);

  BackgroundIoContext m_io;
  vanch::UdpClient m_client;

  std::shared_ptr<vanch::IMessage> m_command{};
  std::shared_ptr<vanch::IMessage> m_received{};
  std::shared_ptr<vanch::IMessage> m_status{};

  bool m_show_status{false};
  AppSettings m_settings{};
};

[[maybe_unused]] inline kr::Application* kr::CreateApp() {
  std::setlocale(LC_CTYPE, "ru_RU.UTF-8");
  std::setlocale(LC_TIME, "ru_RU.UTF-8");
  std::setlocale(LC_COLLATE, "ru_RU.UTF-8");
  const auto app = new kr::Application({"Revanche"});
  app->AttachLayer(std::make_shared<RevancheApp>());
  return app;
}