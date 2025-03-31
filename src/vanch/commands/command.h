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

// Basic commands

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

VANCH_CMD_EMPTY(GetRS485Address, 0x04, "Get RS485 address")
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

VANCH_CMD_BEGIN(SetFrequencyPoints, 0x13, "Set frequency points")
uint8_t region{};
uint8_t startFreq{};
uint8_t endFreq{};
VANCH_CMD_END()
VANCH_RET_EMPTY(SetFrequencyPoints)

VANCH_CMD_EMPTY(GetFrequencyPoints, 0x14, "Get frequency points")
VANCH_RET_BEGIN(GetFrequencyPoints)
uint8_t region{};
uint8_t startFreq{};
uint8_t endFreq{};
VANCH_RET_END()

VANCH_CMD_EMPTY(RestoreFactorySettings, 0x17, "Restore factory settings")
VANCH_RET_EMPTY(RestoreFactorySettings)

VANCH_CMD_EMPTY(RestartReader, 0x18, "Restart reader")
VANCH_RET_EMPTY(RestartReader)

VANCH_CMD_EMPTY(RestoreWIFISettings, 0x19, "Restore WIFI settings")
VANCH_RET_EMPTY(RestoreWIFISettings)

VANCH_CMD_BEGIN(SetReaderTime, 0x1B, "Set reader time")
uint8_t year{};
uint8_t month{};
uint8_t day{};
uint8_t hour{};
uint8_t minute{};
uint8_t second{};
VANCH_CMD_END()
VANCH_RET_EMPTY(SetReaderTime)

VANCH_CMD_EMPTY(GetReaderTime, 0x1C, "Get reader time")
VANCH_RET_BEGIN(GetReaderTime)
uint8_t year{};
uint8_t month{};
uint8_t day{};
uint8_t hour{};
uint8_t minute{};
uint8_t second{};
VANCH_RET_END()

VANCH_CMD_BEGIN(SetTagFilter, 0x1F, "Set tag filter")
uint8_t isEnabled{};
uint8_t maskAddress{};
uint8_t maskLength{};
std::vector<uint8_t> maskData;
VANCH_CMD_END()
VANCH_RET_EMPTY(SetTagFilter)

VANCH_CMD_EMPTY(GetTagFilter, 0x20, "Get tag filter")
VANCH_RET_BEGIN(GetTagFilter)
uint8_t isEnabled{};
uint8_t maskAddress{};
uint8_t maskLength{};
std::vector<uint8_t> maskData;
VANCH_RET_END()

VANCH_CMD_BEGIN(SetRJ45LocalParams, 0x23, "Set RJ45 local network parameters")
static void renderIPAddress(const char* label, std::array<uint8_t, 4>& ip);
std::array<uint8_t, 4> ip{};
std::array<uint8_t, 4> mask{};
std::array<uint8_t, 4> gateway{};
uint16_t port{};
VANCH_CMD_END()
VANCH_RET_EMPTY(SetRJ45LocalParams)

VANCH_CMD_EMPTY(GetRJ45LocalParams, 0x24, "Get RJ45 local network parameters")
VANCH_RET_BEGIN(GetRJ45LocalParams)
std::array<uint8_t, 4> ip{};
std::array<uint8_t, 4> mask{};
std::array<uint8_t, 4> gateway{};
uint16_t port{};
VANCH_RET_END()

VANCH_CMD_BEGIN(SetMACAddress, 0x25, "Set MAC address")
std::array<uint8_t, 6> mac{};
VANCH_CMD_END()
VANCH_RET_EMPTY(SetMACAddress)

VANCH_CMD_EMPTY(GetMACAddress, 0x26, "Get MAC address")
VANCH_RET_BEGIN(GetMACAddress)
std::array<uint8_t, 6> mac{};
VANCH_RET_END()

VANCH_CMD_BEGIN(SetRJ45RemoteParams, 0x27, "Set RJ45 remote network parameters")
std::array<uint8_t, 4> udpServerIP{};
uint16_t udpServerPort{};
uint8_t udpEnabled{};
std::array<uint8_t, 4> tcpServerIP{};
uint16_t tcpServerPort{};
uint8_t tcpEnabled{};
VANCH_CMD_END()
VANCH_RET_EMPTY(SetRJ45RemoteParams)

