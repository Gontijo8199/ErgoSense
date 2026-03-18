#pragma once
#include <Arduino.h>
#include <vl53l4cd_class.h>
#include "mux.h"

struct SensorConfig_VL53L4CD {
    VL53L4CD* sensor;
    int shut_pin;
    int channel;
    uint8_t mux_addr;
    uint8_t address;
    int fail_count;
};
struct VL53L4CD_Data {
    uint8_t address;
    uint16_t distance_mm;
    unsigned long timestamp;
};


void InitSensor_VL53L4CD(SensorConfig_VL53L4CD &cfg);
void ReadSensor_VL53L4CD(SensorConfig_VL53L4CD &cfg);
bool ReadSensor_VL53L4CD_Data(SensorConfig_VL53L4CD &cfg, VL53L4CD_Data &out);
