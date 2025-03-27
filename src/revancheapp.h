#pragma once

#include <krog/entry.h>

#include "vanch/messageregistry.h"
#include "vanch/udpclient.h"

class RevancheApp final : public kr::Layer, protected kr::Loggable {
public:
  RevancheApp() : Layer("RevancheApp"), Loggable("App"), m_client("192.168.1.100", 1969) {
    m_registry.registerMessage(vanch::MessageType_Error, 0x00, "SUCCESSFUL", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x01, "UNSUPPORTED FUNCTION CODE", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x02, "REGISTER ADDRESS ERROR", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x03, "DATA RANGE ERROR", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x04, "WRITING FAILED", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x05, "CONFIRMATION REQUIRED", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x06, "SERVER BUSY", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x08, "STORAGE PARITY ERROR", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x0A, "UNAVAILABLE GATEWAY PATH", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x0B, "GATEWAY TARGET DEVICE FAILED TO RESPOND", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x0C, "MODBUS ACCESS LENGTH ERROR", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x0D, "THIS CONFIGURATION ITEM IS NOT SUPPORTED", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x0E, "RELAY FUNCTION ERROR", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x0F, "CARD READING MODE ERROR", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x10, "TAG TYPE ERROR", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x11, "THE REPORTING INTERFACE IS NOT SUPPORTED", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x12, "THIS REPORTING MODE IS NOT SUPPORTED", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x13, "TAG MATCHING FAILED", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x16, "PARAMETER ERROR", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x17, "THE OPERATED MODULE RETURNS FAILURE", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x18, "NO TAG", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x19, "CHECKSUM ERROR", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x1A, "THIS COMMAND IS NOT SUPPORTED", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x1B, "THIS ANTENNA IS NOT SUPPORTED", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x1C, "FILTER OR ALARM LENGTH IS OUT OF RANGE", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x1D, "ACCESS AREA OUT OF RANGE", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x1E, "ADDRESS OR LENGTH IS NOT A MULTIPLE OF 2", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x1F, "THE READ LENGTH IS OUT OF RANGE", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x20, "WRITING LENGTH IS NOT AN INTEGER MULTIPLE OF 2", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x22, "EPC LENGTH IS NOT AN INTEGER MULTIPLE OF 2", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x23, "POWER OUT OF RANGE", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x24, "WHEN SETTING THE BAUD RATE, THE SPECIFIED INTERFACE NUMBER IS WRONG", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x25, "WHEN SETTING THE BAUD RATE, THE BAUD RATE IS OUT OF RANGE", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x26, "WHEN SETTING THE RS485 ADDRESS, THE RS485 ADDRESS IS OUT OF RANGE", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x27, "HARDWARE NOT SUPPORTED", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x28, "THIS RELAY IS NOT SUPPORTED", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x29, "THE RELAY IS IN AUTOMATIC CONTROL STATE. PLEASE TURN OFF THE AUTOMATIC CONTROL RELAY FIRST",
                               vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x2B, "TID OR USER AREA LENGTH ERROR", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x2C, "TID OR USER AREA ADDRESS ERROR", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x2D, "REQUIRES SWITCHING THE READER TO COMMAND MODE", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x2E, "PROTOCOL ERROR, ONLY SUPPORTS UDP OR TCP REPORTING", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x2F, "RFID MODULE RETURNS ERROR", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x31, "THE FRAME LENGTH FIELD DOES NOT MATCH THE TOTAL FRAME LENGTH", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x32, "THE PARAMETER LENGTH IS INCONSISTENT WITH THE ALLOWED LENGTH OF THE COMMAND", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x33, "THIS ROAD DOES NOT SUPPORT CUSTOM REPORTING", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x34, "THE LENGTH OF THE REPORTED PARAMETER EXCEEDS THE RANGE", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x35, "ILLEGAL DATA FRAME", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x37, "KEY ERROR (USED IN BOOT, PLEASE DO NOT MODIFY IT)", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x39, "FILE WRITING ERROR (USED IN BOOT, PLEASE DO NOT MODIFY)", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x3A, "WRITE ADDRESS ERROR (USED IN BOOT, PLEASE DO NOT MODIFY)", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x3C, "OTHER UNKNOWN ERRORS", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x40, "FLASH ERASE FAILED", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x41, "THIS REGISTER DOES NOT SUPPORT THIS COMMAND CODE", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x42, "BOOTLOADER DOES NOT SUPPORT FLAG BIT OPERATIONS (USED IN BOOT, PLEASE DO NOT MODIFY)",
                               vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x43, "CURRENTLY IN APP MODE", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x44, "FREQUENCY RANGE ERROR", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x45, "BAND AREA ERROR", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x46, "THIS TRIGGER IS NOT SUPPORTED", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x47, "WIEGAND OUTPUT IS NOT SUPPORTED", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x48, "SWITCH VALUE CAN ONLY BE 0 OR 1", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x49, "WHEN RFID SETS THE POWER, THE MODULE RETURNS AN ERROR", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x4A, "RFID WHEN SETTING THE FREQUENCY BAND, THE MODULE RETURNS AN ERROR", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x4B, "RFID WHEN SETTING THE ANTENNA, THE MODULE RETURNS AN ERROR", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x4C, "BUZZER PARAMETER ERROR", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x4E, "TRIGGER LEVEL ERROR", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x50, "RELAY STATUS ERROR", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x51, "HIGH FREQUENCY READER/WRITER CAN ONLY SELECT EPC (UID)", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x57, "THE NUMBER OF CONFIGURATIONS EXCEEDS THE MAXIMUM RANGE SUPPORTED BY THE HARDWARE", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x58, "NEEDS TO ENABLE AUTOMATIC WORK", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x59, "WRONG READER MODEL", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x5D, "MODULE ASSEMBLY FAILED, THE ASSEMBLY POSITION IS OCCUPIED", vanch::ErrorResponse::create);
    m_registry.registerMessage(vanch::MessageType_Error, 0x5E, "THIS MODULE IS NOT INSTALLED AND THIS PARAMETER CANNOT BE SET AND OBTAINED", vanch::ErrorResponse::create);

    // Automatic card reading data status
    m_registry.registerMessage(vanch::MessageType_Status, 0x01, vanch::AutomaticCardReadingDataStatus::getName(), vanch::AutomaticCardReadingDataStatus::create);

    // Set baud rate command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x01, vanch::SetBaudRateCommand::getName(), vanch::SetBaudRateCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x01, vanch::SetBaudRateResponse::getName(), vanch::SetBaudRateResponse::create);

    // Get baud rate command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x02, vanch::GetBaudRateCommand::getName(), vanch::GetBaudRateCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x02, vanch::GetBaudRateResponse::getName(), vanch::GetBaudRateResponse::create);

    // Set RS485 address command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x03, vanch::SetRS485AddressCommand::getName(), vanch::SetRS485AddressCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x03, vanch::SetRS485AddressResponse::getName(), vanch::SetRS485AddressResponse::create);

    // Get RS485 address command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x04, vanch::GetRS485AddressCommand::getName(), vanch::GetRS485AddressCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x04, vanch::GetRS485AddressResponse::getName(), vanch::GetRS485AddressResponse::create);

    // Get version number command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x05, vanch::GetVersionNumberCommand::getName(), vanch::GetVersionNumberCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x05, vanch::GetVersionNumberResponse::getName(), vanch::GetVersionNumberResponse::create);

    // Set relay status command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x06, vanch::SetRelayStatusCommand::getName(), vanch::SetRelayStatusCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x06, vanch::SetRelayStatusResponse::getName(), vanch::SetRelayStatusResponse::create);

    // Get relay status command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x08, vanch::GetRelayStatusCommand::getName(), vanch::GetRelayStatusCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x08, vanch::GetRelayStatusResponse::getName(), vanch::GetRelayStatusResponse::create);

    // Set buzzer command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x0B, vanch::SetBuzzerCommand::getName(), vanch::SetBuzzerCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x0B, vanch::SetBuzzerResponse::getName(), vanch::SetBuzzerResponse::create);

    // Get buzzer command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x0C, vanch::GetBuzzerCommand::getName(), vanch::GetBuzzerCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x0C, vanch::GetBuzzerResponse::getName(), vanch::GetBuzzerResponse::create);

    // Set card reading mode command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x0D, vanch::SetCardReadingModeCommand::getName(), vanch::SetCardReadingModeCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x0D, vanch::SetCardReadingModeResponse::getName(), vanch::SetCardReadingModeResponse::create);

    // Get card reading mode command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x0E, vanch::GetCardReadingModeCommand::getName(), vanch::GetCardReadingModeCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x0E, vanch::GetCardReadingModeResponse::getName(), vanch::GetCardReadingModeResponse::create);

    // Set output power command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x0F, vanch::SetOutputPowerCommand::getName(), vanch::SetOutputPowerCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x0F, vanch::SetOutputPowerResponse::getName(), vanch::SetOutputPowerResponse::create);

    // Get output power command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x10, vanch::GetOutputPowerCommand::getName(), vanch::GetOutputPowerCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x10, vanch::GetOutputPowerResponse::getName(), vanch::GetOutputPowerResponse::create);

    // Set frequency band command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x13, vanch::SetFrequencyBandCommand::getName(), vanch::SetFrequencyBandCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x13, vanch::SetFrequencyBandResponse::getName(), vanch::SetFrequencyBandResponse::create);

    // Get frequency band command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x14, vanch::GetFrequencyBandCommand::getName(), vanch::GetFrequencyBandCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x14, vanch::GetFrequencyBandResponse::getName(), vanch::GetFrequencyBandResponse::create);

    // Restore factory settings command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x17, vanch::RestoreFactorySettingsCommand::getName(), vanch::RestoreFactorySettingsCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x17, vanch::RestoreFactorySettingsResponse::getName(), vanch::RestoreFactorySettingsResponse::create);

    // Restart reader command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x18, vanch::RestartReaderCommand::getName(), vanch::RestartReaderCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x18, vanch::RestartReaderResponse::getName(), vanch::RestartReaderResponse::create);

    // Restore WIFI command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x19, vanch::RestoreWIFICommand::getName(), vanch::RestoreWIFICommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x19, vanch::RestoreWIFIResponse::getName(), vanch::RestoreWIFIResponse::create);

    // Set reader time command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x1B, vanch::SetReaderTimeCommand::getName(), vanch::SetReaderTimeCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x1B, vanch::SetReaderTimeResponse::getName(), vanch::SetReaderTimeResponse::create);

    // Get reader time command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x1C, vanch::GetReaderTimeCommand::getName(), vanch::GetReaderTimeCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x1C, vanch::GetReaderTimeResponse::getName(), vanch::GetReaderTimeResponse::create);

    // Set tag filter command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x1F, vanch::SetTagFilterCommand::getName(), vanch::SetTagFilterCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x1F, vanch::SetTagFilterResponse::getName(), vanch::SetTagFilterResponse::create);

    // Get tag filter command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x20, vanch::GetTagFilterCommand::getName(), vanch::GetTagFilterCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x20, vanch::GetTagFilterResponse::getName(), vanch::GetTagFilterResponse::create);

    // Set reported hardware interface command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x29, vanch::SetReportedHardwareInterfaceCommand::getName(), vanch::SetReportedHardwareInterfaceCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x29, vanch::SetReportedHardwareInterfaceResponse::getName(), vanch::SetReportedHardwareInterfaceResponse::create);

    // Get reported hardware interface command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x2A, vanch::GetReportedHardwareInterfaceCommand::getName(), vanch::GetReportedHardwareInterfaceCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x2A, vanch::GetReportedHardwareInterfaceResponse::getName(), vanch::GetReportedHardwareInterfaceResponse::create);

    // Set reader ID command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x33, vanch::SetReaderIDCommand::getName(), vanch::SetReaderIDCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x33, vanch::SetReaderIDResponse::getName(), vanch::SetReaderIDResponse::create);

    // Get reader ID command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x34, vanch::GetReaderIDCommand::getName(), vanch::GetReaderIDCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x34, vanch::GetReaderIDResponse::getName(), vanch::GetReaderIDResponse::create);

    // Set reader name command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x35, vanch::SetReaderNameCommand::getName(), vanch::SetReaderNameCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x35, vanch::SetReaderNameResponse::getName(), vanch::SetReaderNameResponse::create);

    // Get reader name command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x36, vanch::GetReaderNameCommand::getName(), vanch::GetReaderNameCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x36, vanch::GetReaderNameResponse::getName(), vanch::GetReaderNameResponse::create);

    // Set relay automatic control command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x09, vanch::SetRelayAutomaticControlCommand::getName(), vanch::SetRelayAutomaticControlCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x09, vanch::SetRelayAutomaticControlResponse::getName(), vanch::SetRelayAutomaticControlResponse::create);

    // Get relay automatic control command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x0A, vanch::GetRelayAutomaticControlCommand::getName(), vanch::GetRelayAutomaticControlCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x0A, vanch::GetRelayAutomaticControlResponse::getName(), vanch::GetRelayAutomaticControlResponse::create);

    // Set automatic polling antenna command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x15, vanch::SetAutomaticPollingAntennaCommand::getName(), vanch::SetAutomaticPollingAntennaCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x15, vanch::SetAutomaticPollingAntennaResponse::getName(), vanch::SetAutomaticPollingAntennaResponse::create);

    // Get automatic polling antenna command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x16, vanch::GetAutomaticPollingAntennaCommand::getName(), vanch::GetAutomaticPollingAntennaCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x16, vanch::GetAutomaticPollingAntennaResponse::getName(), vanch::GetAutomaticPollingAntennaResponse::create);

    // Set automatic reading tag type command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x21, vanch::SetAutomaticReadingTagTypeCommand::getName(), vanch::SetAutomaticReadingTagTypeCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x21, vanch::SetAutomaticReadingTagTypeResponse::getName(), vanch::SetAutomaticReadingTagTypeResponse::create);

    // Get automatic reading tag type command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x22, vanch::GetAutomaticReadingTagTypeCommand::getName(), vanch::GetAutomaticReadingTagTypeCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x22, vanch::GetAutomaticReadingTagTypeResponse::getName(), vanch::GetAutomaticReadingTagTypeResponse::create);

    // Set automatic reporting fields command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x2B, vanch::SetAutomaticReportingFieldsCommand::getName(), vanch::SetAutomaticReportingFieldsCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x2B, vanch::SetAutomaticReportingFieldsResponse::getName(), vanch::SetAutomaticReportingFieldsResponse::create);

    // Get automatic reporting fields command and response
    m_registry.registerMessage(vanch::MessageType_Command, 0x2C, vanch::GetAutomaticReportingFieldsCommand::getName(), vanch::GetAutomaticReportingFieldsCommand::create);
    m_registry.registerMessage(vanch::MessageType_Return, 0x2C, vanch::GetAutomaticReportingFieldsResponse::getName(), vanch::GetAutomaticReportingFieldsResponse::create);
  }

private:
  void OnAttach() override;

  void OnDetach() override;

  void OnUiUpdate() override;

  void OnPacketReturn(const std::vector<uint8_t>& data);

  void OnPacketStatus(const std::vector<uint8_t>& data);

  void OnPacketError(const std::string& message, uint8_t code);

  vanch::MessageRegistry m_registry;

  vanch::UdpClient m_client;

  std::shared_ptr<vanch::Message> m_command{};
  std::shared_ptr<vanch::Message> m_received{};
  std::shared_ptr<vanch::Message> m_status{};

  bool m_show_status{false};
};

[[maybe_unused]] inline kr::Application* kr::CreateApp() {
  std::setlocale(LC_CTYPE, "ru_RU.UTF-8");
  std::setlocale(LC_TIME, "ru_RU.UTF-8");
  std::setlocale(LC_COLLATE, "ru_RU.UTF-8");
  const auto app = new kr::Application({"Revanche"});
  app->AttachLayer(std::make_shared<RevancheApp>());
  return app;
}