VANCH_CMD_EMPTY(GetRJ45RemoteParams, 0x28, "Get RJ45 remote network parameters")
VANCH_RET_BEGIN(GetRJ45RemoteParams)
std::array<uint8_t, 4> udpServerIP{};
uint16_t udpServerPort{};
uint8_t udpEnabled{};
std::array<uint8_t, 4> tcpServerIP{};
uint16_t tcpServerPort{};
uint8_t tcpEnabled{};
VANCH_RET_END()

VANCH_CMD_BEGIN(SetTagAlarm, 0x31, "Set tag alarm")
uint8_t isEnabled{};
uint8_t maskAddress{};
uint8_t maskLength{};
std::vector<uint8_t> maskData;
VANCH_CMD_END()
VANCH_RET_EMPTY(SetTagAlarm)

VANCH_CMD_EMPTY(GetTagAlarm, 0x32, "Get tag alarm")
VANCH_RET_BEGIN(GetTagAlarm)
uint8_t isEnabled{};
uint8_t maskAddress{};
uint8_t maskLength{};
std::vector<uint8_t> maskData;
VANCH_RET_END()

VANCH_CMD_BEGIN(SetReaderID, 0x33, "Set reader ID")
std::string readerID;
VANCH_CMD_END()
VANCH_RET_EMPTY(SetReaderID)

VANCH_CMD_EMPTY(GetReaderID, 0x34, "Get reader ID")
VANCH_RET_BEGIN(GetReaderID)
std::string readerID;
VANCH_RET_END()

VANCH_CMD_BEGIN(SetReaderName, 0x35, "Set reader name")
std::string readerName;
VANCH_CMD_END()
VANCH_RET_EMPTY(SetReaderName)

VANCH_CMD_EMPTY(GetReaderName, 0x36, "Get reader name")
VANCH_RET_BEGIN(GetReaderName)
std::string readerName;
VANCH_RET_END()

VANCH_CMD_BEGIN(SetHeartbeatPacket, 0x39, "Set heartbeat packet parameters")
uint8_t isEnabled{};
uint8_t interval{};
std::string heartbeatData;
VANCH_CMD_END()
VANCH_RET_EMPTY(SetHeartbeatPacket)

VANCH_CMD_EMPTY(GetHeartbeatPacket, 0x43, "Get heartbeat packet parameters")
VANCH_RET_BEGIN(GetHeartbeatPacket)
uint8_t isEnabled{};
uint8_t interval{};
std::string heartbeatData;
VANCH_RET_END()

VANCH_CMD_BEGIN(SetWIFIParams, 0x4D, "Set WIFI parameters")
uint8_t wifiMode{};
std::array<uint8_t, 4> localIP{};
std::array<uint8_t, 4> subnetMask{};
std::array<uint8_t, 4> gateway{};
uint16_t port{};
std::array<uint8_t, 4> remoteIP{};
uint8_t ssidLength{};
std::string ssid;
uint8_t passwordLength{};
std::string password;
VANCH_CMD_END()
VANCH_RET_EMPTY(SetWIFIParams)

VANCH_CMD_EMPTY(GetWIFIParams, 0x4E, "Get WIFI parameters")
VANCH_RET_BEGIN(GetWIFIParams)
uint8_t wifiMode{};
std::array<uint8_t, 4> localIP{};
std::array<uint8_t, 4> subnetMask{};
std::array<uint8_t, 4> gateway{};
uint16_t port{};
std::array<uint8_t, 4> remoteIP{};
uint8_t ssidLength{};
std::string ssid;
uint8_t passwordLength{};
std::string password;
VANCH_RET_END()

VANCH_CMD_BEGIN(SetSuperNetworkParams, 0x4F, "Set super network port parameters")
uint8_t mode{};
std::array<uint8_t, 4> ipAddress{};
std::array<uint8_t, 4> subnetMask{};
std::array<uint8_t, 4> gateway{};
uint16_t port{};
std::array<uint8_t, 4> remoteIP{};
VANCH_CMD_END()
VANCH_RET_EMPTY(SetSuperNetworkParams)

