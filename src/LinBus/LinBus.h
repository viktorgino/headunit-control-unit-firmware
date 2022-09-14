#pragma once
#include <Stream.h>
#include <stdint.h>

#include "../HUDSerial/HUDSerial.h"
#include "../common.h"
#include "../config.h"

typedef struct LinMessage {
    uint8_t id = 0;        // Can message ID
    uint8_t len = 8;       // Can message length
    uint8_t buf[8] = {0};  // Can message buffer
    uint8_t cursor = 0;
    uint8_t bytesSent = 0;
    uint8_t checksum = 0;
} LinMessage;

typedef enum LinNodeType { LIN_MASTER, LIN_SLAVE } LinNodeType;

typedef void (*LinMessageCallback)(LinMessage);

class LinBusAbstract {
   public:
    explicit LinBusAbstract(HardwareSerial serialPort, LinNodeType type, LinMessageCallback callback)
        : m_serialPort(serialPort), m_type(type), m_callback(callback), m_enhancedChecksum(false) {}
    virtual void setBaudRate(uint32_t speed);
    virtual uint32_t getBaudRate();
    virtual int sendFrame(LinMessage &frame);
    virtual void loop();
    void setEnhancedChecksum(bool enhancedChecksum) {m_enhancedChecksum=enhancedChecksum;}

   protected:
    HardwareSerial m_serialPort;
    LinNodeType m_type;
    LinMessageCallback m_callback;
    bool m_enhancedChecksum;
};
