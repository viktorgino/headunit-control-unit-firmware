#pragma once

#include "../hal_common.h"
#include <Arduino.h>

#if defined(__IMXRT1062__)
    #define SERIAL_PRIMARY Serial2
#endif
#if defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
    #define SERIAL_PRIMARY Serial3
#endif
// #define SERIAL_PRIMARY Serial
// #define SERIAL_GVRET Serial2
// #define SERIAL_GVRET Serial2
// #define SERIAL_GVRET Serial4

#define SERIAL_DEBUG Serial

#define SERIAL_LIN1 Serial3
#define SERIAL_LIN2 Serial5
