# ErgoSense - Plataforma IoT de Avaliação Postural Contínua


Plataforma IoT para monitoramento ergonômico contínuo, com foco na automação da avaliação de tronco (Step 10) do método RULA. Sensores Time-of-Flight embutidos no encosto de uma cadeira alimentam um firmware ESP32 e um pipeline de coleta/visualização em Python, capturando a postura do usuário de forma contínua e não invasiva. Desenvolvido na EMAp/FGV.


## Pipeline

O ESP32 lê os sensores continuamente em uma task dedicada (`sensorTask`, Core 1) e empilha as leituras em filas circulares. Uma segunda task (`firebaseTask`, Core 0) drena essas filas em lotes e envia ao Firebase Realtime Database, separação que evita que a latência de rede afete a cadência de leitura.

Do lado do servidor, o `pipeline_worker.py` consome os lotes do Firebase periodicamente, exporta para CSV e limpa o nó correspondente. O `dashboard_ergo.py` lê esses CSVs e reproduz as leituras como um replay animado, útil para inspeção e validação dos dados coletados.

## Hardware

| Componente | Qtd | Função |
|---|---|---|
| ESP32 Thing Plus C | 1 | Microcontrolador |
| VL53L5CX (imager 8×8) | 2 | Mapa espacial do tronco |
| VL53L4CD (ponto único) | 4 | Distância pontual encosto–usuário |
| TCA9548A (mux I²C) | 2 | Isolamento de barramento |

I²C a 40 kHz (GPIO 21/22), pull-ups de 4,7 kΩ, frequência reduzida para compensar a capacitância dos cabos até o encosto.

## Estrutura

```
ErgoSense/
├── ErgoSensePlatIO/       # Firmware (PlatformIO)
│   ├── include/           # board_config.h, system_config.h, credentials.example.h
│   └── src/                # config/ sensors/ processing/ database/ tasks/ utils/
├── python/
│   ├── pipeline_worker/   # Firebase -> CSV
│   └── dashboard_ergo.py 
└── .github/workflows/ci.yml
```

## Execução

```bash
# Firmware
cd ErgoSensePlatIO
cp include/credentials.example.h include/credentials.h   # preencha com suas credenciais
pio run && pio run -t upload

# Pipeline
cd python/pipeline_worker && python pipeline_worker.py    # requer chave.json

# Dashboard
cd python && python dashboard_ergo.py                     # requer os CSVs gerados
```

## CI

`.github/workflows/ci.yml`: `firmware-build` (compila via PlatformIO) e `python-lint` (`flake8`).

## Referências

Andre, R. P., Ferreira, R. G., Sodre, D. D., & Miranda, L. S. *Bridging Subjective Ergonomics and Objective Sensing: An IoT-Enabled Approach Toward Continuous RULA Posture Assessment*.

