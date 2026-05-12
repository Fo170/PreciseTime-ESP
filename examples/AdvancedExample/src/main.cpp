/**
 * @file main.cpp
 * @brief Exemple avancé d'utilisation de PreciseTime
 * @example AdvancedExample.ino
 * @version 1.0.1
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

// Périodes des différentes tâches
#define DISPLAY_INTERVAL_MS    2000    // Affichage toutes les 2 secondes
#define LED_BLINK_INTERVAL_MS  500     // LED toutes les 500ms
#define TASK_INTERVAL_MS       100     // Tâche périodique toutes les 100ms

// Variables d'état
bool ledState = false;
int taskCounter = 0;

/**
 * @brief Mesure le temps d'exécution d'une tâche
 */
void measureTaskExecution() {
    uint64_t startTime = PreciseTime::getMicroseconds();
    
    // Simuler une tâche qui prend du temps
    volatile long result = 0;
    for (int i = 0; i < 500; i++) {
        result += i * i;
        delayMicroseconds(5);
    }
    
    uint64_t endTime = PreciseTime::getMicroseconds();
    uint64_t duration = endTime - startTime;
    
    // Afficher occasionnellement la durée
    taskCounter++;
    if (taskCounter % 10 == 0) {
        Serial.printf("[Tâche %d] Exécutée en %llu µs (%.3f ms)\n", 
                     taskCounter, duration, duration / 1000.0);
    }
}

/**
 * @brief Affiche des informations détaillées sur le temps
 */
void displayDetailedTime() {
    Serial.println("\n=== 🕒 INFORMATIONS TEMPS ===");
    
    // Différentes représentations du temps
    Serial.printf("Formaté:        %s\n", PreciseTime::getFormattedString().c_str());
    Serial.printf("Microsecondes:  %llu µs\n", PreciseTime::getMicroseconds());
    Serial.printf("Millisecondes:  %llu ms\n", PreciseTime::getMilliseconds());
    Serial.printf("Secondes:       %llu s\n", PreciseTime::getSeconds());
    Serial.printf("Secondes préc.: %.6f s\n", PreciseTime::getSecondsPrecise());
    
    // Composantes individuelles
    uint64_t days;
    uint32_t hours, minutes, seconds;
    PreciseTime::getFormattedTime(days, hours, minutes, seconds);
    
    if (days > 0) {
        Serial.printf("Détails:        %llu jours, %lu heures\n", days, hours);
    }
    
    // Calcul du pourcentage d'utilisation
    double percent = (PreciseTime::getSecondsPrecise() / 3600.0 / 24.0 / 365.0) / 
                     PreciseTime::getOverflowYears() * 100.0;
    Serial.printf("Utilisation:    %.12f%%\n", percent);
    Serial.println("==========================\n");
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("╔════════════════════════════════════════╗");
    Serial.println("║     Exemple Avancé - PreciseTime       ║");
    Serial.println("║           Version 1.0.1 - 2025         ║");
    Serial.println("╚════════════════════════════════════════╝");
    
    // Initialiser le chronométrage
    PreciseTime::begin();
    
    // Configurer la LED
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    
    Serial.println("Commandes disponibles:");
    Serial.println("  'r' - Réinitialiser le chronomètre");
    Serial.println("  's' - Afficher l'état du système");
    Serial.println("  't' - Exécuter un test de performance");
    Serial.println();
    
    Serial.println("Démarrage des tâches périodiques...");
}

void loop() {
    static uint64_t lastDisplay = 0;
    static uint64_t lastBlink = 0;
    static uint64_t lastTask = 0;
    uint64_t currentTime = PreciseTime::getMilliseconds();
    
    // 1. Affichage périodique
    if (currentTime - lastDisplay >= DISPLAY_INTERVAL_MS) {
        lastDisplay = currentTime;
        displayDetailedTime();
    }
    
    // 2. Clignotement LED
    if (currentTime - lastBlink >= LED_BLINK_INTERVAL_MS) {
        lastBlink = currentTime;
        ledState = !ledState;
        digitalWrite(LED_BUILTIN, ledState);
    }
    
    // 3. Tâche périodique avec mesure de temps
    if (currentTime - lastTask >= TASK_INTERVAL_MS) {
        lastTask = currentTime;
        measureTaskExecution();
    }
    
    // 4. Gestion des commandes série
    if (Serial.available() > 0) {
        char command = Serial.read();
        
        switch (command) {
            case 'r':
            case 'R':
                PreciseTime::reset();
                Serial.println("✅ Chronomètre réinitialisé");
                lastDisplay = lastBlink = lastTask = 0;
                break;
                
            case 's':
            case 'S':
                Serial.print("État système: ");
                Serial.println(PreciseTime::isInitialized() ? "Initialisé" : "Non initialisé");
                Serial.printf("Mémoire libre: %d bytes\n", ESP.getFreeHeap());
                break;
                
            case 't':
            case 'T':
                Serial.println("🚀 Test de performance en cours...");
                uint64_t start = PreciseTime::getMicroseconds();
                for (int i = 0; i < 1000; i++) {
                    volatile int x = i * i;
                }
                uint64_t end = PreciseTime::getMicroseconds();
                Serial.printf("Test terminé en %llu µs\n", end - start);
                break;
        }
    }
    
    // Mise à jour manuelle pour architectures non-ESP
    #if !defined(ESP32) && !defined(ESP8266)
    PreciseTime::update();
    #endif
    
    delay(1);
}
