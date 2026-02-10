#include "firebase_queue.h"
#include "firebase_adapter.h"

static L4CD_Queue queue;
static L5CX_Queue queue5;

void queueL4CD(const VL53L4CD_Data &data) {

    if (queue.count >= L4CD_QUEUE_SIZE) {
        Serial.println("Fila L4CD cheia - descartando dado");
        return;
    }

    queue.buffer[queue.tail] = data;
    queue.tail = (queue.tail + 1) % L4CD_QUEUE_SIZE;
    queue.count++;
}

void queueL5CX(const VL53L5CX_Data &data) {

    //Serial.print("[QUEUE] Recebendo L5CX addr: 0x");
    //Serial.println(data.address, HEX);

    if (queue5.count >= L5CX_QUEUE_SIZE) {
        Serial.println("Fila L5CX cheia - descartando dado");
        return;
    }

    queue5.buffer[queue5.tail] = data;
    queue5.tail = (queue5.tail + 1) % L5CX_QUEUE_SIZE;
    queue5.count++;
}

void processL4CDQueue() {

    while (queue.count > 0) {

        VL53L4CD_Data data = queue.buffer[queue.head];

        //Serial.print("[PROCESS] Enviando L4CD addr: 0x");
        //Serial.println(data.address, HEX);

        sendL4CDtoFirebase(data);

        queue.head = (queue.head + 1) % L4CD_QUEUE_SIZE;
        queue.count--;
    }
}

void processL5CXQueue() {

    static unsigned long lastSend = 0;

    while (queue5.count > 0) {

        if (millis() - lastSend < 300)
            return;

        VL53L5CX_Data data = queue5.buffer[queue5.head];

        //Serial.print("[PROCESS] Enviando L5CX addr: 0x");
        //Serial.println(data.address, HEX);

        bool ok = sendL5CXtoFirebase(data);

        Serial.print("[RESULT] ");
        Serial.println(ok ? "OK" : "FALHOU");

        if (!ok)
            break;

        lastSend = millis();

        queue5.head = (queue5.head + 1) % L5CX_QUEUE_SIZE;
        queue5.count--;
    }
}