VANCH_CMD_EMPTY(GetSuperNetworkParams, 0x50, "Get super network port parameters")
VANCH_RET_BEGIN(GetSuperNetworkParams)
uint8_t mode{};
std::array<uint8_t, 4> ipAddress{};
std::array<uint8_t, 4> subnetMask{};
std::array<uint8_t, 4> gateway{};
uint16_t port{};
std::array<uint8_t, 4> remoteIP{};
VANCH_RET_END()

VANCH_CMD_BEGIN(Set4GParams, 0x51, "Set 4G parameters")
uint8_t transmissionMode{};
uint16_t port{};
uint8_t addressLength{};
std::string address;
VANCH_CMD_END()
VANCH_RET_EMPTY(Set4GParams)

VANCH_CMD_EMPTY(Get4GParams, 0x52, "Get 4G parameters")
VANCH_RET_BEGIN(Get4GParams)
uint8_t transmissionMode{};
uint16_t port{};
uint8_t addressLength{};
std::string address;
VANCH_RET_END()

VANCH_CMD_BEGIN(GetGPIStatus, 0x60, "Get GPI status")
uint8_t gpiNumber{};
VANCH_CMD_END()
VANCH_RET_BEGIN(GetGPIStatus)
uint8_t gpiLevel{};
VANCH_RET_END()

// Automatic mode related commands

VANCH_CMD_BEGIN(SetRelayAutoControlParams, 0x09, "Set relay automatic control parameters")
uint8_t relayNumber{};  // Relay number (1, 2, 3...)
uint8_t relayPurpose{}; // Purpose of relay (0=No auto control, 1=Activate on tag read, 2=Activate on alarm condition)
uint8_t pickupTime{};   // Pickup time in seconds
VANCH_CMD_END()
VANCH_RET_EMPTY(SetRelayAutoControlParams)

VANCH_CMD_BEGIN(GetRelayAutoControlParams, 0x0A, "Get relay automatic control parameters")
uint8_t relayNumber{}; // Relay number (1, 2, 3...)
VANCH_CMD_END()
VANCH_RET_BEGIN(GetRelayAutoControlParams)
uint8_t relayPurpose{}; // Purpose of relay
uint8_t pickupTime{};   // Pickup time in seconds
VANCH_RET_END()

VANCH_CMD_BEGIN(SetAutoPollingAntenna, 0x15, "Set automatic polling antenna")
std::vector<uint8_t> antennas{}; // List of antenna numbers (1, 2, 3, 4...)
VANCH_CMD_END()
VANCH_RET_EMPTY(SetAutoPollingAntenna)

VANCH_CMD_EMPTY(GetAutoPollingAntenna, 0x16, "Get automatic polling antenna")
VANCH_RET_BEGIN(GetAutoPollingAntenna)
std::vector<uint8_t> antennas{}; // List of antenna numbers (1, 2, 3, 4...)
VANCH_RET_END()

VANCH_CMD_BEGIN(SetAutoReadTagType, 0x21, "Set the automatic reading tag type")
uint8_t tagTypeBitmap{}; // Bitmap for tag types (ISO18000-6B, ISO18000-6C, Temperature Tag)
VANCH_CMD_END()
VANCH_RET_EMPTY(SetAutoReadTagType)

VANCH_CMD_EMPTY(GetAutoReadTagType, 0x22, "Get the automatically read tag type")
VANCH_RET_BEGIN(GetAutoReadTagType)
uint8_t tagTypeBitmap{}; // Bitmap for tag types
VANCH_RET_END()

VANCH_CMD_BEGIN(SetReportedHardwareInterface, 0x29, "Set the reported hardware interface")
uint8_t interfaceNumber{}; // Hardware interface number
uint8_t enableReporting{}; // Whether to enable reporting (0=off, 1=on)
VANCH_CMD_END()
VANCH_RET_EMPTY(SetReportedHardwareInterface)

