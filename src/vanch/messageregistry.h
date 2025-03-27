#pragma once

#include "message.h"

namespace vanch {
using MessageCreator = std::shared_ptr<Message>(*)(const std::vector<uint8_t>&);

struct Entry {
  std::string name;
  MessageCreator creator{};
};

class MessageRegistry {
public:
  struct Entry {
    std::string name;
    MessageCreator creator{};
  };

  [[nodiscard]] std::shared_ptr<Message> createMessage(const std::vector<uint8_t>& data) const {
    if (data.size() < 6) {
      // Минимальная длина пакета: Header (1) + Length (2) + Address (1) + Cmd (1) + Check (1)
      return ErrorResponse::create("Invalid packet length", 0xFF);
    }

    auto header = static_cast<MessageType>(data[0]);
    CmdCode cmd = data[4];

    if (const auto key = std::make_pair(header, cmd); m_creators.contains(key)) {
      return m_creators.at(key).creator(data);
    }

    return ErrorResponse::create("Unknown Command Code", 0xFF);
  }

  void registerMessage(MessageType header, CmdCode cmd, const std::string& name, const MessageCreator creator) {
    m_creators[std::make_pair(header, cmd)] = Entry{name, creator};
  }

private:
  std::map<std::pair<MessageType, CmdCode>, Entry> m_creators;
};
} // vanch