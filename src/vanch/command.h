//
// Created by nickz on 3/18/2025.
//

#pragma once

namespace vanch {

enum class ParameterType {
  Custom,
  Byte,
  UInt16,
  STRING,
  ENUM,
  FLAGS
};

struct Parameter {
  std::string Name;
  ParameterType Type{ParameterType::Custom};
  std::vector<uint8_t> Data;
  std::vector<std::pair<std::string, int>> EnumOptions;
  bool IsOptional{false};
};

struct Command {
  using ResponseParser = std::function<void(const std::vector<uint8_t>&)>;

  uint8_t Code;
  std::string Name;
  std::vector<Parameter> Parameters;
  ResponseParser ParseResponse;
};

} // vanch
