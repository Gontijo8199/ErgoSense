#pragma once

#include <Arduino.h>
#include "../sensors/vl53l4cd.h"
#include "../sensors/vl53l5cx.h"

bool sendL4CDtoFirebase(const VL53L4CD_Data &data);
bool sendL5CXtoFirebase(const VL53L5CX_Data &data);
