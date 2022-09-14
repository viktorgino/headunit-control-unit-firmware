#include "ArduinoLin.h"

#if defined(__IMXRT1062__)
LinBus::LinBus(HardwareSerial serialPort, LinNodeType type, LinMessageCallback callback)
    : LinBusAbstract(serialPort, type, callback)
{
    m_baudRate = 0;
}

void LinBus::setBaudRate(uint32_t speed)
{
    m_interFrameTime = (10000000 / speed) * 2;
    m_baudRate = speed;
    SERIAL_LIN1.begin(speed);
    LPUART2_STAT |= LPUART_STAT_BRK13;
    LPUART2_STAT |= LPUART_STAT_LBKDE;
}

uint32_t LinBus::getBaudRate() { return m_baudRate; }

int LinBus::sendFrame(LinMessage &frame)
{
    if (frame.id > 0x3F)
    {
#ifdef DEBUG
        hudSerial.sendDebugMessageCommand("LIN 0 : Invalid LIN id");
#endif
        return -2;
    }
    if (transmitState != FrameFinished)
    {
        return -1;
    }

    if (currentMessage)
    {
        if (currentMessage->bytesSent != currentMessage->len + 3)
        {
#ifdef DEBUG
            char buffer[MESSAGE_MAX_LENGTH];
            sprintf(buffer, "LIN 0 : Sent data length is invalid, bytes sent: %d len: %d", currentMessage->bytesSent,
                    currentMessage->len);
            hudSerial.sendDebugMessageCommand(buffer);
#endif
        }
        free(currentMessage);
        currentMessage = nullptr;
    }

    currentMessage = (LinMessage *)malloc(sizeof(LinMessage));
    memcpy(currentMessage, &frame, sizeof(LinMessage));
    currentMessage->cursor = 0;
    currentMessage->bytesSent = 0;

    LPUART2_CTRL |= LPUART_CTRL_SBK;
    LPUART2_CTRL &= ~LPUART_CTRL_SBK;

    transmitState = SyncField;

    return 1;
}

void LinBus::transmit()
{
    if (currentMessage != nullptr && SERIAL_LIN1.availableForWrite())
    {
        switch (transmitState)
        {
        case SyncField:
            SERIAL_LIN1.write(0x55);
            currentMessage->bytesSent++;

            transmitState = Identifier;
            break;
        case Identifier:
            SERIAL_LIN1.write(addressParity(currentMessage->id));
            currentMessage->bytesSent++;

            transmitState = Data;
            break;
        case Data:
            SERIAL_LIN1.write(currentMessage->buf[currentMessage->cursor++]);
            currentMessage->bytesSent++;

            if (currentMessage->cursor >= currentMessage->len)
            {
                transmitState = Checksum;
            }
            break;
        case Checksum:
            SERIAL_LIN1.write(
                dataChecksum(currentMessage->buf, currentMessage->len, addressParity(currentMessage->id)));
            currentMessage->bytesSent++;

            interFrameTimer = 0;
            transmitState = InterFrame;
            break;
        case InterFrame:
            if (interFrameTimer > m_interFrameTime)
            {
                transmitState = FrameFinished;
            }
            break;

        default:
            break;
        }
    }
}

bool LinBus::validateMessage()
{
    LPUART2_STAT |= LPUART_STAT_LBKDE;

    if (receivedMessage.len <= 0)
    {
#ifdef DEBUG
        char buffer[MESSAGE_MAX_LENGTH];
        sprintf(buffer, "Emtpy message : %02x", receivedMessage.id);
        errorMessage(buffer);
#endif
        return false;
    }

    uint8_t checksum = dataChecksum(receivedMessage.buf, receivedMessage.len, receivedMessage.id);

    if (!checkAddressParity(receivedMessage.id))
    {
#ifdef DEBUG
        errorMessage("Invalid addr parity", true);
#endif
        return false;
    }

    if (receivedMessage.checksum != checksum)
    {
#ifdef DEBUG
        errorMessage("chcksum err", true);
#endif
        return false;
    }
    receivedMessage.id &= 0b00111111;
    m_callback(receivedMessage);
    return true;
}

