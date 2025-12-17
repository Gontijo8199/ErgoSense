#include "vl53l5cx.h"

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
    Serial.print(cfg.muxAddr, HEX);
    Serial.print(":");
    Serial.print(cfg.muxChannel);
    Serial.print("] ");

    SelectChannel(cfg.muxAddr, cfg.muxChannel);
    InternalReset_L5CX(cfg.rstPin);
    delay(50);

    bool ok = false;

    if (cfg.sensor->begin(0x29)) {
        cfg.sensor->setAddress(cfg.addr);
        ok = true;
    } else if (cfg.sensor->begin(cfg.addr)) {
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
    Serial.println(cfg.addr, HEX);
}


void ReadSensor_VL53L5CX(SensorConfig_VL53L5CX &cfg) {
    SelectChannel(cfg.muxAddr, cfg.muxChannel);

    if (!cfg.sensor->isDataReady()) return;

    if (!cfg.sensor->getRangingData(cfg.data)) return;

    int w = cfg.width;

    for (int y = 0; y < w * w; y += w) {
        for (int x = 0; x < w; x++) {
            Serial.print(cfg.data->distance_mm[y + x]);
            Serial.print("\t");
        }
        Serial.println();
    }
    Serial.println();
}
