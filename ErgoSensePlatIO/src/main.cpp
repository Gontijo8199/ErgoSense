#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>

#include "utils/mux.h"
#include "sensors/vl53l5cx.h"
#include "sensors/vl53l4cd.h"

#include "database/firebase.h"
#include "database/credentials.h"
#include "utils/time_sync.h"

#include "processing/sensor_processor.h"
#include "database/firebase_queue.h"

#include "tasks/sensor_task.h"
#include "tasks/firebase_task.h"

#include "config/system_config.h"
#include "esp_task_wdt.h"


#define TCA1_ADDR 0x70
#define TCA2_ADDR 0x71


#define SENSOR1_RST     13
#define SENSOR1_ADDR    0x45
#define SENSOR1_CHANNEL 0

#define SENSOR2_RST     14
#define SENSOR2_ADDR    0x46
#define SENSOR2_CHANNEL 1


#define SENSOR3_SHUT     25
#define SENSOR3_ADDR     0x47
#define SENSOR3_CHANNEL  0

#define SENSOR4_SHUT     26
#define SENSOR4_ADDR     0x48
#define SENSOR4_CHANNEL  1

#define SENSOR5_SHUT     32
#define SENSOR5_ADDR     0x49
#define SENSOR5_CHANNEL  2

#define SENSOR6_SHUT     33
#define SENSOR6_ADDR     0x50
#define SENSOR6_CHANNEL  3



SparkFun_VL53L5CX Sensor1;
VL53L5CX_ResultsData Data1;

SparkFun_VL53L5CX Sensor2;
VL53L5CX_ResultsData Data2;


SensorConfig_VL53L5CX config_VL53L5CX[] =
{
    { &Sensor1, &Data1, SENSOR1_RST, SENSOR1_CHANNEL, TCA1_ADDR, SENSOR1_ADDR },
    { &Sensor2, &Data2, SENSOR2_RST, SENSOR2_CHANNEL, TCA1_ADDR, SENSOR2_ADDR }
};


VL53L4CD Sensor3(&Wire, SENSOR3_SHUT);
VL53L4CD Sensor4(&Wire, SENSOR4_SHUT);
VL53L4CD Sensor5(&Wire, SENSOR5_SHUT);
VL53L4CD Sensor6(&Wire, SENSOR6_SHUT);

SensorConfig_VL53L4CD config_VL53L4CD[] =
{
    { &Sensor3, SENSOR3_SHUT, SENSOR3_CHANNEL, TCA2_ADDR, SENSOR3_ADDR, 0 },
    { &Sensor4, SENSOR4_SHUT, SENSOR4_CHANNEL, TCA2_ADDR, SENSOR4_ADDR, 0 },
    { &Sensor5, SENSOR5_SHUT, SENSOR5_CHANNEL, TCA2_ADDR, SENSOR5_ADDR, 0 },
    { &Sensor6, SENSOR6_SHUT, SENSOR6_CHANNEL, TCA2_ADDR, SENSOR6_ADDR, 0 }
};


void setup()
{
    Serial.begin(115200);

    Wire.begin(21,22);
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

    if(firebaseInit())
        Serial.println("Firebase OK");
    else
        Serial.println("Firebase ERRO");

    if(firebasePing())
        Serial.println("Firebase ping OK");
    else
        Serial.println("Firebase ping falhou");

    ResetMUX(TCA_RST);

    Serial.println("Inicializando VL53L5CX");

    for(auto &cfg : config_VL53L5CX)
        InitSensor_VL53L5CX(cfg, DEFAULT_RES, DEFAULT_FREQ);

    delay(500);

    Serial.println("Inicializando VL53L4CD");

    for(auto &cfg : config_VL53L4CD)
        InitSensor_VL53L4CD(cfg);

    delay(500);

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