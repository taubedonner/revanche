#pragma once

#include <readerwritercircularbuffer.h>

#include <asio.hpp>
#include <asio/awaitable.hpp>
#include <nlohmann/json.hpp>

#include "krog/util/loggable.h"
#include "message.h"

namespace vanch {

struct BroadcastPacket {
  std::string IP;          // IP адрес ридера
  uint16_t Port;           // Порт ридера
  std::string DeviceType;  // Модель устройства
  std::string ID;          // Идентификатор устройства
  uint8_t RS485;           // Адрес RS485
  uint32_t RS232Baud;      // Скорость RS232
  uint32_t RS485Baud;      // Скорость RS485
  uint8_t ti;              // Внутренний код материнской платы
  std::chrono::time_point<std::chrono::system_clock> ts;
};

class UdpClient final : kr::Loggable {
 public:
  using CommandCallback = std::function<void(const std::shared_ptr<IMessage>&)>;
  using StatusCallback = std::function<void(const std::shared_ptr<IMessage>&)>;
  using BroadcastCallback = std::function<void(const BroadcastPacket&)>;
  using ErrorCallback = std::function<void(std::string_view, uint8_t code)>;

  UdpClient(asio::io_context& io, const std::string& serverIp, uint16_t serverPort);

  ~UdpClient() override;

  void setServerEndpoint(const std::string& serverIp, uint16_t serverPort);

  bool isRunning();

  void start();

  void stop();

  void sendCommand(const std::shared_ptr<IMessage>& command);

  void setCommandCallback(const CommandCallback& callback);

  void setStatusCallback(const StatusCallback& callback);

  void setBroadcastCallback(const BroadcastCallback& callback);

  void setErrorCallback(const ErrorCallback& callback);

 private:
  BroadcastPacket parseBroadcastPacket(std::span<const uint8_t> data);

  asio::awaitable<bool> tryDequeueCoWait(std::shared_ptr<IMessage>& message);

  asio::awaitable<void> listenLoop();

  asio::awaitable<void> broadcastListenLoop();

  asio::awaitable<void> sendLoop();

  void handleResponse(const std::vector<uint8_t>&& data, const asio::error_code& ec);

  void handleBroadcast(const std::vector<uint8_t>&& data, const asio::error_code& ec);

  void invokeCommandCallback(const std::shared_ptr<IMessage>& response);

  void invokeStatusCallback(const std::shared_ptr<IMessage>& status);

  void invokeBroadcastCallback(const BroadcastPacket& broadcast);

  void invokeErrorCallback(std::string_view error, uint8_t code = 0xFF);

  asio::io_context& m_io;
  asio::ip::udp::socket m_socket;
  asio::ip::udp::socket m_broadcastSocket;
  asio::ip::udp::endpoint m_serverEndpoint;

  moodycamel::BlockingReaderWriterCircularBuffer<std::shared_ptr<IMessage>> m_commandQueue{10};

  std::atomic_bool m_isRunning;

  mutable std::mutex m_callbackMutex;

  CommandCallback m_commandCallback;
  StatusCallback m_statusCallback;
  BroadcastCallback m_broadcastCallback;
  ErrorCallback m_errorCallback;
};

}  // namespace vanch
