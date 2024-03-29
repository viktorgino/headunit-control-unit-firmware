#pragma once

#include "../CanBus.h"
#if CANBUS == TEENSY_CAN
#include <FlexCAN_T4.h>
#include "HUDSerial/HUDSerial.h"

class CanBus : public CanBusAbstract
{
public:
    CanBus(CanMessageCallback callback);
    void setBaudRate(BusNumber bus, uint32_t speed) override;
    uint32_t getBaudRate(BusNumber bus) override;
    void sendFrame(BusNumber bus, CanMessage &frame) override;
    void loop() override;

private:
#if defined(__IMXRT1062__)
    FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can0;
    FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> can1;
    FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> can2;
#endif
#if defined(__MK20DX256__) || defined(__MK64FX512__) || defined(__MK66FX1M0__)
    FlexCAN_T4<CAN0, RX_SIZE_256, TX_SIZE_16> can0;
#endif
    bool m_enabled = false;
};
#endif