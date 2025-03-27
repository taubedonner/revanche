#pragma once
#include <imgui.h>
#include <nlohmann/json.hpp>

namespace vanch {

enum MessageType : uint8_t {
  MessageType_Unknown = 0x00,
  MessageType_Command = 0x40,
  MessageType_Return = 0xF0,
  MessageType_Status = 0xF1,
  MessageType_Error = 0xF4,
};

using CmdCode = uint8_t;

class Message {
public:
  virtual ~Message() = default;

  MessageType m_header{MessageType_Unknown};
  CmdCode m_cmdCode;

  Message(const MessageType header, const CmdCode cmdCode) : m_header(header), m_cmdCode(cmdCode) {
  }

  virtual std::string getMessageName() = 0;

  virtual void parseParameters(std::vector<uint8_t> params) {
  }

  virtual void render() {
    ImGui::TextUnformatted("No parameters to display");
  }

  [[nodiscard]] std::vector<uint8_t> serialize(const uint8_t address = 0x00) const {
    std::vector<uint8_t> packet;

    packet.push_back(m_header); // 0: Header

    packet.push_back(0); // 1: Length H
    packet.push_back(0); // 2: Length L

    packet.push_back(address); // 3: Address

    packet.push_back(m_cmdCode); // 4: Cmd

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
  [[nodiscard]] virtual std::vector<uint8_t> serializeParameters() const { return {}; }

  static uint8_t calculateChecksum(const std::vector<uint8_t>& data) {
    const uint8_t sum = std::accumulate(data.begin(), data.end(), 0);
    return (~sum) + 1;
  }
};

/* Error Response */

struct ErrorResponse final : Message {
  std::string description;

  ErrorResponse(const std::string_view description, const CmdCode cmdCode) : Message(MessageType_Error, cmdCode), description(description) {
  }

  static std::shared_ptr<Message> create(const std::string_view description, const CmdCode cmdCode) {
    return std::make_shared<ErrorResponse>(description, cmdCode);
  }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>&) {
    return std::make_shared<ErrorResponse>("fix-api", 0xFF);
  }

  void render() override {
    ImGui::Text("Error [%2x]: %s", m_cmdCode, description.c_str());
  }

  std::string getMessageName() override { return {}; };
};

/* 01H Automatic card reading data packet */

class AutomaticCardReadingDataStatus final : public Message {
public:
  struct Parameters {
    int Ant{0}; // Номер антенны
    std::vector<int> FIN; // Состояние триггера
    std::string Door; // Направление двери
    std::string IP; // IP-адрес устройства
    std::string EPC; // EPC или UID метки
    std::string TID; // TID метки
    std::string USER; // Данные области USER метки
    std::string ID; // ID устройства
    int RSSI{0}; // Уровень сигнала метки
    uint64_t TS{0}; // Временная метка
    int TagType{0}; // Тип метки
    std::vector<std::string> Custom; // Пользовательские поля
    float Temp{0.0f}; // Температура метки
  };

private:
  Parameters m_parameters;

public:
  AutomaticCardReadingDataStatus() : Message(MessageType_Status, 0x01) {
  }

  static constexpr std::string getName() { return "Automatic Card Reading Data Packet"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<AutomaticCardReadingDataStatus>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void parseParameters(std::vector<uint8_t> params) override {
    using json = nlohmann::json;

    try {
      std::string jsonString(params.begin(), params.end());
      json jsonData = json::parse(jsonString);

      m_parameters.Ant = jsonData.value("Ant", 0);
      m_parameters.FIN = jsonData.value("FIN", std::vector<int>{});
      m_parameters.Door = jsonData.value("Door", "");
      m_parameters.IP = jsonData.value("IP", "");
      m_parameters.EPC = jsonData.value("EPC", "");
      m_parameters.TID = jsonData.value("TID", "");
      m_parameters.USER = jsonData.value("USER", "");
      m_parameters.ID = jsonData.value("ID", "");
      m_parameters.RSSI = jsonData.value("RSSI", 0);
      m_parameters.TS = jsonData.value("TS", 0ULL);
      m_parameters.TagType = jsonData.value("TagType", 0);

      for (int i = 1; i <= 5; ++i) {
        if (std::string key = "Custom" + std::to_string(i); jsonData.contains(key)) {
          m_parameters.Custom.push_back(jsonData[key].get<std::string>());
        }
      }

      m_parameters.Temp = jsonData.value("Temp", 0.0f);
    } catch (const json::exception&) {
    }
  }

  void render() override {
    if (ImGui::BeginTable("##message_table", 2, ImGuiTableFlags_Borders)) {
      ImGui::TableSetupColumn("Field");
      ImGui::TableSetupColumn("Value");
      ImGui::TableHeadersRow();

      addTableRow("Antenna", std::to_string(m_parameters.Ant));
      addTableRow("Trigger State", vectorToString(m_parameters.FIN));
      addTableRow("Door Direction", m_parameters.Door);
      addTableRow("IP Address", m_parameters.IP);
      addTableRow("EPC/UID", m_parameters.EPC);
      addTableRow("TID", m_parameters.TID);
      addTableRow("USER Data", m_parameters.USER);
      addTableRow("Device ID", m_parameters.ID);
      addTableRow("RSSI", std::to_string(m_parameters.RSSI));
      addTableRow("Timestamp", std::to_string(m_parameters.TS));
      addTableRow("Tag Type", std::to_string(m_parameters.TagType));
      addTableRow("Custom Fields", vectorToString(m_parameters.Custom));
      addTableRow("Temperature", std::to_string(m_parameters.Temp));

      ImGui::EndTable();
    }
  }

private:
  static void addTableRow(const std::string& field, const std::string& value) {
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("%s", field.c_str());
    ImGui::TableSetColumnIndex(1);
    ImGui::Text("%s", value.c_str());
  }

  template <typename T>
  static std::string vectorToString(const std::vector<T>& vec) {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
      oss << vec[i];
      if (i != vec.size() - 1) {
        oss << ", ";
      }
    }
    oss << "]";
    return oss.str();
  }
};

/* 01H Set baud rate */

class SetBaudRateCommand final : public Message {
private:
  uint8_t m_interfaceType{0}; // 00H--RS232, 01H--RS485
  uint8_t m_baudRateCode{0}; // Код скорости передачи данных

public:
  SetBaudRateCommand()
    : Message(MessageType_Command, 0x01) {
  }

