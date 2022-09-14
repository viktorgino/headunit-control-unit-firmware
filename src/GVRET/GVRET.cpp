/*
Implements handling of the GVRET comm protocol, both sending and receiving
*/

#include "GVRET.h"

#ifdef SERIAL_GVRET

void GVRET::processIncomingByte(uint8_t in_byte) {
    uint32_t now = micros();

    switch (state) {
        case IDLE:
            if (in_byte == 0xF1) {
                state = GET_COMMAND;
            }
            break;
        case GET_COMMAND:
            switch (in_byte) {
                case PROTO_TIME_SYNC:// time sync
                    appendToBuffer(0xF1);
                    appendToBuffer(PROTO_TIME_SYNC);  
                    appendToBuffer((uint8_t)(now & 0xFF));
                    appendToBuffer((uint8_t)(now >> 8));
                    appendToBuffer((uint8_t)(now >> 16));
                    appendToBuffer((uint8_t)(now >> 24));
                    state = IDLE;
                    break;
                case PROTO_GET_CANBUS_PARAMS:// return data on canbus params
                    appendToBuffer(0xF1);
                    appendToBuffer(PROTO_GET_CANBUS_PARAMS);
                    appendToBuffer(1);
                    appendToBuffer(canBus.getBaudRate(Bus_Can0));
                    appendToBuffer(canBus.getBaudRate(Bus_Can0) >> 8);
                    appendToBuffer(canBus.getBaudRate(Bus_Can0) >> 16);
                    appendToBuffer(canBus.getBaudRate(Bus_Can0) >> 24);
                    appendToBuffer(1);
                    appendToBuffer(canBus.getBaudRate(Bus_Can1));
                    appendToBuffer(canBus.getBaudRate(Bus_Can1) >> 8);
                    appendToBuffer(canBus.getBaudRate(Bus_Can1) >> 16);
                    appendToBuffer(canBus.getBaudRate(Bus_Can1) >> 24);
                    appendToBuffer(1);
                    appendToBuffer(linBus.getBaudRate());
                    appendToBuffer(linBus.getBaudRate() >> 8);
                    appendToBuffer(linBus.getBaudRate() >> 16);
                    appendToBuffer(linBus.getBaudRate() >> 24);
                    state = IDLE;
                    break;
                case PROTO_GET_DEV_INFO:// return device information
                    appendToBuffer(0xF1);
                    appendToBuffer(PROTO_GET_DEV_INFO);
                    appendToBuffer(618 & 0xFF);
                    appendToBuffer((618 >> 8));
                    appendToBuffer(0x20);
                    appendToBuffer(0);
                    appendToBuffer(0);
                    appendToBuffer(0);
                    state = IDLE;
                    break;
                case PROTO_KEEPALIVE:
                    appendToBuffer(0xF1);
                    appendToBuffer(PROTO_KEEPALIVE);
                    appendToBuffer(0xDE);
                    appendToBuffer(0xAD);
                    state = IDLE;
                    break;
                case PROTO_GET_NUMBUSES:
                    appendToBuffer(0xF1);
                    appendToBuffer(PROTO_GET_NUMBUSES);
                    appendToBuffer(3);
                    state = IDLE;
                    break;
                case PROTO_GET_EXT_BUSES:
                    appendToBuffer(0xF1);
                    appendToBuffer(PROTO_GET_EXT_BUSES);
                    for (int u = 2; u < 17; u++) appendToBuffer(0);
                    state = IDLE;
                    break;
                    
                default :
                    state = IDLE;
            }
            break;
        default:
            state = IDLE;
            break;
    }
}

uint8_t GVRET::popBuffer() {
    if (ringBufferTransmitBufferSize > 0)
    {
        ringBufferTransmitBufferReadIndex++;
        if(ringBufferTransmitBufferReadIndex >= 1024) {
            ringBufferTransmitBufferReadIndex = 0;
        }
        ringBufferTransmitBufferSize--;
        return ringBufferTransmitBuffer[ringBufferTransmitBufferReadIndex];
    } else {
        return 0x0;
    }
}

void GVRET::appendToBuffer(uint8_t byt) {
    if(ringBufferTransmitBufferSize < 1024){
        ringBufferTransmitBufferWriteIndex++;
        if(ringBufferTransmitBufferWriteIndex >= 1024) {
            ringBufferTransmitBufferWriteIndex = 0;
        }
        ringBufferTransmitBufferSize++;
        ringBufferTransmitBuffer[ringBufferTransmitBufferWriteIndex] = byt;
    }
}

void GVRET::displayFrame(BusMessage &frame, int whichBus, bool extended) {
    if (extended) frame.id |= 1 << 31;
    appendToBuffer(0xF1);
    appendToBuffer(0);  // 0 = canbus frame sending
    uint32_t now = micros();
    appendToBuffer((uint8_t)(now & 0xFF));
    appendToBuffer((uint8_t)(now >> 8));
    appendToBuffer((uint8_t)(now >> 16));
    appendToBuffer((uint8_t)(now >> 24));
    appendToBuffer((uint8_t)(frame.id & 0xFF));
    appendToBuffer((uint8_t)(frame.id >> 8));
    appendToBuffer((uint8_t)(frame.id >> 16));
    appendToBuffer((uint8_t)(frame.id >> 24));
    appendToBuffer(frame.len + (uint8_t)(whichBus << 4));
    for (int c = 0; c < frame.len; c++) {
        appendToBuffer(frame.buf[c]);
    }
    appendToBuffer(0x00);
}

void GVRET::loop() {
    if(SERIAL_GVRET.available()) {
        processIncomingByte(SERIAL_GVRET.read());
    }
    if(SERIAL_GVRET.availableForWrite() && ringBufferTransmitBufferSize > 0){
        SERIAL_GVRET.write(popBuffer());
    }
}
#endif