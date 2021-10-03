#pragma once
#include "../config.h"

#if LINBUS == ARDUINO_LIN
#include "LinBus/ArduinoLin/ArduinoLin.h"
#endif


extern LinBus linBus;