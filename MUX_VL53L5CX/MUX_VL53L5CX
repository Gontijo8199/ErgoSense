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

#define DEFAULT_RES     8*8 // pode ser 4x4 também
#define DEFAULT_FREQ    15

// Objetos sensores e buffers
SparkFun_VL53L5CX Sensor1;
VL53L5CX_ResultsData Data1;

SparkFun_VL53L5CX Sensor2;
VL53L5CX_ResultsData Data2;

// struct de configuração por sensor
struct SensorConfig {
  SparkFun_VL53L5CX* sensor;      // ponteiro para o objeto sensor
  VL53L5CX_ResultsData* data;     // ponteiro para o buffer de resultados
  uint8_t rstPin;                 // pino RST do sensor
  uint8_t muxChannel;             // canal do TCA9548A
  uint8_t addr;                   // endereço I2C desejado após renomear
};

SensorConfig configs[] = {
  { &Sensor1, &Data1, SENSOR1_RST, SENSOR1_CHANNEL, SENSOR1_ADDR },
  { &Sensor2, &Data2, SENSOR2_RST, SENSOR2_CHANNEL, SENSOR2_ADDR }
};

int imageResolution_Sensor1;
int imageWidth_Sensor1;

int imageResolution_Sensor2;
int imageWidth_Sensor2;

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

void ResetSensor(uint8_t pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH); 
  delay(1);

  digitalWrite(pin, LOW);  
  delay(10);
}

// Setup

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000); 

  ResetMUX();

  Serial.println("Diagnostico do MUX");

  // DiagnoseMUX(); // se quiser ler todos canais no inicio

  Serial.println("Primeiro Sensor");

  SelectChannel(configs[0].muxChannel);
  ResetSensor(configs[0].rstPin);

  delay(100);

  ScanI2C(); 

  Serial.println("Tentando inicializar Sensor1 em 0x29...");
  if (Sensor1.begin(0x29)) {

    Serial.println("Sucesso ao inicializar em 0x29, renomeando...");

    if (!Sensor1.setAddress(configs[0].addr)) {
      Serial.println("Falha ao mudar endereço do Sensor1");
      while (1);
    }

    Serial.print("Sensor1 agora em endereço 0x");
    Serial.println(configs[0].addr, HEX);
    Sensor1.setRangingFrequency(DEFAULT_FREQ);
    Sensor1.setResolution(DEFAULT_RES); 
    Sensor1.startRanging();
    
  } 
  else{
    Serial.println("Falha ao inicializar Sensor1 em 0x29...");

    Serial.println("Tentando inicializar Sensor1 em 0x45...");
    if (Sensor1.begin(configs[0].addr)) {
      Serial.println("Sucesso ao inicializar em 0x45");
      Sensor1.setRangingFrequency(DEFAULT_FREQ);
      Sensor1.setResolution(DEFAULT_RES); 
      Sensor1.startRanging();
    } else {
      Serial.println("Falha ao inicializar em ambos endereços conhecidos.");
    }
  }

  imageResolution_Sensor1 = Sensor1.getResolution();
  imageWidth_Sensor1 = sqrt(imageResolution_Sensor1);

  Serial.println("Segundo Sensor");

  SelectChannel(configs[1].muxChannel);

  delay(100);

  ResetSensor(configs[1].rstPin); 

  delay(100);

  ScanI2C();

  if (Sensor2.begin(0x29)) {

    Serial.println("Sucesso ao inicializar em 0x29, renomeando...");

    if (!Sensor2.setAddress(configs[1].addr)) {
      Serial.println("Falha ao mudar endereço do Sensor2");
      while (1);
    }

    Serial.print("Sensor2 agora em endereço 0x");
    Serial.println(configs[1].addr, HEX);
    
    Sensor2.setRangingFrequency(DEFAULT_FREQ);
    Sensor2.setResolution(DEFAULT_RES); 
    Sensor2.startRanging();

    
  } 
  else{
    Serial.println("Falha ao inicializar Sensor2 em 0x29...");

    Serial.println("Tentando inicializar Sensor2 em 0x46...");
    if (Sensor2.begin(configs[1].addr)) {
      Serial.println("Sucesso ao inicializar em 0x46");
      
      Sensor2.setRangingFrequency(DEFAULT_FREQ);
      Sensor2.setResolution(DEFAULT_RES); 
      Sensor2.startRanging();
    } else {
      Serial.println("Falha ao inicializar em ambos endereços conhecidos.");
      while (1);
    }
  }

  imageResolution_Sensor2 = Sensor2.getResolution();
  imageWidth_Sensor2 = sqrt(imageResolution_Sensor2);

  Serial.println("Ambos sensores inicializados!");
}

// Loop


void loop() {
  // Sensor1
  SelectChannel(configs[0].muxChannel);
  if (Sensor1.isDataReady()) {
    if (Sensor1.getRangingData(&Data1)) {
      for (int y = 0; y <= imageWidth_Sensor1 * (imageWidth_Sensor1 - 1); y += imageWidth_Sensor1) {
        for (int x = imageWidth_Sensor1 - 1; x >= 0; x--) {
          Serial.print("\t1:");
          Serial.print(Data1.distance_mm[x + y]);
        }
        Serial.println();
      }
      Serial.println();
    }
  }

  SelectChannel(configs[1].muxChannel);
  if (Sensor2.isDataReady()) {
    if (Sensor2.getRangingData(&Data2)) {
      for (int y = 0; y <= imageWidth_Sensor2 * (imageWidth_Sensor2 - 1); y += imageWidth_Sensor2) {
        for (int x = imageWidth_Sensor2 - 1; x >= 0; x--) {
          Serial.print("\t2:");
          Serial.print(Data2.distance_mm[x + y]);
        }
        Serial.println();
      }
      Serial.println();
    }
  }

  delay(500);
}
