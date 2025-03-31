#include "messageregistry.h"

#include "commands/command.h"
#include "statuses/status.h"

#define REG_CMD_RET(name)                                                                                        \
  registerMessage(                                                                                               \
      Cmd##name##_Traits::s_cmdCode, Cmd##name##_Traits::s_header, [] { return std::make_shared<Cmd##name>(); }, \
      Cmd##name##_Traits::getName());                                                                            \
  registerMessage(                                                                                               \
      Ret##name##_Traits::s_cmdCode, Ret##name##_Traits::s_header, [] { return std::make_shared<Ret##name>(); }, \
      Ret##name##_Traits::getName());

#define REG(name)                                                                                 \
  registerMessage(                                                                                \
      name##_Traits::s_cmdCode, name##_Traits::s_header, [] { return std::make_shared<name>(); }, \
      name##_Traits::getName());

namespace vanch {

void MessageRegistry::init() {
  static bool initialized = false;
  if (initialized) return;

  REG_CMD_RET(SetBaudRate)
  REG_CMD_RET(GetBaudRate)
  REG_CMD_RET(SetRS485Address)
  REG_CMD_RET(GetRS485Address)
  REG_CMD_RET(GetVersionNumber)
  REG_CMD_RET(SetRelayStatus)
  REG_CMD_RET(GetRelayStatus)
  REG_CMD_RET(SetBuzzer)
  REG_CMD_RET(GetBuzzer)
  REG_CMD_RET(SetCardReadingMode)
  REG_CMD_RET(GetCardReadingMode)
  REG_CMD_RET(SetOutputPower)
  REG_CMD_RET(GetOutputPower)
  // REG_CMD_RET(SetFrequencyPoints)
  // REG_CMD_RET(GetFrequencyPoints)
  REG_CMD_RET(RestoreFactorySettings)
  REG_CMD_RET(RestartReader)
  REG_CMD_RET(RestoreWIFISettings)
  REG_CMD_RET(SetReaderTime)
  REG_CMD_RET(GetReaderTime)
  REG_CMD_RET(SetTagFilter)
  REG_CMD_RET(GetTagFilter)
  REG_CMD_RET(SetRJ45LocalParams)
  REG_CMD_RET(GetRJ45LocalParams)
  REG_CMD_RET(SetRJ45RemoteParams)
  REG_CMD_RET(GetRJ45RemoteParams)
  REG_CMD_RET(SetMACAddress)
  REG_CMD_RET(GetMACAddress)
  REG_CMD_RET(SetTagAlarm)
  REG_CMD_RET(GetTagAlarm)
  REG_CMD_RET(SetReaderID)
  REG_CMD_RET(GetReaderID)
  REG_CMD_RET(SetReaderName)
  REG_CMD_RET(GetReaderName)
  REG_CMD_RET(SetHeartbeatPacket)
  REG_CMD_RET(GetHeartbeatPacket)
  REG_CMD_RET(SetWIFIParams)
  REG_CMD_RET(GetWIFIParams)
  REG_CMD_RET(SetSuperNetworkParams)
  REG_CMD_RET(GetSuperNetworkParams)
  REG_CMD_RET(Set4GParams)
  REG_CMD_RET(Get4GParams)
  REG_CMD_RET(GetGPIStatus)
  REG_CMD_RET(SetRelayAutoControlParams)
  REG_CMD_RET(GetRelayAutoControlParams)
  REG_CMD_RET(SetAutoPollingAntenna)
  REG_CMD_RET(GetAutoPollingAntenna)
  REG_CMD_RET(SetAutoReadTagType)
  REG_CMD_RET(GetAutoReadTagType)
  REG_CMD_RET(SetReportedHardwareInterface)
  REG_CMD_RET(GetReportedHardwareInterface)
  REG_CMD_RET(SetAutoReportingFields)
  REG_CMD_RET(GetAutoReportingFields)
  REG_CMD_RET(SetAutoReportingContent)
  REG_CMD_RET(GetAutoReportingContent)
  REG_CMD_RET(SetAutoReportingConditions)
  REG_CMD_RET(GetAutoReportingConditions)
  REG_CMD_RET(SetWiegandParams)
  REG_CMD_RET(GetWiegandParams)
  REG_CMD_RET(SetTriggerConditions)
  REG_CMD_RET(GetTriggerConditions)
  REG_CMD_RET(Read15693Tag)
  REG_CMD_RET(Write15693Tag)
  REG_CMD_RET(Write15693MultipleBlocks)
  REG_CMD_RET(ReadISO15693Tag)
  REG_CMD_RET(WriteISO15693Tag)
  REG_CMD_RET(Read14443ATag)
  REG_CMD_RET(Write14443ATag)
  REG_CMD_RET(Select14443ASector)
  REG_CMD_RET(Write14443AMultipleBlocks)

  REG(StatusAutoCardReading)
  REG(StatusUdpBroadcast)
  REG(StatusHeartbeat)
}

}  // namespace vanch
