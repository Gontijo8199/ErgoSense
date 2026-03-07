#include "firebase_queue.h"
#include "firebase_adapter.h"

static L4CD_Queue queue;
static L5CX_Queue queue5;

void queueL4CD(const VL53L4CD_Data &data) {

    if (queue.count >= L4CD_QUEUE_SIZE) {

        // sobrescreve o dado mais antigo
        queue.head = (queue.head + 1) % L4CD_QUEUE_SIZE;
        queue.count--;

        Serial.println("Fila L4CD cheia - sobrescrevendo dado antigo");
    }

    queue.buffer[queue.tail] = data;
    queue.tail = (queue.tail + 1) % L4CD_QUEUE_SIZE;
    queue.count++;
}

void queueL5CX(const VL53L5CX_Data &data) {

    if (queue5.count >= L5CX_QUEUE_SIZE) {

        // sobrescreve o mais antigo
        queue5.head = (queue5.head + 1) % L5CX_QUEUE_SIZE;
        queue5.count--;

        Serial.println("Fila L5CX cheia - sobrescrevendo dado antigo");
    }

    queue5.buffer[queue5.tail] = data;
    queue5.tail = (queue5.tail + 1) % L5CX_QUEUE_SIZE;
    queue5.count++;
}

void processL4CDQueue()
{
    static unsigned long lastSend = 0;

    if (queue.count == 0)
        return;

    if (millis() - lastSend < 200)
        return;

    L4CD_Batch batch;
    batch.count = 0;

    while (queue.count > 0 && batch.count < L4CD_BATCH_SIZE)
    {
        batch.items[batch.count] = queue.buffer[queue.head];

        queue.head = (queue.head + 1) % L4CD_QUEUE_SIZE;
        queue.count--;

        batch.count++;
    }

    bool ok = sendL4CDBatchToFirebase(batch);

    if (ok)
        lastSend = millis();
}

void processL5CXQueue() {

    static unsigned long lastSend = 0;

    if (queue5.count == 0)
        return;

    if (millis() - lastSend < 500)
        return;

    L5CX_Batch batch;

    while (queue5.count > 0 && batch.count < L5CX_BATCH_SIZE) {

        batch.items[batch.count] = queue5.buffer[queue5.head];

        queue5.head = (queue5.head + 1) % L5CX_QUEUE_SIZE;
        queue5.count--;

        batch.count++;
    }

    bool ok = sendL5CXBatchToFirebase(batch);

    if (!ok)
        return;

    lastSend = millis();
}
