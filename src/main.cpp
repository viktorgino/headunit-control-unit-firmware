#include "CanBus/CanBusDriver.h"
#include "LinBus/LinBusDriver.h"
#include "HAL/hal.h"
#include "HUDSerial/HUDSerial.h"
#include "Platforms/Platforms.h"
#include "common.h"
#include "config.h"

#ifdef SERIAL_GVRET
#include "GVRET/GVRET.h"
GVRET gvret;
#endif

void canBusReceiveBusMessage(BusNumber bus, CanMessage message) {
    BusMessage busMessage;
    busMessage.buf = message.buf;
    busMessage.len = message.len;
    busMessage.id = message.id;
    platform.receiveBusMessage(bus, busMessage);
    
#ifdef SERIAL_GVRET
    if(bus == Bus_Can0) gvret.displayFrame(busMessage, 0, message.extended);
    if(bus == Bus_Can1) gvret.displayFrame(busMessage, 1, message.extended);
#endif
    }
void linBusReceiveBusMessage(LinMessage message) {
    BusMessage busMessage;
    busMessage.buf = message.buf;
    busMessage.len = message.len;
    busMessage.id = message.id;
    platform.receiveBusMessage(Bus_Lin0, busMessage);

#ifdef SERIAL_GVRET
    gvret.displayFrame(busMessage, 2);
#endif
}

class Callbacks : public PlatformCallbacks {
    void ClimateControlCallback(const ClimateControlCommandFrame &controlFrame) override {
        platform.receiveClimateControlCommand(controlFrame);
    }

    void CustomCommandCallback(const CustomCommandFrame &commandFrame) override {
        platform.receiveCustomCommand(commandFrame);
    }
    void ButtonInputCommandCallback(Keys key) override { platform.receiveButtonInputCommand(key); }

    void SendMessageCallback(uint8_t sendByte) override {
        SERIAL_PRIMARY.print((char)sendByte);
    }
    void PrintString(char *message, int len) override {

    }
    
    void BodyControlCommandCallback(const BodyControlCommandFrame &commandFrame) override {
        platform.receiveBodyControlCommand(commandFrame);
    }
    void DriveTrainControlCommandCallback(const DriveTrainControlCommandFrame &commandFrame) override {
        platform.receiveDriveTrainControlCommand(commandFrame);
    }
};

HUDSerial::HUDSerial hudSerial;
CanBus canBus(canBusReceiveBusMessage);
LinBus linBus(SERIAL_LIN1, LIN_SLAVE, linBusReceiveBusMessage);
Platform platform(&hudSerial);
Callbacks callbacks;

void setup() {
    HAL::init();
    hudSerial.setCallbacks(&callbacks);
}

void loop() {
    if (SERIAL_PRIMARY.available() > 0) {
        char received = SERIAL_PRIMARY.read();
        hudSerial.receiveByte(received);
    }
    canBus.loop();
    linBus.loop();
    HAL::loop();
    if (HAL::TimerExpired) {
        platform.loop();
        hudSerial.loop();
        HAL::TimerExpired = false;
    }
    #ifdef SERIAL_GVRET
    gvret.loop();
    #endif
}