#include "vl53l5cx.h"
#include "time_sync.h"

void InternalReset_L5CX(uint8_t rstPin) {

    pinMode(rstPin, OUTPUT);
    digitalWrite(rstPin, LOW);
    delay(10);
    digitalWrite(rstPin, HIGH);
    delay(10);
    digitalWrite(rstPin, LOW);
}


void InitSensor_VL53L5CX(SensorConfig_VL53L5CX &cfg, int defaultRes, int defaultFreq) {

    Serial.print("[L5CX ");
    Serial.print(cfg.mux_addr, HEX);
    Serial.print(":");
    Serial.print(cfg.channel);
    Serial.print("] ");

    SelectChannel(cfg.mux_addr, cfg.channel);

    InternalReset_L5CX(cfg.reset_pin);
    delay(50);

    bool ok = false;

    if (cfg.sensor->begin(0x29)) {
        cfg.sensor->setAddress(cfg.address);
        ok = true;
    }
    else if (cfg.sensor->begin(cfg.address)) {
        ok = true;
    }

    if (!ok) {
        Serial.println("ERR");
        return;
    }

    cfg.sensor->setRangingFrequency(defaultFreq);
    cfg.sensor->setResolution(defaultRes);
    cfg.sensor->startRanging();

    cfg.resolution = cfg.sensor->getResolution();
    cfg.width = sqrt(cfg.resolution);

    Serial.print("0x");
    Serial.println(cfg.address, HEX);
}


bool ReadSensor_VL53L5CX_Data(SensorConfig_VL53L5CX &cfg, VL53L5CX_Data &out)
{
    SelectChannel(cfg.mux_addr, cfg.channel);

    if (!cfg.sensor->isDataReady())
        return false;

    if (!cfg.sensor->getRangingData(cfg.data))
        return false;

    int total = cfg.width * cfg.width;

    out.address = cfg.address;
    out.timestamp = getEpoch();
    out.width = cfg.width;

    for(int i = 0; i < total; i++)
        out.matrix[i] = cfg.data->distance_mm[i];

    return true;
}