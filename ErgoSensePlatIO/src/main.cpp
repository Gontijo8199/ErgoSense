#include <Arduino.h>
#include <Wire.h>

#include "utils/mux.h"
#include "sensors/vl53l5cx.h"
#include "sensors/vl53l4cd.h"

// MUX

#define TCA1_ADDR       0x70
#define TCA2_ADDR       0x71
#define TCA_RST         12

// VL53L5CX
#define SENSOR1_RST     13
#define SENSOR1_ADDR    0x45
#define SENSOR1_CHANNEL 0

#define SENSOR2_RST     14
#define SENSOR2_ADDR    0x46
#define SENSOR2_CHANNEL 1

#define DEFAULT_RES     64
#define DEFAULT_FREQ    15

// VL53L4CD
#define SENSOR3_SHUT    15
#define SENSOR3_ADDR    0x47
#define SENSOR3_CHANNEL 0

#define SENSOR4_SHUT    18
#define SENSOR4_ADDR    0x48
#define SENSOR4_CHANNEL 1




// VL53L5CX
SparkFun_VL53L5CX Sensor1;
VL53L5CX_ResultsData Data1;

SparkFun_VL53L5CX Sensor2;
VL53L5CX_ResultsData Data2;

SensorConfig_VL53L5CX config_VL53L5CX[] = {
    { &Sensor1, &Data1, SENSOR1_RST, SENSOR1_CHANNEL, TCA1_ADDR, SENSOR1_ADDR },
    { &Sensor2, &Data2, SENSOR2_RST, SENSOR2_CHANNEL, TCA1_ADDR, SENSOR2_ADDR }
};

// VL53L4CD
VL53L4CD Sensor3(&Wire, SENSOR3_SHUT);
VL53L4CD Sensor4(&Wire, SENSOR4_SHUT);

SensorConfig_VL53L4CD config_VL53L4CD[] = {
    { &Sensor3, SENSOR3_SHUT, SENSOR3_CHANNEL, TCA2_ADDR, SENSOR3_ADDR },
    { &Sensor4, SENSOR4_SHUT, SENSOR4_CHANNEL, TCA2_ADDR, SENSOR4_ADDR }
};




void setup() {
    Serial.begin(115200);
    Wire.begin();
    Wire.setClock(400000);

    ResetMUX(TCA_RST);

    for (auto &cfg : config_VL53L5CX)
        InitSensor_VL53L5CX(cfg, DEFAULT_RES, DEFAULT_FREQ);

    for (auto &cfg : config_VL53L4CD)
        InitSensor_VL53L4CD(cfg);

    Serial.println();
}


void loop() {
    for (auto &cfg : config_VL53L5CX)
        ReadSensor_VL53L5CX(cfg);

    for (auto &cfg : config_VL53L4CD)
        ReadSensor_VL53L4CD(cfg);

    delay(200);
}