  static constexpr std::string getName() { return "Set Baud Rate"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetBaudRateCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void render() override {
    ImGui::InputScalar("Interface Type (0=RS232, 1=RS485)", ImGuiDataType_U8, &m_interfaceType);
    ImGui::InputScalar("Baud Rate Code", ImGuiDataType_U8, &m_baudRateCode);
    ImGui::Text("Baud Rate Codes: 0=9600, 1=19200, 2=38400, 3=57600, 4=115200");
  }

  [[nodiscard]] std::vector<uint8_t> serializeParameters() const override {
    return {m_interfaceType, m_baudRateCode};
  }
};

class SetBaudRateResponse final : public Message {
public:
  SetBaudRateResponse()
    : Message(MessageType_Return, 0x01) {
  }

  static constexpr std::string getName() { return "Set Baud Rate Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetBaudRateResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

/* 02H Get baud rate */

class GetBaudRateCommand final : public Message {
private:
  uint8_t m_interfaceType{0}; // 00H--RS232, 01H--RS485

public:
  GetBaudRateCommand()
    : Message(MessageType_Command, 0x02) {
  }

  static constexpr std::string getName() { return "Get Baud Rate"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetBaudRateCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void render() override {
    ImGui::InputScalar("Interface Type (0=RS232, 1=RS485)", ImGuiDataType_U8, &m_interfaceType);
  }

  [[nodiscard]] std::vector<uint8_t> serializeParameters() const override {
    return {m_interfaceType};
  }
};

class GetBaudRateResponse : public Message {
private:
  uint8_t m_baudRateCode{0}; // Код скорости передачи данных

public:
  GetBaudRateResponse()
    : Message(MessageType_Return, 0x02) {
  }

  static constexpr std::string getName() { return "Get Baud Rate Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetBaudRateResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void parseParameters(std::vector<uint8_t> params) override {
    if (!params.empty()) {
      m_baudRateCode = params[0];
    }
  }

  void render() override {
    ImGui::Text("Baud Rate Code: %u", m_baudRateCode);
    ImGui::Text("Baud Rate: %u bps", getBaudRate(m_baudRateCode));
  }

private:
  [[nodiscard]] static uint32_t getBaudRate(const uint8_t code) {
    static const uint32_t baudRates[] = {9600, 19200, 38400, 57600, 115200};
    return (code < 5) ? baudRates[code] : 0;
  }
};

/* 03H Set RS485 address */

class SetRS485AddressCommand final : public Message {
private:
  uint8_t m_address{0}; // Адрес RS485 (1-254)

public:
  SetRS485AddressCommand()
    : Message(MessageType_Command, 0x03) {
  }

  static constexpr std::string getName() { return "Set RS485 Address"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetRS485AddressCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void render() override {
    ImGui::InputScalar("RS485 Address (1-254)", ImGuiDataType_U8, &m_address);
  }

  [[nodiscard]] std::vector<uint8_t> serializeParameters() const override {
    return {m_address};
  }
};

class SetRS485AddressResponse : public Message {
public:
  SetRS485AddressResponse()
    : Message(MessageType_Return, 0x03) {
  }

  static constexpr std::string getName() { return "Set RS485 Address Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetRS485AddressResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

/* 04H Get RS485 address */

class GetRS485AddressCommand final : public Message {
public:
  GetRS485AddressCommand()
    : Message(MessageType_Command, 0x04) {
  }

  static constexpr std::string getName() { return "Get RS485 Address"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetRS485AddressCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

class GetRS485AddressResponse final : public Message {
private:
  uint8_t m_address{0}; // Адрес RS485

public:
  GetRS485AddressResponse()
    : Message(MessageType_Return, 0x04) {
  }

  static constexpr std::string getName() { return "Get RS485 Address Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetRS485AddressResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void parseParameters(std::vector<uint8_t> params) override {
    if (!params.empty()) {
      m_address = params[0];
    }
  }

  void render() override {
    ImGui::Text("RS485 Address: %u", m_address);
  }
};

/* 05H Get version number */

class GetVersionNumberCommand final : public Message {
public:
  GetVersionNumberCommand()
    : Message(MessageType_Command, 0x05) {
  }

  static constexpr std::string getName() { return "Get Version Number"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetVersionNumberCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

class GetVersionNumberResponse final : public Message {
private:
  std::string m_versionInfo;

public:
  GetVersionNumberResponse()
    : Message(MessageType_Return, 0x05) {
  }

  static constexpr std::string getName() { return "Get Version Number Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetVersionNumberResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void parseParameters(std::vector<uint8_t> params) override {
    m_versionInfo.assign(params.begin(), params.end());
  }

  void render() override {
    ImGui::Text("Version Info: %s", m_versionInfo.c_str());
  }
};

/* 06H Set relay status */

class SetRelayStatusCommand final : public Message {
private:
  uint8_t m_relayNumber{0}; // Номер реле
  uint8_t m_status{0}; // Статус реле (0 - выкл, 1 - вкл)

public:
  SetRelayStatusCommand()
    : Message(MessageType_Command, 0x06) {
  }

  static constexpr std::string getName() { return "Set Relay Status"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetRelayStatusCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void render() override {
    ImGui::InputScalar("Relay Number", ImGuiDataType_U8, &m_relayNumber);
    const char* statusItems[] = {"Disconnect (0x00)", "Pull-in (0x01)"};

    int temp_value = m_status;
    if (ImGui::Combo("Relay Status", &temp_value, statusItems, IM_ARRAYSIZE(statusItems))) {
      m_status = static_cast<uint8_t>(temp_value);
    }
  }

  [[nodiscard]] std::vector<uint8_t> serializeParameters() const override {
    return {m_relayNumber, m_status};
  }
};

class SetRelayStatusResponse final : public Message {
public:
  SetRelayStatusResponse()
    : Message(MessageType_Return, 0x06) {
  }

  static constexpr std::string getName() { return "Set Relay Status Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetRelayStatusResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

/* 08H Get relay status */

class GetRelayStatusCommand final : public Message {
private:
  uint8_t m_relayNumber{0}; // Номер реле

public:
  GetRelayStatusCommand()
    : Message(MessageType_Command, 0x08) {
  }

  static constexpr std::string getName() { return "Get Relay Status"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetRelayStatusCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void render() override {
    ImGui::InputScalar("Relay Number", ImGuiDataType_U8, &m_relayNumber);
  }

  [[nodiscard]] std::vector<uint8_t> serializeParameters() const override {
    return {m_relayNumber};
  }
};

class GetRelayStatusResponse final : public Message {
private:
  uint8_t m_status{0}; // Статус реле (0 - выкл, 1 - вкл)

public:
  GetRelayStatusResponse()
    : Message(MessageType_Return, 0x08) {
  }

  static constexpr std::string getName() { return "Get Relay Status Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetRelayStatusResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void parseParameters(std::vector<uint8_t> params) override {
    if (!params.empty()) {
      m_status = params[0];
    }
  }

  void render() override {
    const char* statusText = (m_status == 0) ? "Disconnect (0x00)" : "Pull-in (0x01)";
    ImGui::Text("Relay Status: %s", statusText);
  }
};

/* 0BH Set buzzer */

class SetBuzzerCommand final : public Message {
private:
  uint8_t m_switch{0}; // Переключатель (0 - выкл, 1 - вкл)

public:
  SetBuzzerCommand()
    : Message(MessageType_Command, 0x0B) {
  }

  static constexpr std::string getName() { return "Set Buzzer"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetBuzzerCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void render() override {
    const char* switchItems[] = {"Off (0x00)", "On (0x01)"};

    int temp_value = m_switch;
    if (ImGui::Combo("Buzzer Switch", &temp_value, switchItems, IM_ARRAYSIZE(switchItems))) {
      m_switch = static_cast<uint8_t>(temp_value);
    }
  }

  [[nodiscard]] std::vector<uint8_t> serializeParameters() const override {
    return {m_switch};
  }
};

class SetBuzzerResponse final : public Message {
public:
  SetBuzzerResponse()
    : Message(MessageType_Return, 0x0B) {
  }

  static constexpr std::string getName() { return "Set Buzzer Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetBuzzerResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

/* 0CH Get buzzer */

class GetBuzzerCommand final : public Message {
public:
  GetBuzzerCommand()
    : Message(MessageType_Command, 0x0C) {
  }

  static constexpr std::string getName() { return "Get Buzzer"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetBuzzerCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

class GetBuzzerResponse final : public Message {
private:
  uint8_t m_switch{0}; // Переключатель (0 - выкл, 1 - вкл)

public:
  GetBuzzerResponse()
    : Message(MessageType_Return, 0x0C) {
  }

  static constexpr std::string getName() { return "Get Buzzer Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetBuzzerResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void parseParameters(std::vector<uint8_t> params) override {
    if (!params.empty()) {
      m_switch = params[0];
    }
  }

  void render() override {
    const char* switchText = (m_switch == 0) ? "Off (0x00)" : "On (0x01)";
    ImGui::Text("Buzzer Switch: %s", switchText);
  }
};

/* 0DH Set card reading mode */

class SetCardReadingModeCommand final : public Message {
private:
  uint8_t m_mode{0}; // Режим чтения карт

public:
  SetCardReadingModeCommand()
    : Message(MessageType_Command, 0x0D) {
  }

  static constexpr std::string getName() { return "Set Card Reading Mode"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetCardReadingModeCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void render() override {
    const char* modeItems[] = {
        "Command Mode (0x00)",
        "Automatic Mode (Continuous) (0x01)",
        "Automatic Mode (Switch Trigger) (0x02)",
        "Automatic Mode (Two-Way Trigger) (0x03)"
    };

    int temp_value = m_mode;
    if (ImGui::Combo("Card Reading Mode", &temp_value, modeItems, IM_ARRAYSIZE(modeItems))) {
      m_mode = static_cast<uint8_t>(temp_value);
    }
  }

  [[nodiscard]] std::vector<uint8_t> serializeParameters() const override {
    return {m_mode};
  }
};

class SetCardReadingModeResponse final : public Message {
public:
  SetCardReadingModeResponse()
    : Message(MessageType_Return, 0x0D) {
  }

  static constexpr std::string getName() { return "Set Card Reading Mode Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetCardReadingModeResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

/* 0EH Get card reading mode */

class GetCardReadingModeCommand final : public Message {
public:
  GetCardReadingModeCommand()
    : Message(MessageType_Command, 0x0E) {
  }

  static constexpr std::string getName() { return "Get Card Reading Mode"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetCardReadingModeCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

class GetCardReadingModeResponse final : public Message {
private:
  uint8_t m_mode{0}; // Режим чтения карт

public:
  GetCardReadingModeResponse()
    : Message(MessageType_Return, 0x0E) {
  }

  static constexpr std::string getName() { return "Get Card Reading Mode Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetCardReadingModeResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void parseParameters(std::vector<uint8_t> params) override {
    if (!params.empty()) {
      m_mode = params[0];
    }
  }

  void render() override {
    const char* modeItems[] = {
        "Command Mode (0x00)",
        "Automatic Mode (Continuous) (0x01)",
        "Automatic Mode (Switch Trigger) (0x02)",
        "Automatic Mode (Two-Way Trigger) (0x03)"
    };
    int temp_value = m_mode;
    if (ImGui::Combo("Card Reading Mode", &temp_value, modeItems, IM_ARRAYSIZE(modeItems))) {
      m_mode = static_cast<uint8_t>(temp_value);
    }
  }
};

/* 0FH Set output power */

class SetOutputPowerCommand final : public Message {
private:
  uint8_t m_antennaNumber{0}; // Номер антенны
  uint8_t m_powerValue{0}; // Значение мощности

public:
  SetOutputPowerCommand()
    : Message(MessageType_Command, 0x0F) {
  }

  static constexpr std::string getName() { return "Set Output Power"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetOutputPowerCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void render() override {
    ImGui::InputScalar("Antenna Number", ImGuiDataType_U8, &m_antennaNumber);
    ImGui::InputScalar("Power Value (0-33 dBm)", ImGuiDataType_U8, &m_powerValue);
  }

  [[nodiscard]] std::vector<uint8_t> serializeParameters() const override {
    return {m_antennaNumber, m_powerValue};
  }
};

class SetOutputPowerResponse final : public Message {
public:
  SetOutputPowerResponse()
    : Message(MessageType_Return, 0x0F) {
  }

  static constexpr std::string getName() { return "Set Output Power Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetOutputPowerResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

/* 10H Get output power */

class GetOutputPowerCommand final : public Message {
private:
  uint8_t m_antennaNumber{0}; // Номер антенны

public:
  GetOutputPowerCommand()
    : Message(MessageType_Command, 0x10) {
  }

  static constexpr std::string getName() { return "Get Output Power"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetOutputPowerCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void render() override {
    ImGui::InputScalar("Antenna Number", ImGuiDataType_U8, &m_antennaNumber);
  }

  [[nodiscard]] std::vector<uint8_t> serializeParameters() const override {
    return {m_antennaNumber};
  }
};

class GetOutputPowerResponse final : public Message {
private:
  uint8_t m_powerValue{0}; // Значение мощности

public:
  GetOutputPowerResponse()
    : Message(MessageType_Return, 0x10) {
  }

  static constexpr std::string getName() { return "Get Output Power Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetOutputPowerResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void parseParameters(std::vector<uint8_t> params) override {
    if (!params.empty()) {
      m_powerValue = params[0];
    }
  }

  void render() override {
    ImGui::Text("Output Power: %u dBm", m_powerValue);
  }
};

/* 13H Set frequency band frequency point */

class SetFrequencyBandCommand final : public Message {
private:
  uint8_t m_region{0}; // Регион
  uint8_t m_startFrequency{0}; // Начальная частота
  uint8_t m_endFrequency{0}; // Конечная частота

public:
  SetFrequencyBandCommand()
    : Message(MessageType_Command, 0x13) {
  }

  static constexpr std::string getName() { return "Set Frequency Band"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetFrequencyBandCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void render() override {
    const char* regionItems[] = {"FCC (0x01)", "ETSI (0x02)", "CHN (0x03)"};
    int temp_region = m_region;
    if (ImGui::Combo("Region", &temp_region, regionItems, IM_ARRAYSIZE(regionItems))) {
      m_region = static_cast<uint8_t>(temp_region);
    }

    ImGui::InputScalar("Start Frequency", ImGuiDataType_U8, &m_startFrequency);
    ImGui::InputScalar("End Frequency", ImGuiDataType_U8, &m_endFrequency);
  }

  [[nodiscard]] std::vector<uint8_t> serializeParameters() const override {
    return {m_region, m_startFrequency, m_endFrequency};
  }
};

class SetFrequencyBandResponse final : public Message {
public:
  SetFrequencyBandResponse()
    : Message(MessageType_Return, 0x13) {
  }

  static constexpr std::string getName() { return "Set Frequency Band Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetFrequencyBandResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

/* 14H Get frequency band frequency point */

class GetFrequencyBandCommand final : public Message {
public:
  GetFrequencyBandCommand()
    : Message(MessageType_Command, 0x14) {
  }

  static constexpr std::string getName() { return "Get Frequency Band"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetFrequencyBandCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

class GetFrequencyBandResponse final : public Message {
private:
  uint8_t m_region{0}; // Регион
  uint8_t m_startFrequency{0}; // Начальная частота
  uint8_t m_endFrequency{0}; // Конечная частота

public:
  GetFrequencyBandResponse()
    : Message(MessageType_Return, 0x14) {
  }

  static constexpr std::string getName() { return "Get Frequency Band Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetFrequencyBandResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void parseParameters(std::vector<uint8_t> params) override {
    if (params.size() >= 3) {
      m_region = params[0];
      m_startFrequency = params[1];
      m_endFrequency = params[2];
    }
  }

  void render() override {
    std::string regionText;
    switch (m_region) {
      case 1:
        regionText = "FCC";
        break;
      case 2:
        regionText = "ETSI";
        break;
      case 3:
        regionText = "CHN";
        break;
      default:
        regionText = "Unknown";
        break;
    }

    ImGui::Text("Region: %s (%#02X)", regionText.c_str(), m_region);
    ImGui::Text("Start Frequency: %u", m_startFrequency);
    ImGui::Text("End Frequency: %u", m_endFrequency);
  }
};

/* 17H Restore reader/writer to factory settings */

class RestoreFactorySettingsCommand final : public Message {
public:
  RestoreFactorySettingsCommand()
    : Message(MessageType_Command, 0x17) {
  }

  static constexpr std::string getName() { return "Restore Factory Settings"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<RestoreFactorySettingsCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

class RestoreFactorySettingsResponse final : public Message {
public:
  RestoreFactorySettingsResponse()
    : Message(MessageType_Return, 0x17) {
  }

  static constexpr std::string getName() { return "Restore Factory Settings Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<RestoreFactorySettingsResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

/* 18H Restart reader/writer */

class RestartReaderCommand final : public Message {
public:
  RestartReaderCommand()
    : Message(MessageType_Command, 0x18) {
  }

  static constexpr std::string getName() { return "Restart Reader"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<RestartReaderCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

class RestartReaderResponse final : public Message {
public:
  RestartReaderResponse()
    : Message(MessageType_Return, 0x18) {
  }

  static constexpr std::string getName() { return "Restart Reader Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<RestartReaderResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

/* 19H Restore WIFI factory settings */

class RestoreWIFICommand final : public Message {
public:
  RestoreWIFICommand()
    : Message(MessageType_Command, 0x19) {
  }

  static constexpr std::string getName() { return "Restore WIFI Factory Settings"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<RestoreWIFICommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

class RestoreWIFIResponse final : public Message {
public:
  RestoreWIFIResponse()
    : Message(MessageType_Return, 0x19) {
  }

  static constexpr std::string getName() { return "Restore WIFI Factory Settings Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<RestoreWIFIResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

/* 1BH Set reader time */

class SetReaderTimeCommand final : public Message {
private:
  uint8_t m_year{0}; // Год
  uint8_t m_month{0}; // Месяц
  uint8_t m_day{0}; // День
  uint8_t m_hour{0}; // Час
  uint8_t m_minute{0}; // Минута
  uint8_t m_second{0}; // Секунда

public:
  SetReaderTimeCommand()
    : Message(MessageType_Command, 0x1B) {
  }

  static constexpr std::string getName() { return "Set Reader Time"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetReaderTimeCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void render() override {
    ImGui::InputScalar("Year", ImGuiDataType_U8, &m_year);
    ImGui::InputScalar("Month", ImGuiDataType_U8, &m_month);
    ImGui::InputScalar("Day", ImGuiDataType_U8, &m_day);
    ImGui::InputScalar("Hour", ImGuiDataType_U8, &m_hour);
    ImGui::InputScalar("Minute", ImGuiDataType_U8, &m_minute);
    ImGui::InputScalar("Second", ImGuiDataType_U8, &m_second);
  }

  [[nodiscard]] std::vector<uint8_t> serializeParameters() const override {
    return {m_year, m_month, m_day, m_hour, m_minute, m_second};
  }
};

class SetReaderTimeResponse final : public Message {
public:
  SetReaderTimeResponse()
    : Message(MessageType_Return, 0x1B) {
  }

  static constexpr std::string getName() { return "Set Reader Time Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetReaderTimeResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

/* 1CH Get reader time */

class GetReaderTimeCommand final : public Message {
public:
  GetReaderTimeCommand()
    : Message(MessageType_Command, 0x1C) {
  }

  static constexpr std::string getName() { return "Get Reader Time"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetReaderTimeCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

class GetReaderTimeResponse final : public Message {
private:
  uint8_t m_year{0}; // Год
  uint8_t m_month{0}; // Месяц
  uint8_t m_day{0}; // День
  uint8_t m_hour{0}; // Час
  uint8_t m_minute{0}; // Минута
  uint8_t m_second{0}; // Секунда

public:
  GetReaderTimeResponse()
    : Message(MessageType_Return, 0x1C) {
  }

  static constexpr std::string getName() { return "Get Reader Time Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetReaderTimeResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void parseParameters(std::vector<uint8_t> params) override {
    if (params.size() >= 6) {
      m_year = params[0];
      m_month = params[1];
      m_day = params[2];
      m_hour = params[3];
      m_minute = params[4];
      m_second = params[5];
    }
  }

  void render() override {
    ImGui::Text("Time: %04d-%02d-%02d %02d:%02d:%02d",
                m_year, m_month, m_day, m_hour, m_minute, m_second);
  }
};

/* 1FH Set tag filter */

class SetTagFilterCommand final : public Message {
private:
  uint8_t m_enable{0}; // Включение фильтра (0 - выкл, 1 - вкл)
  uint8_t m_maskAddress{0}; // Адрес маски
  uint8_t m_maskLength{0}; // Длина маски
  std::vector<uint8_t> m_maskData; // Данные маски

public:
  SetTagFilterCommand()
    : Message(MessageType_Command, 0x1F) {
  }

  static constexpr std::string getName() { return "Set Tag Filter"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetTagFilterCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void render() override {
    const char* enableItems[] = {"Off (0x00)", "On (0x01)"};
    int temp_enable = m_enable;
    if (ImGui::Combo("Enable Filter", &temp_enable, enableItems, IM_ARRAYSIZE(enableItems))) {
      m_enable = static_cast<uint8_t>(temp_enable);
    }

    ImGui::InputScalar("Mask Address", ImGuiDataType_U8, &m_maskAddress);
    ImGui::InputScalar("Mask Length", ImGuiDataType_U8, &m_maskLength);

    static char inputBuffer[64] = "";
    if (ImGui::InputText("Mask Data (comma-separated)", inputBuffer, sizeof(inputBuffer))) {
      m_maskData.clear();
      std::string input(inputBuffer);
      std::stringstream ss(input);
      std::string item;
      while (std::getline(ss, item, ',')) {
        try {
          auto byte = static_cast<uint8_t>(std::stoi(item));
          m_maskData.push_back(byte);
        } catch (...) {
          // Игнорируем некорректные значения
        }
      }
    }
  }

  [[nodiscard]] std::vector<uint8_t> serializeParameters() const override {
    std::vector<uint8_t> params = {m_enable, m_maskAddress, m_maskLength};
    params.insert(params.end(), m_maskData.begin(), m_maskData.end());
    return params;
  }
};

class SetTagFilterResponse final : public Message {
public:
  SetTagFilterResponse()
    : Message(MessageType_Return, 0x1F) {
  }

  static constexpr std::string getName() { return "Set Tag Filter Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetTagFilterResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

/* 20H Get tag filter */

class GetTagFilterCommand final : public Message {
public:
  GetTagFilterCommand()
    : Message(MessageType_Command, 0x20) {
  }

  static constexpr std::string getName() { return "Get Tag Filter"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetTagFilterCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

class GetTagFilterResponse final : public Message {
private:
  uint8_t m_enable{0}; // Включение фильтра (0 - выкл, 1 - вкл)
  uint8_t m_maskAddress{0}; // Адрес маски
  uint8_t m_maskLength{0}; // Длина маски
  std::vector<uint8_t> m_maskData; // Данные маски

public:
  GetTagFilterResponse()
    : Message(MessageType_Return, 0x20) {
  }

  static constexpr std::string getName() { return "Get Tag Filter Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetTagFilterResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void parseParameters(std::vector<uint8_t> params) override {
    if (params.size() >= 3) {
      m_enable = params[0];
      m_maskAddress = params[1];
      m_maskLength = params[2];
      m_maskData.assign(params.begin() + 3, params.end());
    }
  }

  void render() override {
    const char* enableText = m_enable ? "On (0x01)" : "Off (0x00)";
    ImGui::Text("Enable Filter: %s", enableText);
    ImGui::Text("Mask Address: %u", m_maskAddress);
    ImGui::Text("Mask Length: %u", m_maskLength);

    std::string maskDataStr;
    for (uint8_t byte : m_maskData) {
      maskDataStr += std::to_string(byte) + ", ";
    }
    if (!maskDataStr.empty()) {
      maskDataStr.pop_back(); // Удаляем последнюю запятую
      maskDataStr.pop_back();
    }
    ImGui::Text("Mask Data: %s", maskDataStr.c_str());
  }
};

/* 29H Set the reported hardware interface */

class SetReportedHardwareInterfaceCommand final : public Message {
private:
  uint8_t m_interface{0}; // Интерфейс
  uint8_t m_enable{0}; // Включение (0 - выкл, 1 - вкл)

public:
  SetReportedHardwareInterfaceCommand()
    : Message(MessageType_Command, 0x29) {
  }

  static constexpr std::string getName() { return "Set Reported Hardware Interface"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetReportedHardwareInterfaceCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void render() override {
    const char* interfaceItems[] = {
        "RS232 (0x00)", "RS485 (0x01)", "RJ45 (0x02)",
        "Wiegand 26 (0x03)", "Wiegand 34 (0x04)",
        "WIFI (0x05)", "4G (0x06)"
    };
    int temp_interface = m_interface;
    if (ImGui::Combo("Interface", &temp_interface, interfaceItems, IM_ARRAYSIZE(interfaceItems))) {
      m_interface = static_cast<uint8_t>(temp_interface);
    }

    const char* enableItems[] = {"Off (0x00)", "On (0x01)"};
    int temp_enable = m_enable;
    if (ImGui::Combo("Enable Reporting", &temp_enable, enableItems, IM_ARRAYSIZE(enableItems))) {
      m_enable = static_cast<uint8_t>(temp_enable);
    }
  }

  [[nodiscard]] std::vector<uint8_t> serializeParameters() const override {
    return {m_interface, m_enable};
  }
};

class SetReportedHardwareInterfaceResponse final : public Message {
public:
  SetReportedHardwareInterfaceResponse()
    : Message(MessageType_Return, 0x29) {
  }

  static constexpr std::string getName() { return "Set Reported Hardware Interface Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetReportedHardwareInterfaceResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

/* 2AH Gets the reported hardware interface */

class GetReportedHardwareInterfaceCommand final : public Message {
private:
  uint8_t m_interface{0}; // Интерфейс

public:
  GetReportedHardwareInterfaceCommand()
    : Message(MessageType_Command, 0x2A) {
  }

  static constexpr std::string getName() { return "Get Reported Hardware Interface"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetReportedHardwareInterfaceCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void render() override {
    const char* interfaceItems[] = {
        "RS232 (0x00)", "RS485 (0x01)", "RJ45 (0x02)",
        "Wiegand 26 (0x03)", "Wiegand 34 (0x04)",
        "WIFI (0x05)", "4G (0x06)"
    };
    int temp_interface = m_interface;
    if (ImGui::Combo("Interface", &temp_interface, interfaceItems, IM_ARRAYSIZE(interfaceItems))) {
      m_interface = static_cast<uint8_t>(temp_interface);
    }
  }

  [[nodiscard]] std::vector<uint8_t> serializeParameters() const override {
    return {m_interface};
  }
};

class GetReportedHardwareInterfaceResponse final : public Message {
private:
  uint8_t m_enabled{0}; // Включение (0 - выкл, 1 - вкл)

public:
  GetReportedHardwareInterfaceResponse()
    : Message(MessageType_Return, 0x2A) {
  }

  static constexpr std::string getName() { return "Get Reported Hardware Interface Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetReportedHardwareInterfaceResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void parseParameters(std::vector<uint8_t> params) override {
    if (!params.empty()) {
      m_enabled = params[0];
    }
  }

  void render() override {
    const char* enabledText = m_enabled ? "Enabled (0x01)" : "Disabled (0x00)";
    ImGui::Text("Reporting Enabled: %s", enabledText);
  }
};

/* 33H Set reader ID */

class SetReaderIDCommand final : public Message {
private:
  std::string m_id;

public:
  SetReaderIDCommand()
    : Message(MessageType_Command, 0x33) {
  }

  static constexpr std::string getName() { return "Set Reader ID"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetReaderIDCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void render() override {
    static char inputBuffer[25] = "";
    if (ImGui::InputText("Reader ID (ASCII, max 24 chars)", inputBuffer, sizeof(inputBuffer))) {
      m_id = inputBuffer;
    }
  }

  [[nodiscard]] std::vector<uint8_t> serializeParameters() const override {
    std::vector<uint8_t> params(m_id.begin(), m_id.end());
    return params;
  }
};

class SetReaderIDResponse final : public Message {
public:
  SetReaderIDResponse()
    : Message(MessageType_Return, 0x33) {
  }

  static constexpr std::string getName() { return "Set Reader ID Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetReaderIDResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

/* 34H Get reader ID */

class GetReaderIDCommand final : public Message {
public:
  GetReaderIDCommand()
    : Message(MessageType_Command, 0x34) {
  }

  static constexpr std::string getName() { return "Get Reader ID"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetReaderIDCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

class GetReaderIDResponse final : public Message {
private:
  std::string m_id;

public:
  GetReaderIDResponse()
    : Message(MessageType_Return, 0x34) {
  }

  static constexpr std::string getName() { return "Get Reader ID Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetReaderIDResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void parseParameters(std::vector<uint8_t> params) override {
    m_id.assign(params.begin(), params.end());
  }

  void render() override {
    ImGui::Text("Reader ID: %s", m_id.c_str());
  }
};

/* 35H Set reader name */

class SetReaderNameCommand final : public Message {
private:
  std::string m_name;

public:
  SetReaderNameCommand()
    : Message(MessageType_Command, 0x35) {
  }

  static constexpr std::string getName() { return "Set Reader Name"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetReaderNameCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void render() override {
    static char inputBuffer[25] = "";
    if (ImGui::InputText("Reader Name (ASCII, max 24 chars)", inputBuffer, sizeof(inputBuffer))) {
      m_name = inputBuffer;
    }
  }

  [[nodiscard]] std::vector<uint8_t> serializeParameters() const override {
    std::vector<uint8_t> params(m_name.begin(), m_name.end());
    return params;
  }
};

class SetReaderNameResponse final : public Message {
public:
  SetReaderNameResponse()
    : Message(MessageType_Return, 0x35) {
  }

  static constexpr std::string getName() { return "Set Reader Name Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetReaderNameResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

/* 36H Get reader name */

class GetReaderNameCommand final : public Message {
public:
  GetReaderNameCommand()
    : Message(MessageType_Command, 0x36) {
  }

  static constexpr std::string getName() { return "Get Reader Name"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetReaderNameCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

class GetReaderNameResponse final : public Message {
private:
  std::string m_name;

public:
  GetReaderNameResponse()
    : Message(MessageType_Return, 0x36) {
  }

  static constexpr std::string getName() { return "Get Reader Name Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetReaderNameResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void parseParameters(std::vector<uint8_t> params) override {
    m_name.assign(params.begin(), params.end());
  }

  void render() override {
    ImGui::Text("Reader Name: %s", m_name.c_str());
  }
};

/* 09H Set relay automatic control parameters */

class SetRelayAutomaticControlCommand final : public Message {
private:
  uint8_t m_relayNumber{0}; // Номер реле
  uint8_t m_purpose{0}; // Цель использования реле
  uint8_t m_pickupTime{0}; // Время срабатывания реле (в секундах)

public:
  SetRelayAutomaticControlCommand()
    : Message(MessageType_Command, 0x09) {
  }

  static constexpr std::string getName() { return "Set Relay Automatic Control Parameters"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetRelayAutomaticControlCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void render() override {
    ImGui::InputScalar("Relay Number", ImGuiDataType_U8, &m_relayNumber);

    const char* purposeItems[] = {
        "No Automatic Control (0x00)",
        "Activate on Tag Read (0x01)",
        "Activate on Alarm Condition (0x02)"
    };
    int temp_purpose = m_purpose;
    if (ImGui::Combo("Purpose of Relay", &temp_purpose, purposeItems, IM_ARRAYSIZE(purposeItems))) {
      m_purpose = static_cast<uint8_t>(temp_purpose);
    }

    ImGui::InputScalar("Pickup Time (seconds)", ImGuiDataType_U8, &m_pickupTime);
  }

  [[nodiscard]] std::vector<uint8_t> serializeParameters() const override {
    return {m_relayNumber, m_purpose, m_pickupTime};
  }
};

class SetRelayAutomaticControlResponse final : public Message {
public:
  SetRelayAutomaticControlResponse()
    : Message(MessageType_Return, 0x09) {
  }

  static constexpr std::string getName() { return "Set Relay Automatic Control Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetRelayAutomaticControlResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

/* 0AH Get relay automatic control parameters */

class GetRelayAutomaticControlCommand final : public Message {
private:
  uint8_t m_relayNumber{0}; // Номер реле

public:
  GetRelayAutomaticControlCommand()
    : Message(MessageType_Command, 0x0A) {
  }

  static constexpr std::string getName() { return "Get Relay Automatic Control Parameters"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetRelayAutomaticControlCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void render() override {
    ImGui::InputScalar("Relay Number", ImGuiDataType_U8, &m_relayNumber);
  }

  [[nodiscard]] std::vector<uint8_t> serializeParameters() const override {
    return {m_relayNumber};
  }
};

class GetRelayAutomaticControlResponse final : public Message {
private:
  uint8_t m_purpose{0}; // Цель использования реле
  uint8_t m_pickupTime{0}; // Время срабатывания реле (в секундах)

public:
  GetRelayAutomaticControlResponse()
    : Message(MessageType_Return, 0x0A) {
  }

  static constexpr std::string getName() { return "Get Relay Automatic Control Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetRelayAutomaticControlResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void parseParameters(std::vector<uint8_t> params) override {
    if (params.size() >= 2) {
      m_purpose = params[0];
      m_pickupTime = params[1];
    }
  }

  void render() override {
    std::string purposeText;
    switch (m_purpose) {
      case 0:
        purposeText = "No Automatic Control (0x00)";
        break;
      case 1:
        purposeText = "Activate on Tag Read (0x01)";
        break;
      case 2:
        purposeText = "Activate on Alarm Condition (0x02)";
        break;
      default:
        purposeText = "Unknown";
        break;
    }

    ImGui::Text("Purpose of Relay: %s", purposeText.c_str());
    ImGui::Text("Pickup Time: %u seconds", m_pickupTime);
  }
};

/* 15H Set automatic polling antenna */

class SetAutomaticPollingAntennaCommand final : public Message {
private:
  std::vector<uint8_t> m_antennas; // Номера антенн

public:
  SetAutomaticPollingAntennaCommand()
    : Message(MessageType_Command, 0x15) {
  }

  static constexpr std::string getName() { return "Set Automatic Polling Antenna"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetAutomaticPollingAntennaCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void render() override {
    static char inputBuffer[64] = "";
    if (ImGui::InputText("Antennas (comma-separated)", inputBuffer, sizeof(inputBuffer))) {
      m_antennas.clear();
      const std::string input(inputBuffer);
      std::stringstream ss(input);
      std::string item;
      while (std::getline(ss, item, ',')) {
        try {
          auto antenna = static_cast<uint8_t>(std::stoi(item));
          m_antennas.push_back(antenna);
        } catch (...) {
          // Игнорируем некорректные значения
        }
      }
    }
  }

  [[nodiscard]] std::vector<uint8_t> serializeParameters() const override {
    return m_antennas;
  }
};

class SetAutomaticPollingAntennaResponse final : public Message {
public:
  SetAutomaticPollingAntennaResponse()
    : Message(MessageType_Return, 0x15) {
  }

  static constexpr std::string getName() { return "Set Automatic Polling Antenna Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetAutomaticPollingAntennaResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

/* 16H Get automatic polling antenna */

class GetAutomaticPollingAntennaCommand final : public Message {
public:
  GetAutomaticPollingAntennaCommand()
    : Message(MessageType_Command, 0x16) {
  }

  static constexpr std::string getName() { return "Get Automatic Polling Antenna"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetAutomaticPollingAntennaCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

class GetAutomaticPollingAntennaResponse final : public Message {
private:
  std::vector<uint8_t> m_antennas; // Номера антенн

public:
  GetAutomaticPollingAntennaResponse()
    : Message(MessageType_Return, 0x16) {
  }

  static constexpr std::string getName() { return "Get Automatic Polling Antenna Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetAutomaticPollingAntennaResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void parseParameters(std::vector<uint8_t> params) override {
    m_antennas.assign(params.begin(), params.end());
  }

  void render() override {
    std::string antennasStr;
    for (const uint8_t antenna : m_antennas) {
      antennasStr += std::to_string(antenna) + ", ";
    }
    if (!antennasStr.empty()) {
      antennasStr.pop_back(); // Удаляем последнюю запятую
      antennasStr.pop_back();
    }
    ImGui::Text("Antennas: %s", antennasStr.c_str());
  }
};

/* 21H Set the automatic reading tag type */

class SetAutomaticReadingTagTypeCommand final : public Message {
private:
  uint8_t m_tagTypes{0}; // Типы меток (битовая маска)

public:
  SetAutomaticReadingTagTypeCommand()
    : Message(MessageType_Command, 0x21) {
  }

  static constexpr std::string getName() { return "Set Automatic Reading Tag Type"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetAutomaticReadingTagTypeCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void render() override {
    uint32_t temp_value = m_tagTypes;

    bool changed = ImGui::CheckboxFlags("Read ISO18000-6B Tags (Bit 0)", &temp_value, 0b0001);
    changed |= ImGui::CheckboxFlags("Read ISO18000-6C Tags (Bit 1)", &temp_value, 0b0010);
    changed |= ImGui::CheckboxFlags("Read Temperature Tags (Bit 2)", &temp_value, 0b0100);

    if (changed) {
      if (temp_value & 0b0100) {
        temp_value &= ~0b0011;
      } else if ((temp_value & 0b0011) == 0b0011) {
        temp_value &= ~0b0100;
      }
      m_tagTypes = temp_value;
    }

    ImGui::Text("Current Tag Types Bitmap: 0x%02X", m_tagTypes);
  }

  [[nodiscard]] std::vector<uint8_t> serializeParameters() const override {
    return {m_tagTypes};
  }
};

class SetAutomaticReadingTagTypeResponse final : public Message {
public:
  SetAutomaticReadingTagTypeResponse()
    : Message(MessageType_Return, 0x21) {
  }

  static constexpr std::string getName() { return "Set Automatic Reading Tag Type Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetAutomaticReadingTagTypeResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

/* 22H Get the automatically read tag type */

class GetAutomaticReadingTagTypeCommand final : public Message {
public:
  GetAutomaticReadingTagTypeCommand()
    : Message(MessageType_Command, 0x22) {
  }

  static constexpr std::string getName() { return "Get Automatic Reading Tag Type"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetAutomaticReadingTagTypeCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

class GetAutomaticReadingTagTypeResponse final : public Message {
private:
  uint8_t m_tagTypes{0}; // Битовая маска типов меток

public:
  GetAutomaticReadingTagTypeResponse()
    : Message(MessageType_Return, 0x22) {
  }

  static constexpr std::string getName() { return "Get Automatic Reading Tag Type Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetAutomaticReadingTagTypeResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void parseParameters(std::vector<uint8_t> params) override {
    if (!params.empty()) {
      m_tagTypes = params[0] & 0b0111;
    }
  }

  void render() override {
    ImGui::Text("Current Tag Types Bitmap: 0x%02X", m_tagTypes);

    // Отображаем состояние каждого бита
    ImGui::Text("ISO18000-6B Tags (Bit 0): %s", (m_tagTypes & 0b001) ? "Enabled" : "Disabled");
    ImGui::Text("ISO18000-6C Tags (Bit 1): %s", (m_tagTypes & 0b010) ? "Enabled" : "Disabled");
    ImGui::Text("Temperature Tags (Bit 2): %s", (m_tagTypes & 0b100) ? "Enabled" : "Disabled");
  }
};

/* 2BH Set automatic reporting fields */

class SetAutomaticReportingFieldsCommand final : public Message {
public:
  explicit SetAutomaticReportingFieldsCommand(uint32_t fields = 0, uint8_t tidStart = 0, uint8_t tidLength = 0, uint8_t userStart = 0, uint8_t userLength = 0)
    : Message(MessageType_Command, 0x2B), m_fields(fields), m_tidStart(tidStart), m_tidLength(tidLength), m_userStart(userStart), m_userLength(userLength) {
  }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetAutomaticReportingFieldsCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  void render() override {
    // Отображение чекбоксов для выбора полей
    ImGui::Text("Select Fields to Report:");
    ImGui::CheckboxFlags("EPC/UID", &m_fields, 0b00000001); // BIT0
    ImGui::CheckboxFlags("TID", &m_fields, 0b00000010); // BIT1
    ImGui::CheckboxFlags("USER", &m_fields, 0b00000100); // BIT2
    ImGui::CheckboxFlags("RSSI", &m_fields, 0b00001000); // BIT3
    ImGui::CheckboxFlags("Timestamp", &m_fields, 0b00010000); // BIT4
    ImGui::CheckboxFlags("Tag Type", &m_fields, 0b00100000); // BIT5
    ImGui::CheckboxFlags("IP Address", &m_fields, 0b01000000); // BIT6
    ImGui::CheckboxFlags("Antenna Number", &m_fields, 0b10000000); // BIT7

    // Настройка TID области
    if (m_fields & 0b00000010) {
      // Если выбран TID
      int temp_value = m_tidStart;
      if (ImGui::InputInt("TID Start Address", &temp_value)) {
        m_tidStart = temp_value;
      }
      temp_value = m_tidLength;
      if (ImGui::InputInt("TID Length", &temp_value)) {
        m_tidLength = temp_value;
      }
    }

    // Настройка USER области
    if (m_fields & 0b00000100) {
      // Если выбран USER
      int temp_value = m_userStart;
      if (ImGui::InputInt("USER Start Address", &temp_value)) {
        m_userStart = temp_value;
      }
      temp_value = m_userLength;
      if (ImGui::InputInt("USER Length", &temp_value)) {
        m_userLength = temp_value;
      }
    }
  }

  [[nodiscard]] std::vector<uint8_t> serializeParameters() const override {
    std::vector<uint8_t> params;
    params.push_back((m_fields >> 24) & 0xFF); // High byte of fields
    params.push_back((m_fields >> 16) & 0xFF);
    params.push_back((m_fields >> 8) & 0xFF);
    params.push_back(m_fields & 0xFF); // Low byte of fields
    params.push_back(m_tidStart);
    params.push_back(m_tidLength);
    params.push_back(m_userStart);
    params.push_back(m_userLength);
    return params;
  }

  static constexpr std::string getName() { return "Set Automatic Reporting Fields Command"; }

  std::string getMessageName() override { return getName(); }

private:
  uint32_t m_fields; // Битовая маска полей
  uint8_t m_tidStart; // Начальный адрес TID
  uint8_t m_tidLength; // Длина TID
  uint8_t m_userStart; // Начальный адрес USER
  uint8_t m_userLength; // Длина USER
};

class SetAutomaticReportingFieldsResponse final : public Message {
public:
  SetAutomaticReportingFieldsResponse()
    : Message(MessageType_Return, 0x2B) {
  }

  static constexpr std::string getName() { return "Set Automatic Reporting Fields Response"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<SetAutomaticReportingFieldsResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

/* 2CH Get the automatically reported fields */

class GetAutomaticReportingFieldsCommand final : public Message {
public:
  GetAutomaticReportingFieldsCommand()
    : Message(MessageType_Command, 0x2C) {
  }

  static constexpr std::string getName() { return "Get the automatically reported fields"; }
  std::string getMessageName() override { return getName(); }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetAutomaticReportingFieldsCommand>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }
};

class GetAutomaticReportingFieldsResponse final : public Message {
private:
  uint32_t m_fields{0}; // Битовая маска полей
  uint8_t m_tidStart{0}; // Начальный адрес TID
  uint8_t m_tidLength{0}; // Длина TID
  uint8_t m_userStart{0}; // Начальный адрес USER
  uint8_t m_userLength{0}; // Длина USER

public:
  GetAutomaticReportingFieldsResponse() : Message(MessageType_Return, 0x2C) {
  }

  static constexpr std::string getName() { return "Get Automatically Reported Fields Response"; }
  std::string getMessageName() override { return getName(); }

  // Парсинг входных данных
  void parseParameters(std::vector<uint8_t> params) override {
    m_fields = (static_cast<uint32_t>(params[0]) << 24) |
               (static_cast<uint32_t>(params[1]) << 16) |
               (static_cast<uint32_t>(params[2]) << 8) |
               static_cast<uint32_t>(params[3]);

    m_tidStart = params[4];
    m_tidLength = params[5];
    m_userStart = params[6];
    m_userLength = params[7];
  }

  static std::shared_ptr<Message> create(const std::vector<uint8_t>& data) {
    auto msg = std::make_shared<GetAutomaticReportingFieldsResponse>();
    msg->parseParameters({data.begin() + 5, data.end() - 1});
    return msg;
  }

  // Отображение данных через ImGui
  void render() override {
    ImGui::Text("Current Fields Bitmap: 0x%08X", m_fields);

    ImGui::Text("Selected Fields:");
    ImGui::BulletText("EPC/UID: %s", (m_fields & 0b00000001) ? "Enabled" : "Disabled");
    ImGui::BulletText("TID: %s", (m_fields & 0b00000010) ? "Enabled" : "Disabled");
    ImGui::BulletText("USER: %s", (m_fields & 0b00000100) ? "Enabled" : "Disabled");
    ImGui::BulletText("RSSI: %s", (m_fields & 0b00001000) ? "Enabled" : "Disabled");
    ImGui::BulletText("Timestamp: %s", (m_fields & 0b00010000) ? "Enabled" : "Disabled");
    ImGui::BulletText("Tag Type: %s", (m_fields & 0b00100000) ? "Enabled" : "Disabled");
    ImGui::BulletText("IP Address: %s", (m_fields & 0b01000000) ? "Enabled" : "Disabled");
    ImGui::BulletText("Antenna Number: %s", (m_fields & 0b10000000) ? "Enabled" : "Disabled");

    if (m_fields & 0b00000010) {
      // Если выбран TID
      ImGui::Text("TID Start Address: %d", m_tidStart);
      ImGui::Text("TID Length: %d", m_tidLength);
    }

    if (m_fields & 0b00000100) {
      // Если выбран USER
      ImGui::Text("USER Start Address: %d", m_userStart);
      ImGui::Text("USER Length: %d", m_userLength);
    }
  }
};

} // vanch