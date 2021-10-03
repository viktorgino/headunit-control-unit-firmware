#pragma once

#include "../Platform.h"
#if PLATFORM == VOLVO_P1_1

enum VolvoP1CanIDs{
    Key_Pad =  0x04A0409E,
    Media_Buttons = 0x05704000,
    Car_Settings_Send = 0x08E2300E,
    Car_Settings_Receive = 0x0A90F608,
    AC_Settings = 0x12404002,
    Environment_Sensor = 0x1E30104A,
    Indicators = 0x02120498,
    KeyPosition = 0x03C3F7FC
};
class Platform : public PlatformAbstract {
   public:
    Platform(HUDSerial::HUDSerial *hudSerial);
    void loop() override;
    void setup() override;
    void receiveBusMessage(BusNumber bus, BusMessage message) override;
    void receiveCustomCommand(const CustomCommandFrame &commandFrame) override;

   private:
    uint16_t timerCount;
    uint8_t m_settingsTimerCount;
    ClimateControlCommandFrame climateControl;
    CustomCommandFrame m_carSettings;
    CustomCommandFrame m_carSettingsReceived;
    BodyControlCommandFrame m_bodyControlFrame;
    bool m_ccLoaded;
    bool m_carSettingsLoaded;
    bool m_ccSettingsChanged;
    bool m_carSettingsChanged;
    void sendCarSettings();
};

#endif