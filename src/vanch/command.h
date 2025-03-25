#pragma once

#include <utility>

namespace vanch {

enum MessageType : uint8_t {
  MessageType_Command = 0x40,
  MessageType_Reply = 0xF4,
  MessageType_Status = 0xF1,
  MessageType_Dummy = 0x00
};

struct Parameter {
  virtual size_t GetSize() = 0;

  virtual std::string_view GetName() { return ""; }

  virtual std::vector<uint8_t> Serialize() = 0;

  virtual ptrdiff_t Deserialize(std::span<uint8_t> param) = 0;

  virtual ~Parameter() = 0;
};

struct ByteParameter final : Parameter {
  uint8_t Value{};
  std::string Name;

  explicit ByteParameter(std::string name) : Name(std::move(name)) {}

  std::string_view GetName() override { return Name; }

  size_t GetSize() override { return sizeof(Value); }

  std::vector<uint8_t> Serialize() override { return {Value}; }

  ptrdiff_t Deserialize(const std::span<uint8_t> param) override {
    Value = param[0];
    return sizeof(Value);
  }

  ~ByteParameter() {};
};

struct VersionParameter final : Parameter {
  std::string Value;

  size_t GetSize() override { return Value.size(); }

  std::vector<uint8_t> Serialize() override { return {Value.begin(), Value.end()}; }

  ptrdiff_t Deserialize(const std::span<uint8_t> param) override {
    Value = std::string(param.begin(), param.end());
    return Value.size();
  };

  ~VersionParameter() {};
};

struct Message {
  uint64_t Id{0};
  MessageType Type{MessageType_Dummy};
  uint8_t Address{0x00};
  uint8_t Cmd{};
  std::string Name{};
  std::vector<std::shared_ptr<Parameter> > Parameters;

  Message(const MessageType type, const uint8_t cmd, std::string name, std::vector<std::shared_ptr<Parameter> > parameters = {}) : Type(type), Cmd(cmd), Name(std::move(name)),
    Parameters(std::move(parameters)) {
  }

  [[nodiscard]] std::vector<uint8_t> SerializeParams() const {
    std::vector<uint8_t> res;
    for (const auto& param : Parameters) {
      if (param) {
        auto ser = param->Serialize();
        std::ranges::copy(ser, std::back_inserter(res));
      }
    }
    return res;
  }

  void DeserializeParams(const std::span<uint8_t> params) const {
    ptrdiff_t offset = 0;
    for (const auto& param : Parameters) {
      if (param) {
        offset += param->Deserialize({params.begin() + offset, params.end()});
      }
    }
  }

  std::vector<uint8_t> Serialize() {
    std::vector<uint8_t> buffer;
    auto params = SerializeParams();
    const uint16_t length = params.size() + 3;

    buffer.emplace_back(Type); // Header
    buffer.emplace_back(length >> 8); // Length high
    buffer.emplace_back(length & 0xFF); // Length low
    buffer.emplace_back(Address); // RS-485 address
    buffer.emplace_back(Cmd); // Cmd code
    std::ranges::copy(params, std::back_inserter(buffer)); // Params
    buffer.emplace_back(CalculateChecksum(buffer)); // Checksum

    return buffer;
  }

  void Deserialize(std::vector<uint8_t> data) {
    // uint16_t length = ((data[1] << 8) | data[2]);
    Type = static_cast<MessageType>(data[0]);
    Address = data[3];
    Cmd = data[4];
    DeserializeParams({data.begin() + 5, data.end() - 1});
  }

  static uint8_t CalculateChecksum(const std::span<uint8_t> data) {
    uint8_t checksum = 0;
    for (const auto b : data) {
      checksum += b;
    }
    return ((~checksum) + 1);
  }
};

struct MessageBuilder {
  std::string Name;
  std::function<Message()> BuildCommand;
  std::function<Message()> BuildReply;
  std::function<Message()> BuildStatus;
};

const std::map<uint8_t, MessageBuilder> MessageBuilderRegistry = {
    {0x05, MessageBuilder{.Name = "Get version number",
                          .BuildCommand = [] { return Message{MessageType_Command, 0x05, "Get version number"}; },
                          .BuildReply = [] { return Message{MessageType_Reply, 0x05, "Get version number", {std::make_shared<VersionParameter>()}}; }
     },},
    {0x10, MessageBuilder{.Name = "Get output power",
                          .BuildCommand = [] { return Message{MessageType_Command, 0x10, "Get output power", {std::make_shared<ByteParameter>("Antenna number")}}; },
                          .BuildReply = [] { return Message{MessageType_Reply, 0x10, "Get output power", {std::make_shared<ByteParameter>("Power value")}}; },
     },}
};

} // vanch
