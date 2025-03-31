#include "udpclient.h"

#include "messageregistry.h"

namespace vanch {

UdpClient::UdpClient(asio::io_context& io, const std::string& serverIp, const uint16_t serverPort)
    : Loggable("UdpClient"),
      m_io(io),
      m_socket(io),
      m_broadcastSocket(io),
      m_serverEndpoint(asio::ip::make_address(serverIp), serverPort),
      m_isRunning(false) {}

UdpClient::~UdpClient() { stop(); }

void UdpClient::setServerEndpoint(const std::string& serverIp, uint16_t serverPort) {
  asio::error_code ec;

  const auto ip = asio::ip::make_address(serverIp, ec);

  if (ec) {
    logger->error("Invalid IP address: {}", serverIp);
    return;
  }

  if (serverPort < 1 || serverPort > 65535) {
    logger->error("Invalid port number: {}", serverPort);
    return;
  }

  m_serverEndpoint.address(ip);
  m_serverEndpoint.port(serverPort);

  if (m_isRunning) {
    stop();
    start();
  }
}
bool UdpClient::isRunning() { return m_isRunning; }

void UdpClient::start() {
  if (m_isRunning) return;

  asio::error_code ec, ec1, ec2;

  logger->info("Starting UDP Client");

  m_socket.open(asio::ip::udp::v4(), ec);
  m_socket.bind(asio::ip::udp::endpoint(asio::ip::udp::v4(), 0), ec1);

  if (ec) {
    logger->error("Failed to open UDP Client socket: {}", ec.message());
    return;
  }

  if (ec1) {
    logger->error("Failed to bind random port for UDP Client socket: {}", ec.message());
    m_socket.close();
    return;
  }

  logger->info("UDP Client port: {}", m_socket.local_endpoint().port());

  bool broadcastOk = true;
  m_broadcastSocket.open(asio::ip::udp::v4(), ec);
  m_broadcastSocket.bind(asio::ip::udp::endpoint(asio::ip::udp::v4(), k_defaultBroadcastPort), ec1);
  m_broadcastSocket.set_option(asio::socket_base::broadcast(true), ec2);

  if (ec) {
    broadcastOk = false;
    logger->error("Failed to open UDP Broadcast Server socket: {}", ec.message());
  }

  if (ec1) {
    broadcastOk = false;
    logger->error("Failed to bind port {} for UDP Broadcast Server socket: {}", k_defaultBroadcastPort, ec.message());
  }

  if (ec2) {
    broadcastOk = false;
    logger->error("Failed to set flags for UDP Broadcast Server socket: {}", ec.message());
  }

  if (broadcastOk) {
    logger->info("UDP Broadcast Server port: {}", m_broadcastSocket.local_endpoint().port());
  } else {
    logger->warn("UDP Broadcast Server socket is not available until the next restart");
  }

  m_isRunning = true;

  if (broadcastOk) co_spawn(m_io, broadcastListenLoop(), asio::detached);
  co_spawn(m_io, listenLoop(), asio::detached);
  co_spawn(m_io, sendLoop(), asio::detached);
}

void UdpClient::stop() {
  if (!m_isRunning) return;

  logger->info("Stopping UDP Client");

  m_isRunning = false;
  m_broadcastSocket.close();
  m_socket.close();
}

void UdpClient::restart() {
  stop();
  start();
}

void UdpClient::sendCommand(const std::shared_ptr<IMessage>& command) {
  if (!m_isRunning) {
    logger->warn("Could not enqueue command. Client is closed!");
    return;
  }

  if (!m_commandQueue.try_enqueue(command)) {
    logger->warn("Failed to enqueue command. Command queue is full");
  }
}

void UdpClient::setCommandCallback(const CommandCallback& callback) {
  std::lock_guard lock(m_callbackMutex);
  m_commandCallback = callback;
}

void UdpClient::setStatusCallback(const StatusCallback& callback) {
  std::lock_guard lock(m_callbackMutex);
  m_statusCallback = callback;
}

void UdpClient::setBroadcastCallback(const BroadcastCallback& callback) {
  std::lock_guard lock(m_callbackMutex);
  m_broadcastCallback = callback;
}

void UdpClient::setErrorCallback(const ErrorCallback& callback) {
  std::lock_guard lock(m_callbackMutex);
  m_errorCallback = callback;
}

asio::awaitable<bool> UdpClient::tryDequeueCoWait(std::shared_ptr<IMessage>& message) {
  if (m_commandQueue.try_dequeue(message)) co_return true;

  auto timer = steady_timer{co_await asio::this_coro::executor, std::chrono::milliseconds(100)};

  if (auto [ec] = co_await timer.async_wait(); ec && ec != asio::error::operation_aborted) {
    logger->error("Failed to dequeue message from command queue");
  }

  co_return false;
}

asio::awaitable<void> UdpClient::listenLoop() {
  std::array<uint8_t, 1500> buffer{};
  asio::ip::udp::endpoint remoteEndpoint;

  while (m_isRunning) {
    auto [ec, bytesReceived] = co_await m_socket.async_receive_from(asio::buffer(buffer), remoteEndpoint);

    if (ec) {
      if (ec == asio::error::operation_aborted && !m_isRunning) break;
      if (ec == asio::error::connection_refused || ec == asio::error::connection_reset) {
        logger->warn("Destination peer is unreachable ({}:{})", remoteEndpoint.address().to_string(), remoteEndpoint.port());
        continue;
      }
      invokeErrorCallback("Error receiving response: " + ec.message());
      continue;
    }

    if (remoteEndpoint != m_serverEndpoint) continue;

    handleResponse({buffer.begin(), buffer.begin() + bytesReceived}, ec);
  }
}

asio::awaitable<void> UdpClient::broadcastListenLoop() {
  std::array<uint8_t, 1500> buffer{};
  asio::ip::udp::endpoint remoteEndpoint;

  while (m_isRunning) {
    auto [ec, bytesReceived] = co_await m_broadcastSocket.async_receive_from(asio::buffer(buffer), remoteEndpoint);

    if (ec) {
      if (ec == asio::error::operation_aborted && !m_isRunning) break;
      invokeErrorCallback("Error receiving broadcast: " + ec.message());
      continue;
    }

    handleBroadcast({buffer.begin(), buffer.begin() + bytesReceived}, ec);
  }
}

asio::awaitable<void> UdpClient::sendLoop() {
  while (m_isRunning) {
    std::shared_ptr<IMessage> command;

    if (!co_await tryDequeueCoWait(command)) continue;

    auto data = command->serialize();
    auto buffer = asio::buffer(data.data(), data.size());

    if (auto [ec, _] = co_await m_socket.async_send_to(buffer, m_serverEndpoint); ec) {
      invokeErrorCallback("Failed to send command: " + ec.message());
    }
  }
}

void UdpClient::handleResponse(const std::vector<uint8_t>&& data, const asio::error_code& ec) {
  if (ec && data.empty()) return;

  if (const auto message = MessageRegistry::createFromData(data)) {
    if (message->getType() == MessageType_Return) {
      invokeCommandCallback(message);
      return;
    }

    if (message->getType() == MessageType_Status) {
      invokeStatusCallback(message);
      return;
    }

    if (message->getType() == MessageType_Error) {
      const auto code = message->getCmdCode();
      invokeErrorCallback(MessageRegistry::getErrorMessage(code), code);
      invokeCommandCallback(message);
      return;
    }

    logger->warn("Cannot handle packet with header 0x{:02x} and code 0x{:02x}",
                 static_cast<uint8_t>(message->getType()), message->getCmdCode());
  }
}

void UdpClient::handleBroadcast(const std::vector<uint8_t>&& data, const asio::error_code& ec) {
  if (ec && data.empty()) return;

  if (const auto message = MessageRegistry::createFromData(data)) {
    if (message->getType() == MessageType_Status) {
      invokeBroadcastCallback(message);
    }
  }
}

void UdpClient::invokeCommandCallback(const std::shared_ptr<IMessage>& response) {
  std::lock_guard lock(m_callbackMutex);
  if (m_commandCallback) {
    m_commandCallback(response);
  }
}

void UdpClient::invokeStatusCallback(const std::shared_ptr<IMessage>& status) {
  std::lock_guard lock(m_callbackMutex);
  if (m_statusCallback) {
    m_statusCallback(status);
  }
}

void UdpClient::invokeBroadcastCallback(const std::shared_ptr<IMessage>& broadcast) {
  std::lock_guard lock(m_callbackMutex);
  if (m_broadcastCallback) {
    m_broadcastCallback(broadcast);
  }
}

void UdpClient::invokeErrorCallback(const std::string_view error, const uint8_t code) {
  std::lock_guard lock(m_callbackMutex);
  if (m_errorCallback) {
    m_errorCallback(error, code);
  }
}

}  // namespace vanch
