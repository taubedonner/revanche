#pragma once

#include <asio.hpp>
#include <readerwritercircularbuffer.h>

#include "message.h"
#include "krog/util/loggable.h"

namespace vanch {
class UdpClient final : kr::Loggable {
public:
  using CommandCallback = std::function<void(const std::vector<uint8_t>&)>;
  using StatusCallback = std::function<void(const std::vector<uint8_t>&)>;
  using ErrorCallback = std::function<void(const std::string&, uint8_t code)>;

  UdpClient(const std::string& serverIp, const uint16_t serverPort)
    : Loggable("UdpClient"), socket_(ioContext_, asio::ip::udp::endpoint(asio::ip::udp::v4(), 0)),
      serverEndpoint_(asio::ip::make_address(serverIp), serverPort), isRunning_(false),
      serverIp_(serverIp), serverPort_(serverPort) {

    socket_.set_option(asio::detail::socket_option::integer<SOL_SOCKET, SO_RCVTIMEO>{200});
    socket_.set_option(asio::detail::socket_option::integer<SOL_SOCKET, SO_SNDTIMEO>{200});
  }

  ~UdpClient() override {
    stop();
  }

  bool isRunning() const {
    return isRunning_;
  }

  // Геттеры и сеттеры для параметров подключения
  void setServerIp(const std::string& serverIp) {
    std::lock_guard lock(connectionMutex_);
    serverIp_ = serverIp;
    serverEndpoint_ = asio::ip::udp::endpoint(asio::ip::make_address(serverIp_), serverPort_);
    logger->info("New destination: {}:{}", serverEndpoint_.address().to_string().c_str(), serverEndpoint_.port());
  }

  std::string getServerIp() const {
    std::lock_guard lock(connectionMutex_);
    return serverIp_;
  }

  void setServerPort(uint16_t serverPort) {
    std::lock_guard lock(connectionMutex_);
    serverPort_ = serverPort;
    serverEndpoint_ = asio::ip::udp::endpoint(asio::ip::make_address(serverIp_), serverPort_);
    logger->info("New destination: {}:{}", serverEndpoint_.address().to_string().c_str(), serverEndpoint_.port());
  }

  uint16_t getServerPort() const {
    std::lock_guard lock(connectionMutex_);
    return serverPort_;
  }

  // Установка коллбеков с потокобезопасностью
  void setCommandCallback(CommandCallback callback) {
    std::lock_guard lock(callbackMutex_);
    commandCallback_ = callback;
  }

  void setStatusCallback(StatusCallback callback) {
    std::lock_guard lock(callbackMutex_);
    statusCallback_ = callback;
  }

  void setErrorCallback(ErrorCallback callback) {
    std::lock_guard lock(callbackMutex_);
    errorCallback_ = callback;
  }

  // Запуск и остановка клиента
  void start() {
    if (!isRunning_) {
      logger->info("Starting UdpClient");
      logger->info("Listening port: {}", socket_.local_endpoint().port());
      logger->info("Destination: {}:{}", serverEndpoint_.address().to_string().c_str(), serverEndpoint_.port());
      isRunning_ = true;
      workerThread_ = std::thread(&UdpClient::run, this);
    }
  }

  void stop() {
    if (isRunning_) {
      logger->info("Stopping UdpClient");
      isRunning_ = false;
      ioContext_.stop();
      if (workerThread_.joinable()) {
        workerThread_.join();
      }
    }
  }

  // Отправка команды
  void sendCommand(const std::vector<uint8_t>& command) {
    if (!isRunning_) {
      logger->warn("Could not enqueue command. Client is closed!");
      return;
    }

    if (!commandBuffer_.try_enqueue(command)) {
      logger->warn("Failed to enqueue command. Command queue is full");
    }
  }

private:
  void run() {
    try {
      listenForResponses();
    } catch (const std::exception& e) {
      logger->error("Got error during listenForResponses: {}", e.what());
      invokeErrorCallback(e.what(), 0xFF);
    }
  }

  void listenForResponses() {
    while (isRunning_) {
      std::array<uint8_t, 1024> buffer;
      asio::ip::udp::endpoint senderEndpoint;
      asio::error_code ec;

      size_t length = socket_.receive_from(asio::buffer(buffer), senderEndpoint, 0, ec);

      if (!ec && length > 0) {
        if (std::vector response(buffer.begin(), buffer.begin() + length); response[0] == 0xF0) {
          invokeCommandCallback(response);
        } else if (response[0] == 0xF1) {
          invokeStatusCallback(response);
        } else if (response[0] == MessageType_Error) {
          invokeErrorCallback("Received error from device", (response.size() >= 5) ? response[4] : 0xFF);
        } else {
          invokeErrorCallback("Unknown header", 0xFF);
        }
      } else if (ec) {
        invokeErrorCallback("Error receiving data: " + ec.message(), 0xFF);
      }

      processCommands();
    }
  }

  void processCommands() {
    std::vector<uint8_t> command;
    while (commandBuffer_.try_dequeue(command)) {
      asio::error_code ec;
      socket_.send_to(asio::buffer(command), serverEndpoint_, 0, ec);

      if (ec) {
        logger->error("Failed to send command: {}", ec.message());
        invokeErrorCallback("Failed to send command: " + ec.message(), 0xFF);
      }
    }
  }

  // Вызов коллбеков с потокобезопасностью
  void invokeCommandCallback(const std::vector<uint8_t>& response) {
    std::lock_guard lock(callbackMutex_);
    if (commandCallback_) {
      commandCallback_(response);
    }
  }

  void invokeStatusCallback(const std::vector<uint8_t>& status) {
    std::lock_guard lock(callbackMutex_);
    if (statusCallback_) {
      statusCallback_(status);
    }
  }

  void invokeErrorCallback(const std::string& error, const uint8_t code) {
    std::lock_guard lock(callbackMutex_);
    if (errorCallback_) {
      errorCallback_(error, code);
    }
  }

  asio::io_context ioContext_;
  asio::ip::udp::socket socket_;
  asio::ip::udp::endpoint serverEndpoint_;
  moodycamel::BlockingReaderWriterCircularBuffer<std::vector<uint8_t> > commandBuffer_{10};

  std::thread workerThread_;
  std::atomic_bool isRunning_;

  std::string serverIp_;
  uint16_t serverPort_;

  mutable std::mutex connectionMutex_;
  mutable std::mutex callbackMutex_;

  CommandCallback commandCallback_;
  StatusCallback statusCallback_;
  ErrorCallback errorCallback_;
};
} // vanch