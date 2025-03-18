#pragma once

#include <krog/entry.h>

class RevancheApp final : public kr::Layer, protected kr::Loggable {
public:
  RevancheApp() : Layer("RevancheApp"), Loggable("App") {};

private:
  void OnAttach() override;

  void OnDetach() override;

  void OnUiUpdate() override;
};

[[maybe_unused]] inline kr::Application* kr::CreateApp() {
  std::setlocale(LC_CTYPE, "ru_RU.UTF-8");
  std::setlocale(LC_TIME, "ru_RU.UTF-8");
  std::setlocale(LC_COLLATE, "ru_RU.UTF-8");
  const auto app = new kr::Application({"Revanche"});
  app->AttachLayer(std::make_shared<RevancheApp>());
  return app;
}