#include "sensor_processor.h"
#include "../database/firebase_adapter.h"
#include "../database/firebase_queue.h"

void processL4CD(const VL53L4CD_Data &data) {

    Serial.print("[L4CD 0x");
    Serial.print(data.address, HEX);
    Serial.print("] ");
    Serial.print(data.distance_mm);
    Serial.println(" mm");

    queueL4CD(data);
}


void processL5CX(const VL53L5CX_Data &data) {

    Serial.print("[L5CX 0x");
    Serial.print(data.address, HEX);
    Serial.println("]");

    int w = data.width;

    for (int y = 0; y < w * w; y += w) {
        for (int x = 0; x < w; x++) {
            Serial.print(data.matrix[y + x]);
            Serial.print("\t");
        }
        Serial.println();
    }
    Serial.println();

    queueL5CX(data);
}
