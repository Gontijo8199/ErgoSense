#pragma once
#include <Arduino.h>
#include <SparkFun_VL53L5CX_Library.h>
#include "../utils/mux.h"

struct SensorConfig_VL53L5CX {
    SparkFun_VL53L5CX* sensor;
    VL53L5CX_ResultsData* data;
    uint8_t rstPin;
    uint8_t muxChannel;
    uint8_t muxAddr;
    uint8_t addr;
    int resolution;
    int width;
};

struct VL53L5CX_Data {
    uint8_t address;
    int width;
    unsigned long timestamp;
    uint16_t matrix[64];
};


void InternalReset_L5CX(uint8_t rstPin);
void InitSensor_VL53L5CX(SensorConfig_VL53L5CX &cfg, int defaultRes, int defaultFreq);
void ReadSensor_VL53L5CX(SensorConfig_VL53L5CX &cfg);
bool ReadSensor_VL53L5CX_Data(SensorConfig_VL53L5CX &cfg, VL53L5CX_Data &out);