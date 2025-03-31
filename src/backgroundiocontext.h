#pragma once

#include <../cmake-build-debug-visual-studio/_deps/asio-src/asio/include/asio.hpp>

class BackgroundIoContext {
 public:
  BackgroundIoContext();

  ~BackgroundIoContext();

  asio::io_context& getIoContext();

  void stop();

 private:
  asio::io_context m_ioContext;
  asio::executor_work_guard<asio::io_context::executor_type> m_workGuard;
  std::thread m_thread;
};
