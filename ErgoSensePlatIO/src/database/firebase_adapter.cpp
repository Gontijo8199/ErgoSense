#include "firebase_adapter.h"
#include "firebase.h"

bool sendL4CDtoFirebase(const VL53L4CD_Data &data) {
    String path = "/sensors/l4cd/0x";
    path += String(data.address, HEX);

    FirebaseJson json;

    json.set("distance_mm", data.distance_mm);
    json.set("timestamp", data.timestamp);

    Serial.print("[Firebase L4CD] Enviando para: ");
    Serial.println(path);

    bool ok = Firebase.RTDB.pushJSON(&fbdo, path, &json);

    if (!ok) {
        Serial.print("[Firebase L4CD] Erro: ");
        Serial.println(fbdo.errorReason());
    }

    return ok;
}

bool sendL5CXtoFirebase(const VL53L5CX_Data &data) {
    String path = "/sensors/l5cx/0x";
    path += String(data.address, HEX);

    Serial.print("[Firebase L5CX] Enviando para: ");
    Serial.println(path);

    FirebaseJson json;

    json.set("timestamp", data.timestamp);
    json.set("width", data.width);

    for (int i = 0; i < 64; i++) {
        json.set("matrix/" + String(i), data.matrix[i]);
    }

    bool ok = Firebase.RTDB.pushJSON(&fbdo, path, &json);

    if (!ok) {
        Serial.print("[Firebase L5CX] ERRO: ");
        Serial.println(fbdo.errorReason());
    }

    return ok;
}