void LinBus::receive()
{
    if (((LPUART2_STAT >> 31) & 1))
    // if (LPUART_STAT_LBKDIF & LPUART_STAT_LBKDIF)
    {
        LPUART2_STAT &= ~LPUART_STAT_LBKDE;

        // if (receiveState == Data)
        // {
        //     if (receivedMessage.len > 2)
        //     {
        //         receivedMessage.checksum = receivedMessage.buf[receivedMessage.len - 1];
        //         receivedMessage.len -= 1;
        //         validateMessage();
        //     }
        // }
        receiveState = SyncField;
    }

    switch (receiveState)
    {
    case SyncField:
    {
        if (SERIAL_LIN1.available())
        {
            uint8_t received = SERIAL_LIN1.read();
            if (received == 0x55)
            {
                receiveState = Identifier;
            }
            else if (received == 0)
            {
                receiveState = SyncField;
            }
            else
            {
#ifdef DEBUG
                char buffer[MESSAGE_MAX_LENGTH];
                sprintf(buffer, "Invalid sync frame %02x", received);
                errorMessage(buffer);
#endif
                receiveState = FrameFinished;
            }
        }
    }
    break;
    case Identifier:
    {
        if (SERIAL_LIN1.available())
        {
            uint8_t received = SERIAL_LIN1.read();
            receiveState = Data;
            receivedMessage.id = received;
            receivedMessage.len = 0;
            interFrameTimer = 0;
        }
    }
    break;
    case Data:
    {
        if (interFrameTimer > m_interFrameTime)
        {
            if (receivedMessage.len > 2)
            {
                receivedMessage.checksum = receivedMessage.buf[receivedMessage.len - 1];
                receivedMessage.len -= 1;
                if(!validateMessage()){
                    errorMessage("Data: Invalid checksum", true);                     
                }
            }
            else
            {
#ifdef DEBUG
                errorMessage("Data: Byte Time out", true);
#endif
            }

            receiveState = FrameFinished;
        }
        else if (SERIAL_LIN1.available())
        {
            receivedMessage.buf[receivedMessage.len++] = SERIAL_LIN1.read();
            if (receivedMessage.len >= 8)
            {
                receiveState = Checksum;
            }
            interFrameTimer = 0;
        }
    }
    break;
    case Checksum:
    {
        if (interFrameTimer > m_interFrameTime)
        {
#ifdef DEBUG
            errorMessage("Checksum: Byte Time out", true);
#endif
            receiveState = FrameFinished;
        }
        else if (SERIAL_LIN1.available())
        {
            receivedMessage.checksum = SERIAL_LIN1.read();
            if(!validateMessage()){
                errorMessage("Checksum: Invalid checksum", true);                     
            }
            receiveState = FrameFinished;
        }
    }
    break;

    case FrameFinished :
    break;
    default:
#ifdef DEBUG
        errorMessage("Invalid receiver state");
#endif
        receiveState = FrameFinished;
        break;
    }
    if(receiveState == FrameFinished){
        LPUART2_STAT |= LPUART_STAT_LBKDE;
    }
}

#ifdef DEBUG
void LinBus::errorMessage(const char *message, bool dumpMessage)
{
    char buffer[MESSAGE_MAX_LENGTH];
    if (dumpMessage)
    {
        char bufferHex[32];
        for (uint8_t i = 0; i < receivedMessage.len; i++)
        {
            sprintf(&bufferHex[3 * i], "%02x ", receivedMessage.buf[i]);
        }
        sprintf(buffer, "LIN 0 : %s : %02x (%02x) %s chksum : %02x len : %d",
                message,
                receivedMessage.id,
                (receivedMessage.id & 0b00111111),
                bufferHex,
                receivedMessage.checksum,
                receivedMessage.len);
    }
    else
    {
        sprintf(buffer, "LIN 0 : %s", message);
    }
    hudSerial.sendDebugMessageCommand(buffer);
}
#endif

void LinBus::loop()
{
    transmit();
    if (m_type == LIN_SLAVE)
    {
    }
    receive();
}

bool LinBus::checkAddressParity(uint8_t PID) { return (PID == addressParity(PID & 0b00111111)); }

int LinBus::addressParity(uint8_t PID)
{
    PID |= (((PID >> 0) & 1) ^ ((PID >> 1) & 1) ^ ((PID >> 2) & 1) ^ ((PID >> 4) & 1)) << 6;
    PID |= ~(((PID >> 1) & 1) ^ ((PID >> 3) & 1) ^ ((PID >> 4) & 1) ^ ((PID >> 5) & 1)) << 7;
    return PID;
}

uint8_t LinBus::dataChecksum(uint8_t *message, int length, uint8_t id)
{
    uint32_t ret = 0;
    if (m_enhancedChecksum)
    {
        ret += id;
    }
    for (int i = 0; i < length; i++)
    {
        ret += message[i];
        if (ret > 255)
        {
            ret -= 255;
        }
    }

    return ~ret;
}
#endif