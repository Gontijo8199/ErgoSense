#include "firebase_task.h"

#include "../database/firebase_queue.h"

void firebaseTask(void *pvParameters)
{
    while (true)
    {
        processL4CDQueue();
        processL5CXQueue();

        vTaskDelay(10);
    }
}