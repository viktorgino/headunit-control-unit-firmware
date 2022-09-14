#include "teensy4-hal.h"
#if defined(__IMXRT1062__) /*T4*/ || defined(__MK64FX512__) /*T35_36*/ || defined(__MK66FX1M0__) /*T35_36*/ || defined(__MK20DX256__) /*T31_32*/

#include "teensy4-hal.h"
#include <FastCRC.h>
#include <Arduino.h>

FastCRC16 CRC16;

elapsedMillis loop1msTimer;

namespace HAL
{

void init() {
    Serial.begin(1000000);
    SERIAL_PRIMARY.begin(1000000);
    #ifdef SERIAL_GVRET
        SERIAL_GVRET.begin(1000000);
    #endif
    #ifdef SERIAL_DEBUG
        SERIAL_DEBUG.begin(1000000);
    #endif
}
void loop() {
    if(loop1msTimer > 0){
        TimerExpired = true;
        loop1msTimer = 0;
    }
}
void print(const char *buffer){
    Serial.println(buffer);
}

uint16_t calculateCRC16(char *buffer, int length){
    return CRC16.x25((uint8_t*) buffer, length);    
}
    
bool TimerExpired = false;
} // namespace HAL

#endif