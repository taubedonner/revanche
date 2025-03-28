#pragma once

#include <asio.hpp>
#include <readerwritercircularbuffer.h>

#include <nlohmann/json.hpp>

#include "message.h"
#include "messageregistry.h"
#include "krog/util/loggable.h"

namespace vanch {

struct BroadcastPacket {
  std::string IP;            // IP адрес ридера
  uint16_t Port;             // Порт ридера
  std::string DeviceType;    // Модель устройства
  std::string ID;            // Идентификатор устройства
  uint8_t RS485;             // Адрес RS485
  uint32_t RS232Baud;        // Скорость RS232
  uint32_t RS485Baud;        // Скорость RS485
  uint8_t ti;                // Внутренний код материнской платы
};

class UdpClient final : kr::Loggable {
public:
  using CommandCallback = std::function<void(const std::shared_ptr<IMessage>&)>;
  using StatusCallback = std::function<void(const std::shared_ptr<IMessage>&)>;
  using BroadcastCallback = std::function<void(const BroadcastPacket&)>;
  using ErrorCallback = std::function<void(std::string_view, uint8_t code)>;

  UdpClient(const std::string& serverIp, const uint16_t serverPort)
    : Loggable("UdpClient"),
      m_socket(m_io, asio::ip::udp::endpoint(asio::ip::udp::v4(), 0)),
      m_broadcastSocket(m_io, asio::ip::udp::endpoint(asio::ip::udp::v4(), 4444)),
      m_serverEndpoint(asio::ip::make_address(serverIp), serverPort), m_isRunning(false) {
    m_broadcastSocket.set_option(asio::socket_base::broadcast(true));
  }

  ~UdpClient() override {
    stop();
  }

  void setServerEndpoint(const std::string& serverIp, uint16_t serverPort) {
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

  bool isRunning() {
    return m_isRunning;
  }

  void start() {
    if (!m_isRunning) {
      logger->info("Starting UDP Client (listening {}, 4444)", m_socket.local_endpoint().port());
      m_io.restart();
      m_isRunning = true;

      listenForResponses();
      listenForBroadcasts();

      m_networkThread = std::thread([this]{m_io.run();});
      m_processThread = std::thread(&UdpClient::processCommands, this);
    }
  }

  void stop() {
    if (m_isRunning) {
      logger->info("Stopping UDP Client");
      m_isRunning = false;
      m_io.stop();
      if (m_networkThread.joinable()) m_networkThread.join();
      if (m_processThread.joinable()) m_processThread.join();
    }
  }

  void sendCommand(const std::shared_ptr<IMessage>& command) {
    if (!m_isRunning) {
      logger->warn("Could not enqueue command. Client is closed!");
      return;
    }

    if (!commandBuffer_.try_enqueue(command)) {
      logger->warn("Failed to enqueue command. Command queue is full");
    }
  }

  void setCommandCallback(const CommandCallback& callback) {
    std::lock_guard lock(m_callbackMutex);
    m_commandCallback = callback;
  }

  void setStatusCallback(const StatusCallback& callback) {
    std::lock_guard lock(m_callbackMutex);
    m_statusCallback = callback;
  }

  void setBroadcastCallback(const BroadcastCallback& callback) {
    std::lock_guard lock(m_callbackMutex);
    m_broadcastCallback = callback;
  }

  void setErrorCallback(const ErrorCallback& callback) {
    std::lock_guard lock(m_callbackMutex);
    m_errorCallback = callback;
  }

private:
  BroadcastPacket parseBroadcastPacket(std::span<const uint8_t> data) {
    using json = nlohmann::json;
    BroadcastPacket packet{};
    std::string jsonString(reinterpret_cast<const char*>(data.data()), data.size());

    try {
      json jsonData = json::parse(jsonString);
      packet.IP = jsonData.at("IP").get<std::string>();
      packet.Port = jsonData.at("Port").get<uint16_t>();
      packet.DeviceType = jsonData.at("DeviceType").get<std::string>();
      packet.ID = jsonData.at("ID").get<std::string>();
      packet.RS485 = jsonData.at("RS485").get<uint8_t>();
      packet.RS232Baud = jsonData.at("RS232Baud").get<uint32_t>();
      packet.RS485Baud = jsonData.at("RS485Baud").get<uint32_t>();
      packet.ti = jsonData.at("ti").get<uint8_t>();
    } catch (const json::exception& e) {
      logger->warn("JSON parsing error: " + std::string(e.what()));
    }
    return packet;
  }

  void listenForResponses() {

    auto handleReceive = [this](const asio::error_code& ec, const std::size_t bytesReceived) mutable {
      if (!ec && bytesReceived > 0) {
        handleResponse({m_responseBuffer.begin(), m_responseBuffer.begin() + bytesReceived}, ec);
      } else if (ec) {
        invokeErrorCallback("Error receiving response: " + ec.message(), 0xFF);
      }

      if (m_isRunning) {
        listenForResponses();
      }
    };

    m_socket.async_receive_from(asio::buffer(m_responseBuffer), m_remoteEndpoint, handleReceive);
  }

  void listenForBroadcasts() {
    auto handleReceive = [this](const asio::error_code& ec, const std::size_t bytesReceived) mutable {
      if (!ec && bytesReceived > 0) {
        handleBroadcast({m_broadcastBuffer.begin(), m_broadcastBuffer.begin() + bytesReceived}, ec);
      } else if (ec) {
        invokeErrorCallback("Error receiving broadcast: " + ec.message(), 0xFF);
      }

      if (m_isRunning) {
        listenForBroadcasts();
      }
    };

    m_broadcastSocket.async_receive_from(asio::buffer(m_broadcastBuffer), m_remoteEndpoint, handleReceive);
  }

  void processCommands() {
    while (m_isRunning) {
      if (std::shared_ptr<IMessage> command; commandBuffer_.wait_dequeue_timed(command, std::chrono::milliseconds(100))) {
        m_socket.async_send_to(asio::buffer(command->serialize()), m_serverEndpoint, [this](const asio::error_code& ec, std::size_t) {
          if (ec) {
            logger->error("Failed to send command: {}", ec.message());
            invokeErrorCallback("Failed to send command: " + ec.message(), 0xFF);
          }
        });
      }
    }
  }

  void handleResponse(const std::vector<uint8_t> data, const asio::error_code& ec) {
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
        return;
      }

      logger->warn("Cannot handle packet with header 0x{:02x} and code 0x{:02x}", static_cast<uint8_t>(message->getType()), message->getCmdCode());
    }
  }