VANCH_CMD_BEGIN(GetReportedHardwareInterface, 0x2A, "Gets the reported hardware interface")
uint8_t interfaceNumber{}; // Hardware interface number
VANCH_CMD_END()
VANCH_RET_BEGIN(GetReportedHardwareInterface)
uint8_t enableReporting{}; // Whether reporting is enabled (0=off, 1=on)
VANCH_RET_END()

VANCH_CMD_BEGIN(SetAutoReportingFields, 0x2B, "Set automatic reporting fields")
uint32_t fieldBitmap{}; // Bitmap for selecting fields (32-bit)
uint8_t tidStartAddress{};
uint8_t tidLength{};
uint8_t userStartAddress{};
uint8_t userLength{};
VANCH_CMD_END()
VANCH_RET_EMPTY(SetAutoReportingFields)

VANCH_CMD_EMPTY(GetAutoReportingFields, 0x2C, "Get the automatically reported fields")
VANCH_RET_BEGIN(GetAutoReportingFields)
uint32_t fieldBitmap{};
uint8_t tidStartAddress{};
uint8_t tidLength{};
uint8_t userStartAddress{};
uint8_t userLength{};
VANCH_RET_END()

VANCH_CMD_BEGIN(SetAutoReportingContent, 0x1A, "Set up automatic reporting content")
uint8_t customFieldNumber{}; // Custom field number (0-5)
std::array<uint8_t, 24> customContent; // Up to 24 bytes of ASCII data
VANCH_CMD_END()
VANCH_RET_EMPTY(SetAutoReportingContent)

VANCH_CMD_BEGIN(GetAutoReportingContent, 0x44, "Get automatically reported content")
uint8_t customFieldNumber{}; // Custom field number (0-5)
VANCH_CMD_END()
VANCH_RET_BEGIN(GetAutoReportingContent)
std::array<uint8_t, 24> customContent; // Up to 24 bytes of ASCII data
VANCH_RET_END()

VANCH_CMD_BEGIN(SetAutoReportingConditions, 0x37, "Set automatic reporting conditions")
uint8_t reportingMode{}; // Reporting mode (0-4)
uint8_t reportingInterval{}; // Reporting interval in seconds
VANCH_CMD_END()
VANCH_RET_EMPTY(SetAutoReportingConditions)

VANCH_CMD_EMPTY(GetAutoReportingConditions, 0x38, "Get automatic reporting conditions")
VANCH_RET_BEGIN(GetAutoReportingConditions)
uint8_t reportingMode{};
uint8_t reportingInterval{};
VANCH_RET_END()

VANCH_CMD_BEGIN(SetWiegandParams, 0x2D, "Set Wiegand parameters")
uint8_t pulseWidth{}; // Wiegand pulse width (10us units)
uint8_t pulseInterval{}; // Wiegand pulse interval (10us units)
VANCH_CMD_END()
VANCH_RET_EMPTY(SetWiegandParams)

VANCH_CMD_EMPTY(GetWiegandParams, 0x2E, "Get Wiegand parameters")
VANCH_RET_BEGIN(GetWiegandParams)
uint8_t pulseWidth{};
uint8_t pulseInterval{};
VANCH_RET_END()

VANCH_CMD_BEGIN(SetTriggerConditions, 0x2F, "Set trigger conditions")
uint8_t triggerNumber{}; // Trigger number (1, 2, 3...)
uint8_t triggerLevel{}; // Trigger level (0=low, 1=high)
uint8_t triggerDuration{}; // Trigger duration in seconds
VANCH_CMD_END()
VANCH_RET_EMPTY(SetTriggerConditions)

VANCH_CMD_BEGIN(GetTriggerConditions, 0x30, "Get trigger conditions")
uint8_t triggerNumber{}; // Trigger number (1, 2, 3...)
VANCH_CMD_END()
VANCH_RET_BEGIN(GetTriggerConditions)
uint8_t triggerLevel{};
uint8_t triggerDuration{};
VANCH_RET_END()

// High frequency protocol

VANCH_CMD_BEGIN(Read15693Tag, 0x71, "Read 15693 tags (4 bytes)")
uint8_t uidLength{}; // UID length in bytes
std::vector<uint8_t> uid; // UID of the tag
uint8_t startBlock{}; // Starting block number
uint8_t numBlocks{}; // Number of blocks to read
VANCH_CMD_END()
VANCH_RET_BEGIN(Read15693Tag)
std::vector<uint8_t> tagData; // Data read from the tag
VANCH_RET_END()

