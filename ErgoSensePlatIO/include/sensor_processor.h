#pragma once

#include <Arduino.h>
#include "vl53l4cd.h"
#include "vl53l5cx.h"

extern const bool SERIAL_VERBOSITY;

void processL4CD(const VL53L4CD_Data &data);
void processL5CX(const VL53L5CX_Data &data);
