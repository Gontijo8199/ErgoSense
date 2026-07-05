#include "sensor_task.h"

#include <Arduino.h>
#include <Wire.h>

#include "vl53l5cx.h"
#include "vl53l4cd.h"
#include "sensor_processor.h"
#include "mux.h"

#include "board_config.h"
#include "system_config.h"
#include "esp_task_wdt.h"

#define SENSOR_FAIL_LIMIT 10

static int l5_fail[8] = {0};
static int l4_fail[8] = {0};


void resetI2C()
{
    Serial.println("[I2C] Resetando barramento...");

    Wire.end();
    delay(50);

    Wire.begin(21,22);
    Wire.setClock(40000);
    Wire.setTimeout(20);
}


void sensorTask(void *pvParameters)
{
    esp_task_wdt_add(NULL);

    VL53L5CX_Data d5;
    VL53L4CD_Data d4;

    Serial.println("[SensorTask] iniciado no Core 1");

    while (true)
    {
        int fail_total = 0;
        esp_task_wdt_reset();

        for (int i = 0; i < NUM_L5CX; i++)
        {
            if (ReadSensor_VL53L5CX_Data(config_VL53L5CX[i], d5))
            {
                l5_fail[i] = 0;
                processL5CX(d5);
            }
            else
            {
                l5_fail[i]++;
                fail_total++;
                if (l5_fail[i] > SENSOR_FAIL_LIMIT)
                {
                    Serial.print("[SensorTask] Reiniciando VL53L5CX ");
                    Serial.println(i);

                    // ResetMUX(TCA_RST);

                    InitSensor_VL53L5CX(
                        config_VL53L5CX[i],
                        DEFAULT_RES,
                        DEFAULT_FREQ
                    );

                    l5_fail[i] = 0;
                }
            }
        }


        for (int i = 0; i < NUM_L4CD; i++)
        {
            if (ReadSensor_VL53L4CD_Data(config_VL53L4CD[i], d4))
            {
                l4_fail[i] = 0;
                processL4CD(d4);
            }
            else
            {
                l4_fail[i]++;
                fail_total++;
                if (l4_fail[i] > SENSOR_FAIL_LIMIT)
                {
                    Serial.print("[SensorTask] Reiniciando VL53L4CD ");
                    Serial.println(i);

                    // ResetMUX(TCA_RST);

                    InitSensor_VL53L4CD(config_VL53L4CD[i]);

                    l4_fail[i] = 0;
                }
            }
        }

        if(fail_total > 3){
            Serial.println("[SensorTask] Possível travamento I2C");
            resetI2C();
            ResetMUX(TCA_RST);
        }

        // pequeno yield para o scheduler
        vTaskDelay(pdMS_TO_TICKS(10));  
    }
}