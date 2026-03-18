#pragma once
#include <Arduino.h>

// Inicializa a sincronização de tempo via NTP
void initTime();

// Retorna o timestamp atual em segundos desde 1970
unsigned long getEpoch();
