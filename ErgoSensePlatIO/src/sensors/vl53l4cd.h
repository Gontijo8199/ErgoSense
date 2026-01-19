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

struct VL53L4CD_Data {
    uint8_t address;
    uint16_t distance_mm;
    unsigned long timestamp;
};

void InitSensor_VL53L4CD(SensorConfig_VL53L4CD &cfg);
void ReadSensor_VL53L4CD(SensorConfig_VL53L4CD &cfg);
bool ReadSensor_VL53L4CD_Data(SensorConfig_VL53L4CD &cfg, VL53L4CD_Data &out);
