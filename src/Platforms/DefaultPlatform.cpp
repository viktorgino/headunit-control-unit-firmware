#include "DefaultPlatform.h"

#if PLATFORM == DEFAULT_PLATFORM

Platform::Platform(HUDSerial::HUDSerial *hudSerial) : PlatformAbstract(hudSerial),
    m_climateControlFrame(),
    m_customCommandControlFrame(),
    m_bodyControlCommandFrame(),
    m_driveTrainControlCommandFrame(){
}
void Platform::loop(){
    
}

void Platform::setup(){
    
}

void Platform::receiveBusMessage(BusNumber bus, BusMessage message){
    
}

void Platform::receiveCustomCommand(const CustomCommandFrame &commandFrame){
    memcpy(&m_customCommandControlFrame, &commandFrame, sizeof(CustomCommandFrame));
    m_hudSerial->sendCustomCommand(m_customCommandControlFrame);    
}
void Platform::receiveClimateControlCommand(const ClimateControlCommandFrame &climateControlFrame){
    memcpy(&m_climateControlFrame, &climateControlFrame, sizeof(ClimateControlCommandFrame));    
    m_hudSerial->sendClimateControlCommand(m_climateControlFrame);
}
void Platform::receiveBodyControlCommand(const BodyControlCommandFrame &commandFrame){
    memcpy(&m_bodyControlCommandFrame, &commandFrame, sizeof(BodyControlCommandFrame));    
    m_hudSerial->sendBodyControlCommand(m_bodyControlCommandFrame);
};
void Platform::receiveDriveTrainControlCommand(const DriveTrainControlCommandFrame &commandFrame){
    memcpy(&m_driveTrainControlCommandFrame, &commandFrame, sizeof(DriveTrainControlCommandFrame));    
    m_hudSerial->sendDriveTrainControlCommand(m_driveTrainControlCommandFrame);
};
#endif