  void handleBroadcast(const std::vector<uint8_t> data, const asio::error_code& ec) {
    if (ec && data.empty()) return;

    const auto message = parseBroadcastPacket(data);
    invokeBroadcastCallback(message);
  }

  void invokeCommandCallback(const std::shared_ptr<IMessage>& response) {
    std::lock_guard lock(m_callbackMutex);
    if (m_commandCallback) {
      m_commandCallback(response);
    }
  }

  void invokeStatusCallback(const std::shared_ptr<IMessage>& status) {
    std::lock_guard lock(m_callbackMutex);
    if (m_statusCallback) {
      m_statusCallback(status);
    }
  }

  void invokeBroadcastCallback(const BroadcastPacket& broadcast) {
    std::lock_guard lock(m_callbackMutex);
    if (m_broadcastCallback) {
      m_broadcastCallback(broadcast);
    }
  }

  void invokeErrorCallback(std::string_view error, const uint8_t code) {
    std::lock_guard lock(m_callbackMutex);
    if (m_errorCallback) {
      m_errorCallback(error, code);
    }
  }

  std::array<uint8_t, 1500> m_responseBuffer{};
  std::array<uint8_t, 1500> m_broadcastBuffer{};

  asio::io_context m_io;
  asio::ip::udp::socket m_socket;
  asio::ip::udp::socket m_broadcastSocket;
  asio::ip::udp::endpoint m_serverEndpoint;
  asio::ip::udp::endpoint m_remoteEndpoint;

  moodycamel::BlockingReaderWriterCircularBuffer<std::shared_ptr<IMessage> > commandBuffer_{10};

  std::thread m_processThread;
  std::thread m_networkThread;

  std::atomic_bool m_isRunning;

  mutable std::mutex m_callbackMutex;
  mutable std::mutex m_pendingCommandsMutex;

  CommandCallback m_commandCallback;
  StatusCallback m_statusCallback;
  BroadcastCallback m_broadcastCallback;
  ErrorCallback m_errorCallback;
};

} // namespace vanch
