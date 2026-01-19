#include "firebase.h"
#include "credentials.h"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

bool firebaseInit() {
    Serial.println("Inicializando Firebase...");

    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;

    // login anonimo
    if (!Firebase.signUp(&config, &auth, "", "")) {
        Serial.print("Signup failed: ");
        Serial.println(config.signer.signupError.message.c_str());
        return false;
    }

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    unsigned long start = millis();
    while (!Firebase.ready() && millis() - start < 8000) {
        Serial.print(".");
        delay(300);
    }
    Serial.println();

    if (!Firebase.ready()) {
        Serial.println("Firebase NÃO ficou pronto");
        Serial.println(fbdo.errorReason());
        return false;
    }

    Serial.println("Firebase pronto");
    return true;
}




bool firebasePing() {
    return Firebase.RTDB.setBool(&fbdo, "/ping", true);
}


