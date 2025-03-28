#include "messageregistry.h"

#include "commands/command.h"

namespace vanch {

#define REG_CMD_RET(name) \
  registerMessage(Cmd##name##_Traits::s_cmdCode, Cmd##name##_Traits::s_header, []{ return std::make_shared<Cmd##name>(); }, Cmd##name##_Traits::getName()); \
  registerMessage(Ret##name##_Traits::s_cmdCode, Ret##name##_Traits::s_header, []{ return std::make_shared<Ret##name>(); }, Ret##name##_Traits::getName()); \

void MessageRegistry::init() {
  static bool initialized = false;
  if (initialized) return;

  REG_CMD_RET(SetBaudRate);
  REG_CMD_RET(GetBaudRate);
  REG_CMD_RET(GetVersionNumber);
}

} // namespace vanch
