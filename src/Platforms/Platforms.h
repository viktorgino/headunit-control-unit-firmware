#pragma once

#include "../config.h"
#include "Platform.h"

#if (PLATFORM == VOLVO_P1_1)
#include "Volvo/PlatformVolvoP1_1.h"
#elif (PLATFORM == KEYPAD_PLATFORM)
#include "Keypad/PlatformKeypad.h"
#else
#include "DefaultPlatform.h"
#endif

extern Platform platform;