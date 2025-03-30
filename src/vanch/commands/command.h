#pragma once

#include "vanch/message.h"

#define VANCH_DEFINE_CMD_RET(MessageName, cmdCode, description)                    \
  VANCH_DEFINE_TRAITS(Cmd##MessageName, cmdCode, MessageType_Command, description) \
  VANCH_DEFINE_TRAITS(Ret##MessageName, cmdCode, MessageType_Return, Cmd##MessageName##_Traits::getName())

#define VANCH_CMD_BEGIN(messageName, cmdCode, description)                         \
  VANCH_DEFINE_TRAITS(Cmd##messageName, cmdCode, MessageType_Command, description) \
  struct Cmd##messageName : Message<Cmd##messageName##_Traits> {                   \
    std::vector<uint8_t> serializeParameters() const override;                     \
    void render() override;

#define VANCH_CMD_END() \
  }                     \
  ;

#define VANCH_CMD_EMPTY(messageName, cmdCode, description)                         \
  VANCH_DEFINE_TRAITS(Cmd##messageName, cmdCode, MessageType_Command, description) \
  struct Cmd##messageName : Message<Cmd##messageName##_Traits> {};

#define VANCH_RET_BEGIN(messageName)                                                              \
  VANCH_DEFINE_TRAITS(Ret##messageName, Cmd##messageName##_Traits::s_cmdCode, MessageType_Return, \
                      Cmd##messageName##_Traits::getName())                                       \
  struct Ret##messageName : Message<Ret##messageName##_Traits> {                                  \
    void deserializeParameters(std::span<const uint8_t> data) override;                           \
    void render() override;

#define VANCH_RET_END() \
  }                     \
  ;

#define VANCH_RET_EMPTY(messageName)                                                              \
  VANCH_DEFINE_TRAITS(Ret##messageName, Cmd##messageName##_Traits::s_cmdCode, MessageType_Return, \
                      Cmd##messageName##_Traits::getName())                                       \
  struct Ret##messageName : Message<Ret##messageName##_Traits> {};

namespace vanch {

VANCH_CMD_BEGIN(SetBaudRate, 0x01, "Set baud rate")
uint8_t interfaceType{};  // 0=RS232, 1=RS485
uint8_t baudRateCode{};   // Index of baud rate
[[nodiscard]] static uint32_t getBaudRate(uint8_t code);
VANCH_CMD_END()
VANCH_RET_EMPTY(SetBaudRate)

VANCH_CMD_BEGIN(GetBaudRate, 0x02, "Get baud rate");
uint8_t interfaceType{};
VANCH_CMD_END();
VANCH_RET_BEGIN(GetBaudRate)
uint8_t baudRateCode{};
VANCH_RET_END();

VANCH_CMD_BEGIN(SetRS485Address, 0x03, "Set RS485 address")
uint8_t address{};  // RS485 address
VANCH_CMD_END()
VANCH_RET_EMPTY(SetRS485Address)

VANCH_CMD_BEGIN(GetRS485Address, 0x04, "Get RS485 address")
VANCH_CMD_END()
VANCH_RET_BEGIN(GetRS485Address)
uint8_t address{};
VANCH_RET_END()

VANCH_CMD_EMPTY(GetVersionNumber, 0x05, "Get version number")
VANCH_RET_BEGIN(GetVersionNumber)
std::string versionInfo{};
VANCH_RET_END();

VANCH_CMD_BEGIN(SetRelayStatus, 0x06, "Set relay status")
uint8_t relayNumber{};
uint8_t relayState{};

VANCH_CMD_END()
VANCH_RET_EMPTY(SetRelayStatus)

VANCH_CMD_BEGIN(GetRelayStatus, 0x08, "Get relay status")
uint8_t relayNumber{};
VANCH_CMD_END()
VANCH_RET_BEGIN(GetRelayStatus)
uint8_t relayState{};
VANCH_RET_END()

VANCH_CMD_BEGIN(SetBuzzer, 0x0B, "Set buzzer")
uint8_t state{};  // 0=Off, 1=On
VANCH_CMD_END()
VANCH_RET_EMPTY(SetBuzzer)

VANCH_CMD_EMPTY(GetBuzzer, 0x0C, "Get buzzer")
VANCH_RET_BEGIN(GetBuzzer)
uint8_t state{};
VANCH_RET_END()

VANCH_CMD_BEGIN(SetCardReadingMode, 0x0D, "Set card reading mode")
uint8_t mode{};
VANCH_CMD_END()
VANCH_RET_EMPTY(SetCardReadingMode)

VANCH_CMD_EMPTY(GetCardReadingMode, 0x0E, "Get card reading mode")
VANCH_RET_BEGIN(GetCardReadingMode)
uint8_t mode{};
VANCH_RET_END()

VANCH_CMD_BEGIN(SetOutputPower, 0x0F, "Set output power")
uint8_t antennaNumber{};
uint8_t powerValue{};
VANCH_CMD_END()
VANCH_RET_EMPTY(SetOutputPower)

VANCH_CMD_BEGIN(GetOutputPower, 0x10, "Get output power")
uint8_t antennaNumber{};
VANCH_CMD_END()
VANCH_RET_BEGIN(GetOutputPower)
uint8_t powerValue{};
VANCH_RET_END()

}  // namespace vanch
