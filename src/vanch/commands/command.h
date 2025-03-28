#pragma once

#include "vanch/message.h"
#include "vanch/messageregistry.h"

#define VANCH_DEFINE_CMD_RET(MessageName, cmdCode, description) \
  VANCH_DEFINE_TRAITS(Cmd##MessageName, cmdCode, MessageType_Command, description) \
  VANCH_DEFINE_TRAITS(Ret##MessageName, cmdCode, MessageType_Return, Cmd##MessageName##_Traits::getName())

namespace vanch {

VANCH_DEFINE_CMD_RET(SetBaudRate, 0x01, "Set baud rate");

struct CmdSetBaudRate final : Message<CmdSetBaudRate_Traits> {
  std::vector<uint8_t> serializeParameters() const override;

  void render() override;

  uint8_t interfaceType{};
  uint8_t baudRateCode{};
};

struct RetSetBaudRate final : Message<RetSetBaudRate_Traits> {};

VANCH_DEFINE_CMD_RET(GetBaudRate, 0x02, "Get baud rate");

struct CmdGetBaudRate final : Message<CmdGetBaudRate_Traits> {
  std::vector<uint8_t> serializeParameters() const override;

  void render() override;

  uint8_t interfaceType{};
};

struct RetGetBaudRate final : Message<RetGetBaudRate_Traits> {
  void deserializeParameters(std::span<const uint8_t> data) override;

  void render() override;

  uint8_t baudRateCode{};
};

VANCH_DEFINE_CMD_RET(GetVersionNumber, 0x05, "Get version number");

struct CmdGetVersionNumber final : Message<CmdGetVersionNumber_Traits> {};

struct RetGetVersionNumber final : Message<RetGetVersionNumber_Traits> {
  void deserializeParameters(std::span<const uint8_t> data) override;

  void render() override;

  std::string versionInfo{};
};

}
