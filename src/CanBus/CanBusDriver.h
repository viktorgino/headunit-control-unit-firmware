#pragma once
#include "../config.h"

#if CANBUS == TEENSY_CAN
#include "CanBus/TeensyCan/TeensyCan.h"
#elif CANBUS == MCP2515_CAN
#include "CanBus/MCP2515/MCP2515Can.h"
#endif


extern CanBus canBus;