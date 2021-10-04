#include "PlatformVolvoP1_1.h"

#if PLATFORM == VOLVO_P1_1

Platform::Platform(HUDSerial::HUDSerial *hudSerial) : PlatformAbstract(hudSerial),
      timerCount(0),
      m_settingsTimerCount(0),
      climateControl(),
      m_carSettings(),
      m_carSettingsReceived(),
      m_bodyControlFrame(),
      m_ccLoaded(false),
      m_ccSettingsChanged(false),
      m_carSettingsChanged(false){    
    canBus.setBaudRate(Bus_Can0, 125000);
    canBus.setBaudRate(Bus_Can1, 125000);
    canBus.setBaudRate(Bus_Can2, 125000);
    linBus.setBaudRate(9600);
    linBus.setEnhancedChecksum(false);
}
void Platform::loop() {
    switch (timerCount++) {
        case 50:
            sendCarSettings();
            break;
        default:
            break;
    }
    if (timerCount >= 100) {
        timerCount = 0;
    }
}

void Platform::setup() {}

void Platform::sendCarSettings() {
    uint8_t autoBlower;
    switch (m_carSettingsReceived.Bytes[0]) {
        case 0: {
            autoBlower = 0b0111;
        } break;
        case 1: {
            autoBlower = 0;
        } break;
        case 2:
        default: {
            autoBlower = 0b0001;
        } break;
    }

    CanMessage message;
    message.extended = true;
    message.id = Car_Settings_Send;
    message.len = 8;
    message.buf[0] = m_settingsTimerCount++ << 6;                           // Timer
    message.buf[2] = (m_ccSettingsChanged & 0b00000001);                    // AC Settings changed flag
    message.buf[4] = m_carSettingsReceived.Bits[1] |                        // Lock confirm lights
                     (m_carSettingsReceived.Bits[2] << 1) |                 // Unlock confrim lights
                     (m_carSettingsReceived.Bits[3] << 2) |                 // Doors auto lock
                     ((m_carSettingsReceived.Bytes[1] & 0b00000011) << 3) | // Doors unlock driver, then all
                     ((m_carSettingsReceived.Bytes[2] & 0b00000011) << 5) | // Approach light
                     (m_carSettingsReceived.Bits[4] << 7);                  // Home safe light
    message.buf[6] = m_carSettingsChanged << 7;                             // CEM Settings changed flag
    message.buf[7] = (m_carSettingsReceived.Bits[0] << 7) | autoBlower;     // Recirculation timer & auto blower
    canBus.sendFrame(Bus_Can0, message);
    canBus.sendFrame(Bus_Can1, message);

    if (m_settingsTimerCount >= 4) {
        m_settingsTimerCount = 0;
    }
}

