#pragma once

#include "vanch/message.h"

#include <frozen/unordered_map.h>

namespace vanch {
struct CommandMetadata {
  uint8_t cmdCode;
  std::string name;
};

class MessageRegistry {
public:
  using FactoryFunction = std::function<std::shared_ptr<IMessage>()>;

  static void init();

  static void registerMessage(uint8_t cmdCode, MessageType type, const FactoryFunction& factory, const std::string& name) {
    const auto& registry = getRegistry();
    const auto key = std::make_pair(cmdCode, type);

    if (registry.contains(key)) {return;}

    getRegistry()[key] = factory;

    if (type == MessageType_Command) getCommandMetadata().push_back({cmdCode, name});
  }

  static std::shared_ptr<IMessage> create(uint8_t cmdCode, MessageType type) {
    auto& registry = getRegistry();
    const auto key = std::make_pair(cmdCode, type);
    if (const auto it = registry.find(key); it != registry.end()) {
      return it->second();
    }
    return nullptr;
  }

  static std::shared_ptr<IMessage> createFromData(std::span<const uint8_t> data) {
    if (data.size() < 6) return nullptr;

    auto type = static_cast<MessageType>(data[0]);
    auto cmdCode = data[4];

    std::shared_ptr<IMessage> msg{};

    auto& registry = getRegistry();
    const auto key = std::make_pair(cmdCode, type);
    if (const auto it = registry.find(key); it != registry.end()) {
      msg = it->second();
      msg->deserialize(data);
    }

    if (type == MessageType_Error) {
      msg = std::make_shared<Error>();
    }

    return msg;
  }

  static std::vector<CommandMetadata>& getCommandMetadata() {
    static std::vector<CommandMetadata> commandMetadata;
    return commandMetadata;
  }

  static constexpr std::string_view getErrorMessage(const uint8_t errorCode) {
    if (const auto it = m_errorDescriptions.find(errorCode); it != m_errorDescriptions.end()) {
      return it->second;
    }
    return "Unknown error";
  }

private:
  static std::map<std::pair<uint8_t, MessageType>, FactoryFunction>& getRegistry() {
    static std::map<std::pair<uint8_t, MessageType>, FactoryFunction> registry;
    return registry;
  }