VANCH_CMD_BEGIN(Write15693Tag, 0x72, "Write 15693 tags (4 bytes)")
uint8_t uidLength{}; // UID length in bytes
std::vector<uint8_t> uid; // UID of the tag
uint8_t writeBlock{}; // Block address to write
std::array<uint8_t, 4> writeData; // 4 bytes of data to write
VANCH_CMD_END()
VANCH_RET_EMPTY(Write15693Tag)

VANCH_CMD_BEGIN(Write15693MultipleBlocks, 0x76, "Write 15693 label multiple blocks (4 bytes)")
uint8_t uidLength{}; // UID length in bytes
std::vector<uint8_t> uid; // UID of the tag
uint8_t startBlock{}; // Starting block address
uint8_t numBlocks{}; // Number of blocks to write
std::vector<uint8_t> writeData; // n * 4 bytes of data to write
VANCH_CMD_END()
VANCH_RET_EMPTY(Write15693MultipleBlocks)

VANCH_CMD_BEGIN(ReadISO15693Tag, 0x78, "Read ISO15693 RFID tag")
uint8_t uidLength{}; // UID length in bytes
std::vector<uint8_t> uid; // UID of the tag
uint8_t startBlock{}; // Starting block number
uint8_t numBlocks{}; // Number of blocks to read
uint8_t blockSize{}; // Size of each block in bytes
VANCH_CMD_END()
VANCH_RET_BEGIN(ReadISO15693Tag)
std::vector<uint8_t> tagData; // Data read from the tag
VANCH_RET_END()

VANCH_CMD_BEGIN(WriteISO15693Tag, 0x79, "Write ISO15693 Protocol RFID tag data block")
uint8_t uidLength{}; // UID length in bytes
std::vector<uint8_t> uid; // UID of the tag
uint8_t startBlock{}; // Starting block address
uint8_t numBlocks{}; // Number of blocks to write
uint8_t blockSize{}; // Size of each block in bytes
std::vector<uint8_t> writeData; // n * blockSize bytes of data to write
VANCH_CMD_END()
VANCH_RET_EMPTY(WriteISO15693Tag)

VANCH_CMD_BEGIN(Read14443ATag, 0x73, "Read 14443A label")
uint8_t uidLength{}; // UID length in bytes
std::vector<uint8_t> uid; // UID of the tag
uint8_t startBlock{}; // Starting block address
uint8_t numBlocks{}; // Number of blocks to read
VANCH_CMD_END()
VANCH_RET_BEGIN(Read14443ATag)
std::vector<uint8_t> tagData; // Data read from the tag
VANCH_RET_END()

VANCH_CMD_BEGIN(Write14443ATag, 0x74, "Write 14443A label")
uint8_t uidLength{}; // UID length in bytes
std::vector<uint8_t> uid; // UID of the tag
uint8_t writeBlock{}; // Block address to write
std::array<uint8_t, 4> writeData; // 4 bytes of data to write
VANCH_CMD_END()
VANCH_RET_EMPTY(Write14443ATag)

VANCH_CMD_BEGIN(Select14443ASector, 0x75, "Select 14443A label sector")
uint8_t uidLength{}; // UID length in bytes
std::vector<uint8_t> uid; // UID of the tag
uint8_t sectorNumber{}; // Sector number to select
VANCH_CMD_END()
VANCH_RET_EMPTY(Select14443ASector)

VANCH_CMD_BEGIN(Write14443AMultipleBlocks, 0x77, "Write multiple blocks of 14443A tags")
uint8_t uidLength{}; // UID length in bytes
std::vector<uint8_t> uid; // UID of the tag
uint8_t startBlock{}; // Starting block address
uint8_t numBlocks{}; // Number of blocks to write
std::vector<uint8_t> writeData; // n * 4 bytes of data to write
VANCH_CMD_END()
VANCH_RET_EMPTY(Write14443AMultipleBlocks)

}  // namespace vanch
