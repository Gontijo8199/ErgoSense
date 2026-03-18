#pragma once
#include <Arduino.h>
#include <SparkFun_VL53L5CX_Library.h>
#include "mux.h"

struct SensorConfig_VL53L5CX {

    SparkFun_VL53L5CX *sensor;
    VL53L5CX_ResultsData *data;

    uint8_t reset_pin;
    uint8_t channel;
    uint8_t mux_addr;
    uint8_t address;

    int resolution;
    int width;
};

struct VL53L5CX_Data
{
    uint32_t timestamp;
    uint16_t matrix[64];
    uint8_t address;
    uint8_t width;
};

void InternalReset_L5CX(uint8_t rstPin);
void InitSensor_VL53L5CX(SensorConfig_VL53L5CX &cfg, int defaultRes, int defaultFreq);
void ReadSensor_VL53L5CX(SensorConfig_VL53L5CX &cfg);
bool ReadSensor_VL53L5CX_Data(SensorConfig_VL53L5CX &cfg, VL53L5CX_Data &out);