#include <Wire.h>
#include <SparkFun_VL53L5CX_Library.h>

// Mux
#define TCA_ADDR        0x70
#define TCA_RST         12      

// Sensores
#define SENSOR1_RST     13
#define SENSOR1_ADDR    0x45
#define SENSOR1_CHANNEL 0

#define SENSOR2_RST     14
#define SENSOR2_ADDR    0x46
#define SENSOR2_CHANNEL 1

#define DEFAULT_RES     8*8
#define DEFAULT_FREQ    15

// Objetos sensores e buffers
SparkFun_VL53L5CX Sensor1;
VL53L5CX_ResultsData Data1;

SparkFun_VL53L5CX Sensor2;
VL53L5CX_ResultsData Data2;

// struct de configuração por sensor
struct SensorConfig {
  SparkFun_VL53L5CX* sensor;
  VL53L5CX_ResultsData* data;
  uint8_t rstPin;
  uint8_t muxChannel;
  uint8_t addr;

  int resolution;
  int width;
};

// Lista de sensores (adicione mais aqui)
SensorConfig configs[] = {
  { &Sensor1, &Data1, SENSOR1_RST, SENSOR1_CHANNEL, SENSOR1_ADDR },
  { &Sensor2, &Data2, SENSOR2_RST, SENSOR2_CHANNEL, SENSOR2_ADDR }
};

// MUX / I2C helpers 
void SelectChannel(uint8_t channel) {
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(1 << channel);
  Wire.endTransmission();
}

void ResetMUX() {
  pinMode(TCA_RST, OUTPUT);
  digitalWrite(TCA_RST, LOW);
  delay(5);
  digitalWrite(TCA_RST, HIGH);
  delay(5);
}

void ScanI2C() {
  for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    uint8_t error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("  -> Dispositivo achado em 0x");
      Serial.println(addr, HEX);
    }
  }
}

void DiagnoseMUX() {
  Serial.println("\n--- Diagnóstico MUX ---");

  Wire.beginTransmission(TCA_ADDR);
  if (Wire.endTransmission() != 0) {
    Serial.println("MUX não encontrado");
    return;
  }

  for (uint8_t ch = 0; ch < 8; ch++) {
    Serial.print("\nCanal ");
    Serial.println(ch);

    SelectChannel(ch);
    delay(5);
    ScanI2C();
  }

  Serial.println("\n--- Fim ---");
}

// Sensor reset
void ResetSensor(uint8_t pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH); 
  delay(1);
  digitalWrite(pin, LOW);  
  delay(10);
}

// Inicialização genérica
void InitSensor(SensorConfig &cfg) {

  Serial.println("\n--- Inicializando Sensor ---");

  SelectChannel(cfg.muxChannel);
  ResetSensor(cfg.rstPin);
  delay(100);

  ScanI2C(); 

  Serial.print("Tentando inicializar Sensor em 0x29 no canal ");
  Serial.println(cfg.muxChannel);

  if (cfg.sensor->begin(0x29)) {

    Serial.println("Sucesso ao inicializar em 0x29, renomeando...");

    if (!cfg.sensor->setAddress(cfg.addr)) {
      Serial.println("Falha ao mudar endereço do sensor");
      while (1);
    }

    Serial.print("Sensor agora em endereço 0x");
    Serial.println(cfg.addr, HEX);
  } 
  else {
    Serial.println("Falha ao inicializar em 0x29...");

    Serial.print("Tentando inicializar em 0x");
    Serial.println(cfg.addr, HEX);

    if (cfg.sensor->begin(cfg.addr)) {
      Serial.print("Sucesso ao inicializar em 0x");
      Serial.println(cfg.addr, HEX);
    } else {
      Serial.println("Falha ao inicializar em ambos endereços conhecidos.");
      while (1);
    }
  }

  cfg.sensor->setRangingFrequency(DEFAULT_FREQ);
  cfg.sensor->setResolution(DEFAULT_RES); 
  cfg.sensor->startRanging();

  cfg.resolution = cfg.sensor->getResolution();
  cfg.width = sqrt(cfg.resolution);

  //Serial.print("Resolução: ");
  //Serial.print(cfg.resolution);
  //Serial.print("  Width: ");
  //Serial.println(cfg.width);

  Serial.println("--- Sensor inicializado com sucesso ---\n");
}

// Setup
void setup() {

  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000); 

  ResetMUX();

  int numSensores = sizeof(configs) / sizeof(configs[0]);

  for (int i = 0; i < numSensores; i++) {
    InitSensor(configs[i]);
  }

  Serial.println("Todos sensores inicializados!");
}

// Loop 

void loop() {

  int numSensores = sizeof(configs) / sizeof(configs[0]);

  for (int i = 0; i < numSensores; i++) {

    SelectChannel(configs[i].muxChannel);

    if (configs[i].sensor->isDataReady()) {

      if (configs[i].sensor->getRangingData(configs[i].data)) {

        Serial.print("\n--- Sensor ");
        Serial.print(i + 1);
        Serial.println(" ---");

        int width = configs[i].width;

        for (int y = 0; y <= width * (width - 1); y += width) {
          for (int x = width - 1; x >= 0; x--) {
            Serial.print("\t");
            Serial.print(i + 1);
            Serial.print(":");
            Serial.print(configs[i].data->distance_mm[x + y]);
          }
          Serial.println();
        }
        Serial.println();
      }
    }
  }

  delay(500);
}
