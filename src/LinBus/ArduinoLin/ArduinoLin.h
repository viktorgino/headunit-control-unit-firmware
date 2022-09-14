#pragma once
#include <stdint.h>
#include <Arduino.h>
#include <HardwareSerial.h>
#include "hal/hal.h"
#include "../LinBus.h"

#if defined(__IMXRT1062__)
typedef enum LinReaderState {
  FrameFinished,
  SyncField,
  Identifier,
  Data,
  Checksum,
  InterFrame,
} LinReaderState;

class LinBus : public LinBusAbstract{
public:
    explicit LinBus(HardwareSerial serialPort, LinNodeType type, LinMessageCallback callback);
    void setBaudRate(uint32_t speed) override;
    uint32_t getBaudRate() override;
    int sendFrame(LinMessage &frame) override;
    void loop();
private:
    uint8_t dataChecksum (uint8_t* message, int length, uint8_t id = 0);
    int addressParity(uint8_t PID);
    bool checkAddressParity(uint8_t PID);

    bool validateMessage();
    void errorMessage(const char * message, bool dumpMessage = false);
    void transmit();
    void receive();
    HardwareSerial *serialDevice;
    LinMessage *currentMessage;
    LinMessage receivedMessage;
    LinReaderState transmitState;
    LinReaderState receiveState;
    elapsedMicros interFrameTimer;
    uint32_t m_interFrameTime;

    uint32_t m_baudRate;
};
#endif