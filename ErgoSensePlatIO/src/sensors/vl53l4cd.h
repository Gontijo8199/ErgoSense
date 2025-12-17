#pragma once
#include <Arduino.h>
#include <vl53l4cd_class.h>
#include "../utils/mux.h"

struct SensorConfig_VL53L4CD {
    VL53L4CD* sensor;
    uint8_t shutPin;
    uint8_t muxChannel;
    uint8_t muxAddr;
    uint8_t addr;
};

void InitSensor_VL53L4CD(SensorConfig_VL53L4CD &cfg);
void ReadSensor_VL53L4CD(SensorConfig_VL53L4CD &cfg);
