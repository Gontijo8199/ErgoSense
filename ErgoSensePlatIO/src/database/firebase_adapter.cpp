#include "firebase_adapter.h"
#include "firebase.h"
#include "../utils/time_sync.h"
/*
bool sendL4CDtoFirebase(const VL53L4CD_Data &data) {
    String path = "/devices/sensors/l4cd/0x";
    path += String(data.address, HEX);
    path += "/";
    path += String(data.timestamp);  // timestamp vira chave

    FirebaseJson json;
    json.set("distance_mm", data.distance_mm);

    bool ok = Firebase.RTDB.setJSON(&fbdo, path, &json);

    if (!ok) {
        Serial.print("[Firebase L4CD] Erro: ");
        Serial.println(fbdo.errorReason());
    }

    return ok;
}

bool sendL5CXtoFirebase(const VL53L5CX_Data &data) {
    String path = "/devices/sensors/l5cx/0x";
    path += String(data.address, HEX);
    path += "/";
    path += String(data.timestamp);  // timestamp vira chave

    FirebaseJson json;
    json.set("width", data.width);

    for (int i = 0; i < 64; i++) {
        json.set("matrix/" + String(i), data.matrix[i]);
    }

    bool ok = Firebase.RTDB.setJSON(&fbdo, path, &json);

    if (!ok) {
        Serial.print("[Firebase L5CX] ERRO: ");
        Serial.println(fbdo.errorReason());
    }

    return ok;
}
*/


bool sendL4CDBatchToFirebase(const L4CD_Batch &batch)
{
    static FirebaseJson json;
    static FirebaseJsonArray addrArr;
    static FirebaseJsonArray distArr;

    json.clear();
    addrArr.clear();
    distArr.clear();

    unsigned long ts = getEpoch();

    for (int i = 0; i < batch.count; i++)
    {
        addrArr.add(batch.items[i].address);
        distArr.add(batch.items[i].distance_mm);
    }

    json.set("timestamp", ts);
    json.set("addr", addrArr);
    json.set("dist", distArr);

    char path[64];
    snprintf(path, sizeof(path), "/devices/l4cd_batch/%lu", ts);

    return Firebase.RTDB.setJSON(&fbdo, path, &json);
}

bool sendL5CXBatchToFirebase(const L5CX_Batch &batch) {

    FirebaseJson root;

    root.set("timestamp", getEpoch());

    FirebaseJsonArray frames;

    for (int i = 0; i < batch.count; i++) {

        FirebaseJson frame;

        frame.set("addr", batch.items[i].address);

        FirebaseJsonArray grid;

        for (int j = 0; j < 64; j++) {
            grid.add(batch.items[i].matrix[j]);
        }

        frame.set("grid", grid);

        frames.add(frame);
    }

    root.set("frames", frames);

    return Firebase.RTDB.pushJSON(&fbdo, "/devices/l5cx_batch", &root);
}