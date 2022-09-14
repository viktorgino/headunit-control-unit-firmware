#include "MCP2515Can.h"

#if CANBUS == MCP2515_CAN
CanBus::CanBus(CanMessageCallback callback) : CanBusAbstract(callback),
    CAN0(CAN0_PIN),
    CAN1(CAN1_PIN),
    CAN2(CAN2_PIN)
    {
    CAN0.begin(MCP_ANY, CAN_125KBPS, MCP_8MHZ);
    CAN1.begin(MCP_ANY, CAN_125KBPS, MCP_8MHZ);
    CAN2.begin(MCP_ANY, CAN_125KBPS, MCP_8MHZ);
    
    CAN0.setMode(MCP_NORMAL); 
    CAN1.setMode(MCP_NORMAL); 
    CAN2.setMode(MCP_NORMAL); 
}

void CanBus::setBaudRate(BusNumber bus, uint32_t speed) {}

void CanBus::sendFrame(BusNumber bus, CanMessage &frame) {
    if (bus == Bus_Can0) {
        CAN0.sendMsgBuf(frame.id, frame.extended, frame.len, frame.buf);
    } else if (bus == Bus_Can1) {
        CAN1.sendMsgBuf(frame.id, frame.extended, frame.len, frame.buf);
    } else if (bus == Bus_Can2) {
        CAN2.sendMsgBuf(frame.id, frame.extended, frame.len, frame.buf);
    } else {
        Serial.print("Invalid CAN Bus ");
        Serial.println(bus, DEC);
        return;
    }
}
void CanBus::loop() {
    CanMessage msg1;
    if (CAN0.readMsgBuf(&msg1.id, &msg1.len, msg1.buf) == CAN_OK) {
        {
            msg1.extended = msg1.id > 0x07FF;
            m_callback(Bus_Can0, msg1);
        }
    }
    CanMessage msg2;
    if (CAN1.readMsgBuf(&msg2.id, &msg2.len, msg2.buf) == CAN_OK) {
        {
            msg2.extended = msg2.id > 0x07FF;
            m_callback(Bus_Can1, msg2);
        }
    }
    CanMessage msg3;
    if (CAN2.readMsgBuf(&msg3.id, &msg3.len, msg3.buf) == CAN_OK) {
        {
            msg3.extended = msg3.id > 0x07FF;
            m_callback(Bus_Can2, msg3);
        }
    }
}

#endif