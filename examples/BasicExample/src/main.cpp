/**
 * @file main.cpp
 * @brief Exemple basique d'utilisation de PreciseTime
 * @example BasicExample.ino
 * @version 1.0.0
 * @date 2025
 * 
 * Copyright (C) 2025 Fo170
 * 
 * Ce programme est un logiciel libre ; vous pouvez le redistribuer
 * et/ou le modifier selon les termes de la GNU General Public License
 * telle que publiée par la Free Software Foundation ; soit la version 3
 * de la Licence, soit (à votre choix) toute version ultérieure.
 */

#include <Arduino.h>
#include <PreciseTime.h>

void setup() {
    Serial.begin(115200);
    delay(1000); // Attente pour le port série
    
    Serial.println("=== Exemple Basique PreciseTime ===");
    Serial.println("Version 1.0.0 - 2025");
    
    // Initialiser le système de temps
    PreciseTime::begin();
    
    // Afficher les informations système
    Serial.print("Architecture: ");
    #if defined(ESP32)
        Serial.println("ESP32 (1µs résolution)");
    #elif defined(ESP8266)
        Serial.println("ESP8266 (~4µs résolution)");
    #else
        Serial.println("Arduino (1ms résolution)");
    #endif
    
    Serial.println("Le système affichera le temps écoulé toutes les secondes...");
    Serial.println();
}

void loop() {
    static uint64_t lastDisplay = 0;
    
    // Afficher le temps toutes les secondes
    if (PreciseTime::getMilliseconds() - lastDisplay >= 1000) {
        lastDisplay = PreciseTime::getMilliseconds();
        
        Serial.print("Temps écoulé: ");
        Serial.print(PreciseTime::getFormattedString());
        Serial.print(" (");
        Serial.print(PreciseTime::getMilliseconds());
        Serial.println(" ms)");
    }
    
    delay(10);
}