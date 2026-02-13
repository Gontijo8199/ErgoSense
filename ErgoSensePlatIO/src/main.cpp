/**
 * @file main.cpp
 * @author 
 * @brief Operação de leitura e processamento de dados (ErgoSense)
 * @version 1.0
 * @date 2026-02-11
*/

#include <Arduino.h>
#include <Wire.h>

#include "utils/mux.h"
#include "sensors/vl53l5cx.h"
#include "sensors/vl53l4cd.h"

#include "database/firebase.h"
#include "database/credentials.h"
#include "processing/sensor_processor.h"
#include "database/firebase_queue.h"
#include "utils/time_sync.h"

// MUX
#define TCA1_ADDR           0x70
#define TCA2_ADDR           0x71
#define TCA_RST             12

// VL53L5CX   
#define SENSOR1_RST         13
#define SENSOR1_ADDR        0x45
#define SENSOR1_CHANNEL     0

#define SENSOR2_RST         14
#define SENSOR2_ADDR        0x46
#define SENSOR2_CHANNEL     1

#define DEFAULT_RES         64
#define DEFAULT_FREQ        15

// VL53L4CD   
#define SENSOR3_SHUT        15
#define SENSOR3_ADDR        0x47
#define SENSOR3_CHANNEL     0

#define SENSOR4_SHUT        18 // 16 e 17 são reservados para o ESP
#define SENSOR4_ADDR        0x48
#define SENSOR4_CHANNEL     1

#define SENSOR5_SHUT    19
#define SENSOR5_ADDR    0x49
#define SENSOR5_CHANNEL 2

#define SENSOR6_SHUT    21
#define SENSOR6_ADDR    0x50
#define SENSOR6_CHANNEL 3



// VL53L5CX
SparkFun_VL53L5CX Sensor1;
VL53L5CX_ResultsData Data1;

SparkFun_VL53L5CX Sensor2;
VL53L5CX_ResultsData Data2;

/**
* @brief Array de configuração do sensor VL53L5CX.
* * @details Este array contém o mapeamento de hardware para cada instância 
* do sensor, incluindo ponteiro de dados, pinos de reset,
* canais de comunicação e endereçamentos I2C/TCA.
*/
SensorConfig_VL53L5CX config_VL53L5CX[] = {
    { &Sensor1, &Data1, SENSOR1_RST, SENSOR1_CHANNEL, TCA1_ADDR, SENSOR1_ADDR },
    { &Sensor2, &Data2, SENSOR2_RST, SENSOR2_CHANNEL, TCA1_ADDR, SENSOR2_ADDR }
};

// VL53L4CD
VL53L4CD Sensor3(&Wire, SENSOR3_SHUT);
VL53L4CD Sensor4(&Wire, SENSOR4_SHUT);
VL53L4CD Sensor5(&Wire, SENSOR5_SHUT);
VL53L4CD Sensor6(&Wire, SENSOR6_SHUT);


/**
* @brief Array de configuração do sensor VL53L4CD.
* * @details Este array contém o mapeamento de hardware para cada instância 
* do sensor, incluindo ponteiro de dados, pinos de reset,
* canais de comunicação e endereçamentos I2C/TCA.
*/
SensorConfig_VL53L4CD config_VL53L4CD[] = {
    { &Sensor3, SENSOR3_SHUT, SENSOR3_CHANNEL, TCA2_ADDR, SENSOR3_ADDR },
    { &Sensor4, SENSOR4_SHUT, SENSOR4_CHANNEL, TCA2_ADDR, SENSOR4_ADDR },
    { &Sensor5, SENSOR5_SHUT, SENSOR5_CHANNEL, TCA2_ADDR, SENSOR5_ADDR },
    { &Sensor6, SENSOR6_SHUT, SENSOR6_CHANNEL, TCA2_ADDR, SENSOR6_ADDR }
};


/**
* @brief Inicializa o sistema, comunicação e periféricos.
* * @details Esta função estabelece conexão com a rede WiFi e com a Firebase do projeto, 
* sincroniza o tempo da operação, reseta o multiplexador I2C (MUX) e realiza a 
* varredura para inicialização dos sensores VL53L5CX e VL53L4CD.
*/
void setup() {

    // --- Conectividade WiFi ---
    Serial.begin(115200);
    Wire.begin();
    Wire.setClock(400000);

    Serial.println("Conectando WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi conectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    // --- Firebase ---
    initTime();

    if (firebaseInit()) {
        Serial.println("Firebase inicializado!");
    } else {
        Serial.println("Erro ao iniciar Firebase");
    }

    if (firebasePing()) {
        Serial.println("PING Firebase OK");
    } else {
        Serial.print("Erro Firebase: ");
        Serial.println(fbdo.errorReason());
    }

    // --- Hardware e Sensores ---
    ResetMUX(TCA_RST);

    // Inicializa todos os sensores mapeados nos arrays de configuração
    for (auto &cfg : config_VL53L5CX)
        InitSensor_VL53L5CX(cfg, DEFAULT_RES, DEFAULT_FREQ);

    for (auto &cfg : config_VL53L4CD)
        InitSensor_VL53L4CD(cfg);

    Serial.println();

    delay(1000);
    
   
}

/**
* @brief Ciclo principal de execução do sistema.
 * * @details Realiza a leitura cíclica de todos os sensores VL53L5CX e VL53L4CD conforme  
 * listado em suas tabelas de configuração. Os dados lidos são processados imediatamente e, 
 * em seguida, a função gerencia o consumo de cada uma das filas de transmissão para 
 * garantir o envio dos dados acumulados ao Firebase respeitando o tempo de rede.
 * @note O atraso de 200ms ao fim do loop ajuda a estabilizar as comunicações I2C e WiFi.
 */
void loop() {

    VL53L5CX_Data d5;

    for (auto &cfg : config_VL53L5CX) {
        if (ReadSensor_VL53L5CX_Data(cfg, d5)) {
            processL5CX(d5);
            // queueL5CX(d5);
        }
    }

    VL53L4CD_Data d4;
    
    for (auto &cfg : config_VL53L4CD) {
        if (ReadSensor_VL53L4CD_Data(cfg, d4)) {
            processL4CD(d4);
            // queueL4CD(d4);
        }
    }

    processL4CDQueue();
    processL5CXQueue();

    delay(200);
}
