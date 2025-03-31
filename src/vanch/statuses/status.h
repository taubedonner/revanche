#pragma once

#include <nlohmann/json.hpp>

#include "vanch/message.h"

using json = nlohmann::json;

// Helper function to serialize JSON into bytes
static std::vector<uint8_t> serializeJson(const json& jsonData) {
  std::string jsonString = jsonData.dump();
  return {jsonString.begin(), jsonString.end()};
}

// Helper function to deserialize JSON from bytes
static json deserializeJson(const std::span<const uint8_t>& data) {
  std::string jsonString(data.begin(), data.end());
  return json::parse(jsonString);
}

namespace vanch {

VANCH_DEFINE_TRAITS(StatusAutoCardReading, 0x01, MessageType_Status, "Automatic Card Reading Data Packet");

class StatusAutoCardReading final : public Message<StatusAutoCardReading_Traits> {
 public:
  uint8_t antennaNumber{};
  std::vector<uint8_t> triggeredChannels;
  std::string direction;
  std::string ipAddress;
  std::string epc;
  std::string tid;
  std::string userArea;
  std::string deviceId;
  int8_t rssi{};
  uint32_t timestamp{};
  uint8_t tagType{};
  std::vector<std::string> customFields;
  float temperature{};

  void render() override;

  [[nodiscard]] std::vector<uint8_t> serializeParameters() const override;

  void deserializeParameters(std::span<const uint8_t> data) override;
};

VANCH_DEFINE_TRAITS(StatusUdpBroadcast, 0x02, MessageType_Status, "UDP Broadcast Package");

class StatusUdpBroadcast final : public Message<StatusUdpBroadcast_Traits> {
public:
    std::string ipAddress;
    uint16_t port{};
    std::string deviceType;
    std::string deviceId;
    uint8_t rs485Address{};
    uint32_t rs232BaudRate{};
    uint32_t rs485BaudRate{};
    uint8_t internalModel{};

    void render() override;

    [[nodiscard]] std::vector<uint8_t> serializeParameters() const override;

    void deserializeParameters(std::span<const uint8_t> data) override;
};

VANCH_DEFINE_TRAITS(StatusHeartbeat, 0x03, MessageType_Status, "Heartbeat");

class StatusHeartbeat final : public Message<StatusHeartbeat_Traits> {
 public:
  std::string heartbeatData;

  void render() override;

  [[nodiscard]] std::vector<uint8_t> serializeParameters() const override;

  void deserializeParameters(std::span<const uint8_t> data) override;
};

}  // namespace vanch
