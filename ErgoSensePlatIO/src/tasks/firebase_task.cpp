#include "firebase_task.h"

#include "firebase_queue.h"
#include "esp_task_wdt.h"

void firebaseTask(void *pvParameters)
{
    esp_task_wdt_add(NULL);

    while (true)
    {
        esp_task_wdt_reset();

        processL4CDQueue();
        processL5CXQueue();

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}