void Platform::receiveBusMessage(BusNumber bus, BusMessage message) {
    // Setup message forwarding between the 2 CAN bus channels
    CanMessage canMessage;
    
    memcpy(canMessage.buf, message.buf, message.len);
    canMessage.len = message.len;
    canMessage.id = message.id;
    canMessage.extended = message.id > 255;
    if (bus == Bus_Can0) {
        canBus.sendFrame(Bus_Can1, canMessage);
    } else if (bus == Bus_Can1) {
        canBus.sendFrame(Bus_Can0, canMessage);
    }

    if (bus == Bus_Can0) {
        switch (message.id) {
            case Media_Buttons:  // Media Buttons
            {
                if ((message.buf[3] & 0b00001000) && (message.buf[3] & 0b00000100)){ // Tuning Sound
                    m_hudSerial->sendButtonInputCommand(Key_Sound);
                }
                if ((message.buf[4] & 0b10000000) && (message.buf[4] & 0b01000000)) { // Power
                    m_hudSerial->sendButtonInputCommand(Key_Power);
                }
                
                if ((message.buf[5] & 0b00000001) && (message.buf[5] & 0b00000010)) {  // AM/FM
                    m_hudSerial->sendButtonInputCommand(Key_AMFM);
                } 
                if ((message.buf[5] & 0b00000100) && (message.buf[5] & 0b00001000)) {  // CD
                    m_hudSerial->sendButtonInputCommand(Key_CD);
                } 
                if ((message.buf[5] & 0b00100000) && (message.buf[5] & 0b00010000)) {  // Eject
                    m_hudSerial->sendButtonInputCommand(Key_Eject);
                } 
                if ((message.buf[5] & 0b10000000) && (message.buf[5] & 0b01000000)) {  // Enter
                    m_hudSerial->sendButtonInputCommand(Key_Enter);
                }
                
                if ((message.buf[4] & 0b00000001) && (message.buf[4] & 0b00000010)) {  // Exit
                    m_hudSerial->sendButtonInputCommand(Key_Exit);
                }
                if ((message.buf[4] & 0b00000100) && (message.buf[4] & 0b00001000)) {  // Menu
                    m_hudSerial->sendButtonInputCommand(Key_Menu);
                }

                if (message.buf[1] & 0b00000001) {
                    if (message.buf[7] & 0b00000001) {  // Up button
                        m_hudSerial->sendButtonInputCommand(Key_Up);
                    } else if (message.buf[7] & 0b00000010) {  // Down button
                        m_hudSerial->sendButtonInputCommand(Key_Down);
                    } else if (message.buf[7] & 0b00000100) {  // Left button
                        m_hudSerial->sendButtonInputCommand(Key_Left);
                    } else if (message.buf[7] & 0b00001000) {  // Right button
                        m_hudSerial->sendButtonInputCommand(Key_Right);
                    }
                }

                if (message.buf[1] & 0b00010000) {      // Tune knob rotate
                    if (message.buf[3] & 0b10000000) {  // Tune up
                        m_hudSerial->sendButtonInputCommand(Key_Next);
                    } else if (message.buf[2] & 0b00000001) {  // Tune down
                        m_hudSerial->sendButtonInputCommand(Key_Previous);
                    }
                }

                if (message.buf[0] & 0b00000001) {      // Volume knob rotate
                    if (message.buf[2] & 0b01000000) {  // Volume up
                        m_hudSerial->sendButtonInputCommand(Key_VolumeUp);
                    } else if (message.buf[2] & 0b10000000) {  // Volume down
                        m_hudSerial->sendButtonInputCommand(Key_VolumeDown);
                    }
                }
            } break;
            case Key_Pad :
            {
                if (message.buf[6] & 0b00000100) {
                    if (message.buf[5] & 0b00000001) {  // 0
                        m_hudSerial->sendButtonInputCommand(Key_0);
                    } else if (message.buf[5] & 0b00000010) {  // 1
                        m_hudSerial->sendButtonInputCommand(Key_1);
                    } else if (message.buf[5] & 0b00000100) {  // 2
                        m_hudSerial->sendButtonInputCommand(Key_2);
                    } else if (message.buf[5] & 0b00001000) {  // 3
                        m_hudSerial->sendButtonInputCommand(Key_3);
                    } else if (message.buf[5] & 0b00010000) {  // 4
                        m_hudSerial->sendButtonInputCommand(Key_4);
                    } else if (message.buf[5] & 0b00100000) {  // 5
                        m_hudSerial->sendButtonInputCommand(Key_5);
                    } else if (message.buf[5] & 0b01000000) {  // 6
                        m_hudSerial->sendButtonInputCommand(Key_6);
                    } else if (message.buf[5] & 0b10000000) {  // 7
                        m_hudSerial->sendButtonInputCommand(Key_7);
                    }

                    if (message.buf[4] & 0b00000001) {  // 8
                        m_hudSerial->sendButtonInputCommand(Key_8);
                    } else if (message.buf[4] & 0b00000010) {  // 9
                        m_hudSerial->sendButtonInputCommand(Key_9);
                    } else if (message.buf[4] & 0b00000100) {  // Auto
                        m_hudSerial->sendButtonInputCommand(Key_Auto);
                    } else if (message.buf[4] & 0b00001000) {  // Scan
                        m_hudSerial->sendButtonInputCommand(Key_Scan);
                    }
                }
            } break;
            case AC_Settings :
            {
                bool sendCC = false;
                if ((message.buf[3] & 0b01000000) || !m_ccLoaded) {  // Temperature Select Update 
                
                    sendCC = true;
                    climateControl.TempSelectLeft = (message.buf[7] & 0b01000000);
                    climateControl.TempSelectRight = (message.buf[7] & 0b10000000);
                    if(!(climateControl.TempSelectLeft | climateControl.TempSelectRight)){
                        climateControl.TempSelectLeft = true;
                        climateControl.TempSelectRight = true;
                    }
                }
                if ((message.buf[2] & 0b10000000) || !m_ccLoaded) {  // Temperature Update
                    climateControl.Front.Left.Temperature = message.buf[1] & 0b00000001 ? 18 : message.buf[1] >> 1;
                    sendCC = true;
                }
                
                if ((message.buf[3] & 0b00100000) || !m_ccLoaded) {  // Temperature Update
                    climateControl.Front.Right.Temperature = message.buf[0] & 0b00000001 ? 18 : message.buf[0] >> 1;
                    sendCC = true;
                }
                if ((message.buf[7] & 0b00100000) || !m_ccLoaded) {  // Fan direction update

                    climateControl.ProgWindscreen = (message.buf[5] & 0b00001000);
                    climateControl.Front.Left.Direction.Auto = (((message.buf[7] & 0b00011100) >> 2) == 0);

                    climateControl.Front.Left.Direction.Up = (message.buf[7] & 0b00010000);
                    climateControl.Front.Left.Direction.Center = (message.buf[7] & 0b00001000);
                    climateControl.Front.Left.Direction.Down = (message.buf[7] & 0b00000100);

                    sendCC = true;
                }
                if ((message.buf[4] & 0b00100000) || !m_ccLoaded) {  // Fan update
                    uint8_t fan = (message.buf[3] & 0b00011110) >> 1;

                    climateControl.ProgAutoFanFront = (fan > 9);

                    climateControl.Front.Left.Fan = fan;

                    sendCC = true;
                }

                if (sendCC == true) {
                    climateControl.ProgAuto =
                        (climateControl.ProgAutoFanFront && climateControl.Front.Left.Direction.Auto);
                    m_hudSerial->sendClimateControlCommand(climateControl);
                }

                if (message.buf[5] & 0b10000000 || !m_ccLoaded) {           // AC settings update
                    m_carSettings.Bits[0] = (message.buf[6] & 0b10000000);  // Recirc timer enable

                    // Auto blower settings
                    uint8_t autoBlower = message.buf[6] & 0b00001111;
                    switch (autoBlower) {
                        case 0b0111: {
                            m_carSettings.Bytes[0] = 0;
                        } break;
                        case 0: {
                            m_carSettings.Bytes[0] = 1;
                        } break;
                        case 0b0001:
                        default: {
                            m_carSettings.Bytes[0] = 2;
                        } break;
                    }

                    m_ccSettingsChanged = false;
                    m_hudSerial->sendCustomCommand(m_carSettings);
                }
                if (!m_ccLoaded) {
                    m_ccLoaded = true;
                }
            } break;
            case Car_Settings_Receive :
            {
                if (message.buf[0] & 0b00000001) {
                    m_carSettings.Bits[1] = (message.buf[1] & 0b00000001);        // Lock confirm lights
                    m_carSettings.Bits[2] = (message.buf[1] & 0b00000010);        // unlock confirm lights
                    m_carSettings.Bits[3] = (message.buf[1] & 0b00000100);        // doors auto lock
                    m_carSettings.Bits[4] = (message.buf[1] & 0b10000000);        // doors unlock driver, then all
                    m_carSettings.Bytes[1] = (message.buf[1] & 0b00011000) >> 3;  // Approach light
                    m_carSettings.Bytes[2] = (message.buf[1] & 0b01100000) >> 5;  // Home safe light
                    m_carSettingsChanged = false;
                    m_hudSerial->sendCustomCommand(m_carSettings);
                }
            } break;
            case Environment_Sensor : {
                bool nightLight = (message.buf[5] &0b00111111) < 0x10;

                bool change = false;

                if(m_bodyControlFrame.NightLight != nightLight) {
                    m_bodyControlFrame.NightLight = nightLight;
                    change = true;
                }
                if(change){
                    m_hudSerial->sendBodyControlCommand(m_bodyControlFrame);
                }
            } break;
            case Indicators : {
                bool leftIndex = (message.buf[1] &0b00001000);
                bool rightIndex = (message.buf[1] &0b00010000);
                bool reverse = (message.buf[0] &0b00001000);

                bool change = false;

                if(m_bodyControlFrame.IndicatorLeft != leftIndex) {
                    m_bodyControlFrame.IndicatorLeft = leftIndex;
                    change = true;
                }
                if(m_bodyControlFrame.IndicatorRight != rightIndex) {
                    m_bodyControlFrame.IndicatorRight = rightIndex;
                    change = true;
                }
                if(m_bodyControlFrame.Reversing != reverse) {
                    m_bodyControlFrame.Reversing = reverse;
                    change = true;
                }
                if(change){
                    m_hudSerial->sendBodyControlCommand(m_bodyControlFrame);
                }
            } break;
            case KeyPosition : {
                uint8_t position = message.buf[5] & 0b00001111;
                switch(position) {
                    case 0x01 : //Key out
                    break;
                    case 0x02 : //Key in, pos 0
                    break;
                    case 0x04 : //Key in, pos 1
                    break;
                    case 0x06 : //Key in, pos 2
                    break;
                    case 0x07 : //Key in, pos 2, engine running
                    break;
                    case 0x08 : //Key in, pos 3, engine cranking
                    break;
                }

            } break;
            default:
                break;
        }
    } else if(bus == Bus_Lin0) {
        if(message.id == 0x20 && message.len >= 3 ) {
            if(message.buf[1] == 0x80) {
                m_hudSerial->sendButtonInputCommand(Key_VolumeDown);
            } else if(message.buf[2] == 0x01) {
                m_hudSerial->sendButtonInputCommand(Key_VolumeUp);                    
            } else if(message.buf[1] == 0x10) {
                m_hudSerial->sendButtonInputCommand(Key_Next);
            } else if(message.buf[1] == 0x02) {
                m_hudSerial->sendButtonInputCommand(Key_Previous);
            } else if(message.buf[1] == 0x01) {
                m_hudSerial->sendButtonInputCommand(Key_Back);
            } else if(message.buf[1] == 0x08) {
                m_hudSerial->sendButtonInputCommand(Key_Enter);
            } else if(message.buf[1] == 0x01) {
                m_hudSerial->sendButtonInputCommand(Key_Up);
            } else if(message.buf[1] == 0x02) {
                m_hudSerial->sendButtonInputCommand(Key_Down);
            } else if(message.buf[1] == 0x04) {
                m_hudSerial->sendButtonInputCommand(Key_Left);
            } else if(message.buf[1] == 0x08) {
                m_hudSerial->sendButtonInputCommand(Key_Right);
            }
        }
    }
}

void Platform::receiveCustomCommand(const CustomCommandFrame &commandFrame) {
    memcpy(&m_carSettingsReceived, &commandFrame, sizeof(CustomCommandFrame));
    m_ccSettingsChanged = true;
    m_carSettingsChanged = true;
    if (m_carSettingsReceived.Bytes[0] >= 3) {
        m_carSettingsReceived.Bytes[0] = 2;
    }
    if (m_carSettingsReceived.Bytes[1] >= 3) {
        m_carSettingsReceived.Bytes[1] = 2;
    }
    if (m_carSettingsReceived.Bytes[2] >= 3) {
        m_carSettingsReceived.Bytes[2] = 2;
    }
    sendCarSettings();
}

#endif