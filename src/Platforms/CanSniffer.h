#pragma once

#include "Platform.h"

#if PLATFORM == DEFAULT_PLATFORM 

class Platform : public PlatformAbstract {
   public:
    Platform(HUDSerial *hudSerial);
    void loop() override;
    void setup() override;
    void receiveBusMessage(BusNumber bus, CanMessage message) override;
    void receiveClimateControlCommand(ClimateControlCommandFrame climateControlFrame) override;
    void receiveCustomCommand(CustomCommandFrame commandFrame) override;

   private:
    ClimateControlCommandFrame m_climateControlFrame;
    CustomCommandFrame m_customCommandControlFrame;
};
#endif