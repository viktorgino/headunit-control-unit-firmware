#pragma once

#include "../Platform.h"
#if PLATFORM == KEYPAD_PLATFORM

const byte ROWS = 8;
const byte COLUMNS = 3;

const byte ROW_PINS[ROWS] = {2, 5, 10, 12, 14, 15, 16, 17};
const byte COLUMN_PINS[COLUMNS] = {1, 6, 11};

class Platform : public PlatformAbstract {
   public:
    Platform(HUDSerial::HUDSerial *hudSerial);
    void loop() override;
    void setup() override;
    void receiveBusMessage(BusNumber bus, BusMessage message) override;
    void receiveCustomCommand(const CustomCommandFrame &commandFrame) override;
    void receiveClimateControlCommand(const ClimateControlCommandFrame &climateControlFrame) override;
    void receiveBodyControlCommand(const BodyControlCommandFrame &commandFrame) override;
    void receiveDriveTrainControlCommand(const DriveTrainControlCommandFrame &commandFrame) override;
   private:

    uint16_t timerCount = 0;
    bool gpioState[COLUMNS][ROWS] = {};
    bool gpioPreviousState[COLUMNS][ROWS] = {};

    bool keyState[COLUMNS][ROWS] = {};
    bool keyPrevState[COLUMNS][ROWS] = {};

    uint8_t climateControlState = 0;
    ClimateControlCommandFrame controlFrame;
};

#endif