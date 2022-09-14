#pragma once
#include <stdint.h>
#include "../common.h"
#include "../config.h"

typedef struct CanMessage {
  uint32_t id = 0;          // Can message ID
  bool extended = 0;        // Extended message ID?
  uint8_t len = 8;          // Can message length
  uint8_t buf[8] = { 0 };   // Can message buffer
} CanMessage;

typedef void (*CanMessageCallback)(BusNumber, CanMessage);

class CanBusAbstract
{
public:
    explicit CanBusAbstract(CanMessageCallback callback) : m_callback(callback)  { }
    virtual void setBaudRate(BusNumber bus, uint32_t speed);
    virtual uint32_t getBaudRate(BusNumber bus);
    virtual void sendFrame(BusNumber bus, CanMessage &frame);
    virtual void loop();
protected:
    CanMessageCallback m_callback;
};
