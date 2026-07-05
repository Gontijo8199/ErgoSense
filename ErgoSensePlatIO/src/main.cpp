#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>

#include "board_config.h"
#include "firebase.h"
#include "credentials.h"
#include "time_sync.h"

#include "sensor_task.h"
#include "firebase_task.h"

#include "esp_task_wdt.h"


void setup()
{
    Serial.begin(115200);

    Wire.begin(21, 22);
    Wire.setClock(40000);
    Wire.setTimeout(20);

    Serial.println("Conectando WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi conectado");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    initTime();

    if (firebaseInit())
        Serial.println("Firebase OK");
    else
        Serial.println("Firebase ERRO");

    if (firebasePing())
        Serial.println("Firebase ping OK");
    else
        Serial.println("Firebase ping falhou");

    initBoardSensors();

    esp_task_wdt_init(10, true);
    xTaskCreatePinnedToCore(
        sensorTask,
        "SensorTask",
        8000,
        NULL,
        3,
        NULL,
        1
    );

    xTaskCreatePinnedToCore(
        firebaseTask,
        "FirebaseTask",
        10000,
        NULL,
        1,
        NULL,
        0
    );

    Serial.println("\nTasks iniciadas:");
    Serial.println("SensorTask   -> Core1");
    Serial.println("FirebaseTask -> Core0");
}


void loop()
{
    vTaskDelay(portMAX_DELAY);
}
