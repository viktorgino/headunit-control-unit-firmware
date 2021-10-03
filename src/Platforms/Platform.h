#ifndef Platform_H
#define Platform_H

#include "../CanBus/CanBusDriver.h"
#include "../LinBus/LinBusDriver.h"
#include "../HUDSerial/HUDSerial.h"
#include "../config.h"
#include "../common.h"

typedef struct PlatformSettings {
    struct {
        bool enabled = false;
        uint32_t speed = 0; 
    } can0;
    struct {
        bool enabled = false;
        uint32_t speed = 0; 
    } can1;
    struct {
        bool enabled = false;
        uint32_t speed = 0; 
    } can2;
    struct {
        bool enabled = false;
        uint32_t speed = 0; 
    } lin0;
    struct {
        bool enabled = false;
        uint32_t speed = 0; 
    } lin1;

} PlatformSettings;

typedef struct BusMessage {
  uint32_t id = 0;          // Message ID
  uint8_t len = 8;          // Message length
  uint8_t *buf;   // Message buffer
} BusMessage;

class PlatformAbstract
{
public:
    PlatformAbstract(HUDSerial::HUDSerial *hudSerial) : m_hudSerial(hudSerial),  m_settings() {}
    virtual void loop();
    virtual void setup();
    virtual void receiveBusMessage(BusNumber bus, BusMessage message);
    virtual void receiveClimateControlCommand(const ClimateControlCommandFrame &climateControlFrame) {}
    virtual void receiveCustomCommand(const CustomCommandFrame &commandFrame) {}
    virtual void receiveButtonInputCommand(Keys key) {}
    virtual void receiveBodyControlCommand(const BodyControlCommandFrame &commandFrame) { }
    virtual void receiveDriveTrainControlCommand(const DriveTrainControlCommandFrame &commandFrame) { }
    PlatformSettings getSettings(){
        return m_settings;
    }
protected:
    HUDSerial::HUDSerial *m_hudSerial;
    PlatformSettings m_settings;
};

#endif