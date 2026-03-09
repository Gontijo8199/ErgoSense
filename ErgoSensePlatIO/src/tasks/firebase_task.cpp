#include "firebase_task.h"

#include "../database/firebase_queue.h"
#include "esp_task_wdt.h"

void firebaseTask(void *pvParameters)
{
    while (true)
    {
        esp_task_wdt_reset();
        
        processL4CDQueue();
        processL5CXQueue();

        vTaskDelay(pdMS_TO_TICKS(10));  
    }
}