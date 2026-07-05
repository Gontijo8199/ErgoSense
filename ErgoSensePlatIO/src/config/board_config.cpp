#include "board_config.h"
#include "system_config.h"
#include "mux.h"

// VL53L5CX (multizona)
#define SENSOR1_RST     13
#define SENSOR1_ADDR    0x45
#define SENSOR1_CHANNEL 0

#define SENSOR2_RST     14
#define SENSOR2_ADDR    0x46
#define SENSOR2_CHANNEL 1

static SparkFun_VL53L5CX Sensor1;
static VL53L5CX_ResultsData Data1;

static SparkFun_VL53L5CX Sensor2;
static VL53L5CX_ResultsData Data2;

SensorConfig_VL53L5CX config_VL53L5CX[] =
{
    { &Sensor1, &Data1, SENSOR1_RST, SENSOR1_CHANNEL, TCA1_ADDR, SENSOR1_ADDR },
    { &Sensor2, &Data2, SENSOR2_RST, SENSOR2_CHANNEL, TCA1_ADDR, SENSOR2_ADDR },
};

// VL53L4CD (ponto único)
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

static VL53L4CD Sensor3(&Wire, SENSOR3_SHUT);
static VL53L4CD Sensor4(&Wire, SENSOR4_SHUT);
static VL53L4CD Sensor5(&Wire, SENSOR5_SHUT);
static VL53L4CD Sensor6(&Wire, SENSOR6_SHUT);

SensorConfig_VL53L4CD config_VL53L4CD[] =
{
    { &Sensor3, SENSOR3_SHUT, SENSOR3_CHANNEL, TCA2_ADDR, SENSOR3_ADDR, 0 },
    { &Sensor4, SENSOR4_SHUT, SENSOR4_CHANNEL, TCA2_ADDR, SENSOR4_ADDR, 0 },
    { &Sensor5, SENSOR5_SHUT, SENSOR5_CHANNEL, TCA2_ADDR, SENSOR5_ADDR, 0 },
    { &Sensor6, SENSOR6_SHUT, SENSOR6_CHANNEL, TCA2_ADDR, SENSOR6_ADDR, 0 },
};

const int NUM_L5CX = sizeof(config_VL53L5CX) / sizeof(config_VL53L5CX[0]);
const int NUM_L4CD = sizeof(config_VL53L4CD) / sizeof(config_VL53L4CD[0]);

void initBoardSensors()
{
    ResetMUX(TCA_RST);

    Serial.println("Inicializando VL53L5CX");
    for (int i = 0; i < NUM_L5CX; i++)
        InitSensor_VL53L5CX(config_VL53L5CX[i], DEFAULT_RES, DEFAULT_FREQ);

    delay(500);

    Serial.println("Inicializando VL53L4CD");
    for (int i = 0; i < NUM_L4CD; i++)
        InitSensor_VL53L4CD(config_VL53L4CD[i]);

    delay(500);
}
