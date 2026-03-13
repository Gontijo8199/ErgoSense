#include "firebase_adapter.h"
#include "firebase.h"
#include "../utils/time_sync.h"

bool sendL4CDBatchToFirebase(const L4CD_Batch &batch)
{
    static FirebaseJson root;
    static FirebaseJsonArray data;

    root.clear();
    data.clear();

    unsigned long ts = getEpoch();

    for (int i = 0; i < batch.count; i++)
    {
        FirebaseJson obj;

        obj.set("addr", batch.items[i].address);
        obj.set("dist", batch.items[i].distance_mm);

        data.add(obj);
    }

    root.set("timestamp", ts);
    root.set("data", data);


    return Firebase.RTDB.pushJSON(&fbdo, "/devices/ergosense0/l4cd_batch", &root);
}

bool sendL5CXBatchToFirebase(const L5CX_Batch &batch)
{
    static FirebaseJson root;
    static FirebaseJsonArray frames;

    root.clear();
    frames.clear();

    root.set("timestamp", getEpoch());

    for (int i = 0; i < batch.count; i++)
    {
        FirebaseJson frame;
        FirebaseJsonArray pixels;

        frame.set("addr", batch.items[i].address);

        for (int j = 0; j < 64; j++)
        {
            pixels.add(batch.items[i].matrix[j]);
        }

        frame.set("pixels", pixels);

        frames.add(frame);
    }

    root.set("frames", frames);

    return Firebase.RTDB.pushJSON(&fbdo, "/devices/ergosense0/l5cx_batch", &root);
}
