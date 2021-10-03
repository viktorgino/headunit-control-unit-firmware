#pragma once

#include "../CanBus.h"
#if CANBUS == MCP2515_CAN
#include <Arduino.h>
#include <mcp_can.h>
#include <SPI.h>

class CanBus : public CanBusAbstract
{
public:
    CanBus(CanMessageCallback callback);
    void setBaudRate(BusNumber bus, uint32_t speed) override;
    void sendFrame(BusNumber bus, CanMessage &frame) override;
    void loop() override;
private:
    bool m_enabled = false;
    MCP_CAN CAN0;
    MCP_CAN CAN1;
    MCP_CAN CAN2;
};
#endif