  static constexpr frozen::unordered_map<uint8_t, std::string_view, 95> m_errorDescriptions = {
      {0x00, "0x00 Successful"},
      {0x01, "0x01 Unsupported function code"},
      {0x02, "0x02 Register address error"},
      {0x03, "0x03 Data range error"},
      {0x04, "0x04 Writing failed"},
      {0x05, "0x05 Confirm"},
      {0x06, "0x06 Server busy"},
      {0x07, "0x07 Reserved"},
      {0x08, "0x08 Storage parity error"},
      {0x09, "0x09 Reserved"},
      {0x0A, "0x0A Unavailable gateway path"},
      {0x0B, "0x0B Gateway target device failed to respond"},
      {0x0C, "0x0C Modbus access length error"},
      {0x0D, "0x0D This configuration item is not supported"},
      {0x0E, "0x0E Relay function error"},
      {0x0F, "0x0F Card reading mode error"},
      {0x10, "0x10 Tag type error"},
      {0x11, "0x11 The reporting interface is not supported"},
      {0x12, "0x12 This reporting mode is not supported"},
      {0x13, "0x13 Tag matching failed"},
      {0x14, "0x14 Reserved"},
      {0x15, "0x15 Reserved"},
      {0x16, "0x16 Parameter error"},
      {0x17, "0x17 The operated module returns failure"},
      {0x18, "0x18 No label"},
      {0x19, "0x19 Checksum error"},
      {0x1A, "0x1A This command is not supported"},
      {0x1B, "0x1B This antenna is not supported"},
      {0x1C, "0x1C Filter or alarm length is out of range"},
      {0x1D, "0x1D Access area out of range"},
      {0x1E, "0x1E Address or length is not a multiple of 2"},
      {0x1F, "0x1F The read length is out of range"},
      {0x20, "0x20 Writing length is not an integer multiple of 2"},
      {0x21, "0x21 Reserved"},
      {0x22, "0x22 EPC length is not an integer multiple of 2"},
      {0x23, "0x23 Power out of range"},
      {0x24, "0x24 When setting the baud rate, the specified interface number is wrong"},
      {0x25, "0x25 When setting the baud rate, the baud rate is out of range"},
      {0x26, "0x26 When setting the 485 address, the 485 address is out of range"},
      {0x27, "0x27 Hardware not supported"},
      {0x28, "0x28 This relay is not supported"},
      {0x29, "0x29 The relay is in automatic control state. Please turn off the automatic control relay first"},
      {0x2A, "0x2A Reserved"},
      {0x2B, "0x2B TID or USER area length error"},
      {0x2C, "0x2C TID or USER area address error"},
      {0x2D, "0x2D Requires switching the reader to command mode"},
      {0x2E, "0x2E Protocol error, only supports UDP or TCP reporting"},
      {0x2F, "0x2F RRFID module returns error"},
      {0x30, "0x30 Reserved"},
      {0x31, "0x31 The frame length field does not match the total frame length"},
      {0x32, "0x32 The parameter length is inconsistent with the allowed length of the command"},
      {0x33, "0x33 This road does not support custom reporting"},
      {0x34, "0x34 The length of the reported parameter exceeds the range"},
      {0x35, "0x35 Illegal data frame"},
      {0x36, "0x36 Reserved"},
      {0x37, "0x37 Key error (used in Boot, please do not modify it)"},
      {0x38, "0x38 Reserved"},
      {0x39, "0x39 File writing error (used in Boot, please do not modify)"},
      {0x3A, "0x3A Write address error (used in Boot, please do not modify)"},
      {0x3B, "0x3B Reserved"},
      {0x3C, "0x3C Other unknown errors"},
      {0x3D, "0x3D Reserved"},
      {0x3E, "0x3E Reserved"},
      {0x3F, "0x3F Reserved"},
      {0x40, "0x40 Flash erase failed"},
      {0x41, "0x41 This register does not support this command code"},
      {0x42, "0x42 Bootloader does not support flag bit operations (used in Boot, please do not modify)"},
      {0x43, "0x43 Currently in APP mode"},
      {0x44, "0x44 Frequency range error"},
      {0x45, "0x45 Band area error"},
      {0x46, "0x46 This trigger is not supported"},
      {0x47, "0x47 Wiegand output is not supported"},
      {0x48, "0x48 Switch value can only be 0 or 1"},
      {0x49, "0x49 When RFID sets the power, the module returns an error"},
      {0x4A, "0x4A RFID When setting the frequency band, the module returns an error"},
      {0x4B, "0x4B RFID When setting the antenna, the module returns an error"},
      {0x4C, "0x4C Buzzer parameter error"},
      {0x4D, "0x4D Reserved"},
      {0x4E, "0x4E Trigger level error"},
      {0x4F, "0x4F Reserved"},
      {0x50, "0x50 Relay status error"},
      {0x51, "0x51 High frequency reader/writer can only select EPC (UID)"},
      {0x52, "0x52 Reserved"},
      {0x53, "0x53 Reserved"},
      {0x54, "0x54 Reserved"},
      {0x55, "0x55 Reserved"},
      {0x56, "0x56 Reserved"},
      {0x57, "0x57 The number of configurations exceeds the maximum range supported by the hardware"},
      {0x58, "0x58 Needs to enable automatic work"},
      {0x59, "0x59 Wrong reader model"},
      {0x5A, "0x5A Reserved"},
      {0x5B, "0x5B Reserved"},
      {0x5C, "0x5C Reserved"},
      {0x5D, "0x5D Module assembly failed, the assembly position is occupied"},
      {0x5E, "0x5E This module is not installed and this parameter cannot be set and obtained"}
  };
};
} // vanch