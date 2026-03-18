#include "sensor_processor.h"
#include "firebase_adapter.h"
#include "firebase_queue.h"


const bool SERIAL_VERBOSITY = false;

void processL4CD(const VL53L4CD_Data &data) {


    if (SERIAL_VERBOSITY) 
    {
        Serial.println();
        Serial.print("[L4CD 0x");
        Serial.print(data.address, HEX);
        Serial.print("] ");
        Serial.print(data.distance_mm);
        Serial.print(" mm");
        Serial.println();
    }

    queueL4CD(data);
}


void processL5CX(const VL53L5CX_Data &data) {

    if (SERIAL_VERBOSITY){
        Serial.println();
        Serial.print("[L5CX 0x");
        Serial.print(data.address, HEX);
        Serial.print("]");
        Serial.println();

        int w = data.width;

        for (int y = 0; y < w * w; y += w) {
            for (int x = 0; x < w; x++) {
                Serial.print(data.matrix[y + x]);
                Serial.print("\t");
            }
            Serial.println();
        }

        Serial.println();
    }
    queueL5CX(data);
}
