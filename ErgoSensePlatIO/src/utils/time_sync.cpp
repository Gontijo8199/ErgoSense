#include "time_sync.h"
#include <WiFi.h>
#include <time.h>

void initTime() {
    // UTC-3, sem horário de verão
    const long gmtOffset_sec = -3 * 3600;
    const int daylightOffset_sec = 0;
    configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org", "time.nist.gov");

    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
        Serial.println("Falha ao obter hora via NTP");
    } else {
        Serial.println("Hora sincronizada via NTP");
    }
}

unsigned long getEpoch() {
    return time(nullptr); 
}
