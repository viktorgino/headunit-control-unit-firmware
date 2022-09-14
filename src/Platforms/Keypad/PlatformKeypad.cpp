#include "PlatformKeypad.h"

#if PLATFORM == KEYPAD_PLATFORM

Platform::Platform(HUDSerial::HUDSerial *hudSerial) : PlatformAbstract(hudSerial)
{
}
void Platform::loop()
{
    switch (timerCount++)
    {
    case 0:
        for (uint8_t x = 0; x < COLUMNS; x++)
        {
            pinMode(COLUMN_PINS[x], OUTPUT);
            digitalWrite(COLUMN_PINS[x], LOW);
            for (byte y = 0; y < ROWS; y++)
            {
                gpioPreviousState[x][y] = gpioState[x][y];
                gpioState[x][y] = !digitalRead(ROW_PINS[y]);
            }

            digitalWrite(COLUMN_PINS[x], HIGH);
            pinMode(COLUMN_PINS[x], INPUT);
        }
        break;
    case 5:
        for (byte y = 0; y < ROWS; y++)
        {
            for (uint8_t x = 0; x < COLUMNS; x++)
            {
                keyPrevState[x][y] = keyState[x][y];
                keyState[x][y] = gpioPreviousState[x][y] & gpioState[x][y];
                if (keyState[x][y] > keyPrevState[x][y])
                {
                    uint8_t pos = x * ROWS + y;
                    switch (pos)
                    {
                    case 7:
                        m_hudSerial->sendButtonInputCommand(Key_TuneDown);
                        break;
                    case 15:
                        m_hudSerial->sendButtonInputCommand(Key_TuneUp);
                        break;
                    case 23:
                        m_hudSerial->sendButtonInputCommand(Key_Sound);
                        break;
                    case 6:
                        m_hudSerial->sendButtonInputCommand(Key_VolumeDown);
                        break;
                    case 14:
                        m_hudSerial->sendButtonInputCommand(Key_VolumeUp);
                        break;
                    case 5:
                    case 13:
                    {
                        ClimateControlZoneControl *zone;
                        uint8_t *parameter = nullptr;
                        int8_t direction = -1;
                        uint8_t offset = 0;

                        if (climateControlState < 4)
                        {
                            zone = &controlFrame.Front.Left;
                        }
                        else if (climateControlState < 8)
                        {
                            zone = &controlFrame.Front.Right;
                            offset = 4;
                        }
                        else if (climateControlState < 12)
                        {
                            zone = &controlFrame.Rear.Right;
                            offset = 8;
                        }
                        else if (climateControlState < 16)
                        {
                            zone = &controlFrame.Rear.Right;
                            offset = 12;
                        }
                        else
                        {
                            return;
                        }

                        switch (climateControlState - offset)
                        {
                        case 0:
                            parameter = &zone->Fan;
                            break;
                        case 1:
                            parameter = &zone->Temperature;
                            break;
                        case 2:
                            parameter = &zone->SeatHeating;
                            break;
                        case 3:
                            direction = zone->Direction.Auto | (zone->Direction.Down << 1) | (zone->Direction.Center << 2) | (zone->Direction.Up << 3);
                            break;
                        default:
                            return;
                        }

                        if(parameter){
                            if (pos == 5)
                            {
                                (*parameter)--;
                            }
                            else
                            {
                                (*parameter)++;
                            }
                        }
                        if(direction >= 0){
                            if (pos == 5)
                            {
                                direction--;
                            }
                            else
                            {
                                direction++;
                            }
                            zone->Direction.Auto = direction & 0b00000001;
                            zone->Direction.Down = direction & 0b00000010;
                            zone->Direction.Center = direction & 0b00000100;
                            zone->Direction.Up = direction & 0b00001000;
                        }
                        m_hudSerial->sendClimateControlCommand(controlFrame);
                    }
                    break;
                    case 21:
                        climateControlState++;
                        if (climateControlState >= 8)
                        {
                            climateControlState = 0;
                        }
                        break;
                    case 4:
                        m_hudSerial->sendButtonInputCommand(Key_Previous);
                        break;
                    case 12:
                        m_hudSerial->sendButtonInputCommand(Key_Next);
                        break;
                    default :
                        SERIAL_DEBUG.printf("Key pressed : %d\n", x * ROWS + y);
                        break;
                    }
                }
            }
        }
        break;
    default:
        break;
    }
    if (timerCount >= 10)
    {
        timerCount = 0;
    }
}

void Platform::setup()
{
    for (int y = 0; y < ROWS; y++)
    {
        pinMode(ROW_PINS[y], INPUT_PULLUP);
    }
}

void Platform::receiveBusMessage(BusNumber bus, BusMessage message)
{
}

void Platform::receiveCustomCommand(const CustomCommandFrame &commandFrame)
{
}
void Platform::receiveClimateControlCommand(const ClimateControlCommandFrame &climateControlFrame)
{
}
void Platform::receiveBodyControlCommand(const BodyControlCommandFrame &commandFrame)
{
}
void Platform::receiveDriveTrainControlCommand(const DriveTrainControlCommandFrame &commandFrame)
{
}
#endif
