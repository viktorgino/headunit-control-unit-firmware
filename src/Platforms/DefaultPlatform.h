#pragma once

#include "Platform.h"
#if PLATFORM == DEFAULT_PLATFORM

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
    ClimateControlCommandFrame m_climateControlFrame;
    CustomCommandFrame m_customCommandControlFrame;
    BodyControlCommandFrame m_bodyControlCommandFrame;
    DriveTrainControlCommandFrame m_driveTrainControlCommandFrame;
};

#endif