#pragma once

#include "vanch/message.h"
#include "vanch/messageregistry.h"

#define VANCH_DEFINE_STATUS(MessageName, cmdCode, description) VANCH_DEFINE_MESSAGE(Stat##MessageName, cmdCode, MessageType_Status, description)
#define VANCH_DEFINE_STATUS1(MessageName, cmdCode) VANCH_DEFINE_STATUS(MessageName, cmdCode, #MessageName)
