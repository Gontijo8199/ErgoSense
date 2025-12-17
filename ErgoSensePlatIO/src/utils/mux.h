#pragma once
#include <Arduino.h>
#include <Wire.h>

void SelectChannel(uint8_t muxAddr, uint8_t channel);
void ResetMUX(uint8_t rstPin);   
