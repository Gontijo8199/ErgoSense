#include "mux.h"

void SelectChannel(uint8_t muxAddr, uint8_t channel) {
    Wire.beginTransmission(muxAddr);
    Wire.write(1 << channel);
    Wire.endTransmission();
}

void ResetMUX(uint8_t rstPin) {
    pinMode(rstPin, OUTPUT);
    digitalWrite(rstPin, LOW);
    delay(5);
    digitalWrite(rstPin, HIGH);
    delay(5);
}
