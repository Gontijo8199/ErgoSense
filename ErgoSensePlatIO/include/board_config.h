#pragma once

#include "vl53l4cd.h"
#include "vl53l5cx.h"

#define TCA1_ADDR 0x70
#define TCA2_ADDR 0x71

extern SensorConfig_VL53L5CX config_VL53L5CX[];
extern SensorConfig_VL53L4CD config_VL53L4CD[];

// derivados do tamanho dos arrays, ver board_config.cpp
extern const int NUM_L5CX;
extern const int NUM_L4CD;

void initBoardSensors();
