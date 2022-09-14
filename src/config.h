#pragma once

#define DEFAULT 0

#define DEBUG 1

// Boards
#define TEENSY_BOARD 2
#define UNO_BOARD 3

// Platforms
#define DEFAULT_PLATFORM 2
#define KEYPAD_PLATFORM 3
#define VOLVO_P1_1 4

// Can Bus drivers
#define TEENSY_CAN 2
#define MCP2515_CAN 3

#ifndef PLATFORM
    #define PLATFORM DEFAULT_PLATFORM
#endif
#ifndef BOARD
    #define BOARD DEFAULT
#endif

#ifdef ARDUINO
    #if defined(__IMXRT1062__)
        #define LINBUS ARDUINO_LIN
    #endif
#endif

#if BOARD == TEENSY_BOARD
    #define CANBUS TEENSY_CAN
#elif BOARD == UNO_BOARD
    #define CANBUS MCP2515_CAN
    #define CAN0_PIN 8
    #define CAN1_PIN 9
    #define CAN2_PIN 10
#endif
