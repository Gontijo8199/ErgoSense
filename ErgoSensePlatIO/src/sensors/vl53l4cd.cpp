#include "vl53l4cd.h"
#include "time_sync.h"

void InitSensor_VL53L4CD(SensorConfig_VL53L4CD &cfg) {

    Serial.print("[L4CD ");
    Serial.print(cfg.mux_addr, HEX);
    Serial.print(":");
    Serial.print(cfg.channel);
    Serial.print("] ");

    SelectChannel(cfg.mux_addr, cfg.channel);

    pinMode(cfg.shut_pin, OUTPUT);
    digitalWrite(cfg.shut_pin, LOW);
    delay(5);
    digitalWrite(cfg.shut_pin, HIGH);
    delay(10);

    cfg.sensor->begin();

    uint8_t status = cfg.sensor->InitSensor(cfg.address);

    if (status != VL53L4CD_ERROR_NONE) {
        delay(5);
        status = cfg.sensor->InitSensor(cfg.address);
    }

    if (status != VL53L4CD_ERROR_NONE) {
        Serial.println("ERR");
        return;
    }

    cfg.sensor->VL53L4CD_StartRanging();

    Serial.print("0x");
    Serial.println(cfg.address, HEX);
}


bool ReadSensor_VL53L4CD_Data(SensorConfig_VL53L4CD &cfg, VL53L4CD_Data &out) {

    SelectChannel(cfg.mux_addr, cfg.channel);

    uint8_t ready;
    VL53L4CD_Result_t result;

    cfg.sensor->VL53L4CD_CheckForDataReady(&ready);

    if (!ready)
        return false;

    cfg.sensor->VL53L4CD_GetResult(&result);
    cfg.sensor->VL53L4CD_ClearInterrupt();

    out.address = cfg.address;
    out.distance_mm = result.distance_mm;
    out.timestamp = getEpoch();



    return true;
}