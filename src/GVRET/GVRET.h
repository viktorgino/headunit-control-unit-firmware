/* MIT License
 * 
 * Copyright (c) 2018 Collin Kidder
 * Copyright (c) 2021 Viktor Verebelyi
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 */

#pragma once
#include <Arduino.h>

#include <FlexCAN_T4.h>
#include <EEPROM.h>
#include "../HAL/hal.h"
#include "CanBus/CanBusDriver.h"
#include "LinBus/LinBusDriver.h"
#include "../Platforms/Platform.h"

#ifdef SERIAL_GVRET

class GVRET
{
public:    
    void loop();
    void displayFrame(BusMessage &frame, int whichBus, bool extended = false);
    
private:
    enum STATE {
        IDLE,
        GET_COMMAND,
        BUILD_CAN_FRAME,
        TIME_SYNC,
        GET_DIG_INPUTS,
        GET_ANALOG_INPUTS,
        SET_DIG_OUTPUTS,
        SETUP_CANBUS,
        GET_CANBUS_PARAMS,
        GET_DEVICE_INFO,
        SET_SINGLEWIRE_MODE,
        SET_SYSTYPE,
        ECHO_CAN_FRAME,
        SETUP_EXT_BUSES
    };

    enum GVRET_PROTOCOL
    {
        PROTO_BUILD_CAN_FRAME = 0,
        PROTO_TIME_SYNC = 1,
        PROTO_DIG_INPUTS = 2,
        PROTO_ANA_INPUTS = 3,
        PROTO_SET_DIG_OUT = 4,
        PROTO_SETUP_CANBUS = 5,
        PROTO_GET_CANBUS_PARAMS = 6,
        PROTO_GET_DEV_INFO = 7,
        PROTO_SET_SW_MODE = 8,
        PROTO_KEEPALIVE = 9,
        PROTO_SET_SYSTYPE = 10,
        PROTO_ECHO_CAN_FRAME = 11,
        PROTO_GET_NUMBUSES = 12,
        PROTO_GET_EXT_BUSES = 13,
        PROTO_SET_EXT_BUSES = 14
    };
    uint8_t popBuffer();
    void appendToBuffer(uint8_t byt);
    void processIncomingByte(uint8_t in_byte);

    byte ringBufferTransmitBuffer[1024];
    uint16_t ringBufferTransmitBufferWriteIndex;
    uint16_t ringBufferTransmitBufferReadIndex;
    uint16_t ringBufferTransmitBufferSize;

    CanMessage busMessage;
    STATE state;
};

extern GVRET gvret;
#endif