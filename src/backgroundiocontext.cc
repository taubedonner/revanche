#include "backgroundiocontext.h"

BackgroundIoContext::BackgroundIoContext()
    : m_workGuard(make_work_guard(m_ioContext)), m_thread([this] { m_ioContext.run(); }) {}

BackgroundIoContext::~BackgroundIoContext() { stop(); }

asio::io_context& BackgroundIoContext::getIoContext() { return m_ioContext; }

void BackgroundIoContext::stop() {
  if (m_ioContext.stopped()) return;
  m_ioContext.stop();
  if (m_thread.joinable()) m_thread.join();
}
