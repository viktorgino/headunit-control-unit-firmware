#include "TeensyCan.h"

#if CANBUS == TEENSY_CAN
CanBus::CanBus(CanMessageCallback callback) : CanBusAbstract(callback)
{
    can0.begin();

#if defined(__IMXRT1062__)
    can1.begin();
    can2.begin();
#endif
}

void CanBus::setBaudRate(BusNumber bus, uint32_t speed)
{
    if (bus == Bus_Can0)
    {
        can0.setBaudRate(speed);
    }
#if defined(__IMXRT1062__)
    else if (bus == Bus_Can1)
    {
        can1.setBaudRate(speed);
    }
    else if (bus == Bus_Can2)
    {
        can2.setBaudRate(speed);
    }
#endif
}

uint32_t CanBus::getBaudRate(BusNumber bus)
{
    if (bus == Bus_Can0)
    {
        return can0.getBaudRate();
    }
#if defined(__IMXRT1062__)
    else if (bus == Bus_Can1)
    {
        return can1.getBaudRate();
    }
    else if (bus == Bus_Can2)
    {
        return can2.getBaudRate();
    }
#endif
    return 0;
}
void CanBus::sendFrame(BusNumber bus, CanMessage &frame)
{
    CAN_message_t msg;
    memcpy(msg.buf, frame.buf, frame.len);
    msg.flags.extended = frame.extended;
    msg.id = frame.id;
    msg.len = frame.len;
    if (bus == Bus_Can0)
    {
        can0.write(msg);
    }
#if defined(__IMXRT1062__)
    else if (bus == Bus_Can1)
    {
        can1.write(msg);
    }
    else if (bus == Bus_Can2)
    {
        can2.write(msg);
    }
#endif
    else
    {
#if defined(DEBUG)
        hudSerial.sendDebugMessageCommand("Invalid CAN Bus");
#endif
    }
}
void CanBus::loop()
{
    CAN_message_t msg1;
    if (can0.read(msg1))
    {
        CanMessage msg;
        memcpy(msg.buf, msg1.buf, msg1.len);
        msg.extended = msg1.flags.extended;
        msg.id = msg1.id;
        msg.len = msg1.len;
        m_callback(Bus_Can0, msg);
    }
#if defined(__IMXRT1062__)
    CAN_message_t msg2;
    if (can1.read(msg2))
    {
        CanMessage msg;
        memcpy(msg.buf, msg2.buf, msg2.len);
        msg.extended = msg2.flags.extended;
        msg.id = msg2.id;
        msg.len = msg2.len;
        m_callback(Bus_Can1, msg);
    }
    CAN_message_t msg3;
    if (can2.read(msg3))
    {
        CanMessage msg;
        memcpy(msg.buf, msg3.buf, msg3.len);
        msg.extended = msg3.flags.extended;
        msg.id = msg3.id;
        msg.len = msg3.len;
        m_callback(Bus_Can2, msg);
    }
#endif
}

#endif