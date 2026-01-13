#include "vl53l4cd.h"

void InitSensor_VL53L4CD(SensorConfig_VL53L4CD &cfg) {
    Serial.print("[L4CD ");
    Serial.print(cfg.muxAddr, HEX);
    Serial.print(":");
    Serial.print(cfg.muxChannel);
    Serial.print("] ");

    SelectChannel(cfg.muxAddr, cfg.muxChannel);

    pinMode(cfg.shutPin, OUTPUT);
    digitalWrite(cfg.shutPin, LOW);
    delay(5);
    digitalWrite(cfg.shutPin, HIGH);
    delay(10);

    cfg.sensor->begin();

    uint8_t status = cfg.sensor->InitSensor(cfg.addr);

    if (status != VL53L4CD_ERROR_NONE) {
        delay(5);
        status = cfg.sensor->InitSensor(cfg.addr);
    }

    if (status != VL53L4CD_ERROR_NONE) {
        Serial.println("ERR");
        return;
    }


    cfg.sensor->VL53L4CD_StartRanging();

    Serial.print("0x");
    Serial.println(cfg.addr, HEX);
}

void ReadSensor_VL53L4CD(SensorConfig_VL53L4CD &cfg) {
    SelectChannel(cfg.muxAddr, cfg.muxChannel);

    uint8_t ready;
    VL53L4CD_Result_t result;

    cfg.sensor->VL53L4CD_CheckForDataReady(&ready);

    if (!ready) return;

    cfg.sensor->VL53L4CD_GetResult(&result);
    cfg.sensor->VL53L4CD_ClearInterrupt();

    Serial.print("L4CD: ");
    Serial.println(result.distance_mm);
}
