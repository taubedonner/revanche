#pragma once

#include <imgui.h>

#include <numeric>

namespace vanch {
enum MessageType : uint8_t {
  MessageType_Unknown = 0x00,
  MessageType_Command = 0x40,
  MessageType_Return = 0xF0,
  MessageType_Status = 0xF1,
  MessageType_Error = 0xF4,
};

struct IMessage {
  virtual ~IMessage() = default;

  virtual std::string getMessageName() const = 0;

  virtual MessageType getType() const = 0;

  virtual uint8_t getCmdCode() const = 0;

  virtual void render() {
    ImGui::TextUnformatted("No parameters to display");
  };

  virtual std::vector<uint8_t> serialize(uint8_t address = 0x00) const = 0;

  void deserialize(const std::vector<uint8_t>& data) {
    deserialize(std::span{data});
  };

  virtual void deserialize(std::span<const uint8_t> data) = 0;

  virtual void deserializeParameters(std::span<const uint8_t> data) {
  };

  [[nodiscard]] virtual std::vector<uint8_t> serializeParameters() const { return {}; }
};

template <typename T>
struct Message : IMessage {
  std::string getMessageName() const override { return T::getName(); }

  MessageType getType() const override { return T::s_header; }

  uint8_t getCmdCode() const override { return T::s_cmdCode; }

  void deserialize(std::span<const uint8_t> data) override {
    deserializeParameters({data.begin() + 5, data.end() - 1});
  }

  [[nodiscard]] std::vector<uint8_t> serialize(const uint8_t address = 0x00) const override {
    std::vector<uint8_t> packet;

    packet.push_back(getType()); // 0: Header

    packet.push_back(0); // 1: Temp Length H
    packet.push_back(0); // 2: Temp Length L

    packet.push_back(address); // 3: Address

    packet.push_back(getCmdCode()); // 4: Cmd

    auto params = serializeParameters();
    packet.insert(packet.end(), params.begin(), params.end()); // 5-n: Param

    const auto length = static_cast<uint16_t>(packet.size() - 3 + 1); // (Address + Cmd + Params + Checksum)
    packet[1] = static_cast<uint8_t>((length >> 8) & 0xFF);
    packet[2] = static_cast<uint8_t>(length & 0xFF);

    const uint8_t checksum = calculateChecksum(packet);
    packet.push_back(checksum); // n-1: Check

    return packet;
  }

protected:
  static uint8_t calculateChecksum(const std::vector<uint8_t>& data) {
    const uint8_t sum = std::accumulate(data.begin(), data.end(), 0);
    return (~sum) + 1;
  }
};

struct Error_Traits {
  struct Parameters {
  };

  static constexpr uint8_t s_cmdCode = 0xFF;
  static constexpr MessageType s_header = MessageType_Error;

  static constexpr const char* getName() { return "Unknown Error"; }

  inline static std::vector<uint8_t> serializeParameters(const Message<Error_Traits>&) { return {}; }

  inline static void deserializeParameters(Message<Error_Traits>&, std::span<const uint8_t>) {
  }

  inline static void render(Message<Error_Traits>&) {
  }
};

using Error = Message<Error_Traits>;
} // namespace vanch

#define VANCH_DEFINE_TRAITS(MessageName, cmdCode, headerType, description) \
struct MessageName##_Traits { \
    static constexpr uint8_t s_cmdCode = cmdCode; \
    static constexpr MessageType s_header = headerType; \
    static constexpr const char* getName() { return description; } \
};