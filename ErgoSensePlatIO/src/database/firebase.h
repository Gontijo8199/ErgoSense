#pragma once

#include <Firebase_ESP_Client.h>
#include <WiFi.h>

extern FirebaseData fbdo;
extern FirebaseAuth auth;
extern FirebaseConfig config;

bool firebaseInit();
bool firebasePing();
